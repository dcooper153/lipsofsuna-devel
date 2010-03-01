/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup liser Server
 * @{
 * \addtogroup LISerObject Object
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/system.h>
#include "server.h"
#include "server-callbacks.h"
#include "server-object.h"
#include "server-observer.h"
#include "server-script.h"

/**
 * \brief Purges the object from the object database.
 *
 * \param self Object.
 * \param server Server.
 * \return Nonzero on success.
 */
int
liser_object_purge (LIEngObject* self,
                    LISerServer* server)
{
	int ret;
	const char* query;
	LIArcSql* sql;
	sqlite3_stmt* statement;

	ret = 1;
	sql = server->sql;

	/* Remove from the main table. */
	query = "DELETE FROM objects WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return ret;
}

/**
 * \brief Serializes or deserializes the object.
 * 
 * \param self Object.
 * \param server Server.
 * \param save Nonzero if saving, zero if reading.
 * \return Nonzero on success.
 */
int
liser_object_serialize (LIEngObject* self,
                        LISerServer* server,
                        int          save)
{
	int ret;
	int col;
	int colgroup;
	int colmask;
	int control;
	int flags;
	int sector;
	float mass;
	float movement;
	float speed;
	float step;
	const char* extra;
	const char* model;
	const char* type;
	const char* query;
	LIArcSql* sql;
	LIMatTransform transform;
	LIMatVector angular;
	LIScrScript* script;
	sqlite3_stmt* statement;

	sql = server->sql;

	if (!save)
	{
		/* Prepare statement. */
		query = "SELECT "
			"flags,angx,angy,angz,posx,posy,posz,rotx,roty,rotz,rotw,"
			"mass,move,speed,step,colgrp,colmsk,control,model,type,extra "
			"FROM objects WHERE id=?;";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			return 0;
		}
		if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Execute statement. */
		ret = sqlite3_step (statement);
		if (ret == SQLITE_DONE)
		{
			sqlite3_finalize (statement);
			return 1;
		}
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read columns. */
		col = 0;
		flags = sqlite3_column_int (statement, col++);
		angular.x = sqlite3_column_double (statement, col++);
		angular.y = sqlite3_column_double (statement, col++);
		angular.z = sqlite3_column_double (statement, col++);
		transform.position.x = sqlite3_column_double (statement, col++);
		transform.position.y = sqlite3_column_double (statement, col++);
		transform.position.z = sqlite3_column_double (statement, col++);
		transform.rotation.x = sqlite3_column_double (statement, col++);
		transform.rotation.y = sqlite3_column_double (statement, col++);
		transform.rotation.z = sqlite3_column_double (statement, col++);
		transform.rotation.w = sqlite3_column_double (statement, col++);
		mass = sqlite3_column_double (statement, col++);
		movement = sqlite3_column_double (statement, col++);
		speed = sqlite3_column_double (statement, col++);
		step = sqlite3_column_double (statement, col++);
		colgroup = sqlite3_column_int (statement, col++);
		colmask = sqlite3_column_int (statement, col++);
		control = sqlite3_column_int (statement, col++);
		if (sqlite3_column_type (statement, col) == SQLITE_TEXT)
			model = (char*) sqlite3_column_text (statement, col++);
		else
			model = NULL;
		if (sqlite3_column_type (statement, col) == SQLITE_TEXT)
			type = (char*) sqlite3_column_text (statement, col++);
		else
			type = NULL;
		if (sqlite3_column_type (statement, col) == SQLITE_TEXT)
			extra = (char*) sqlite3_column_text (statement, col++);
		else
			extra = NULL;

		/* Set state. */
		lieng_object_set_angular (self, &angular);
		lieng_object_set_transform (self, &transform);
		lieng_object_set_mass (self, mass);
		liphy_object_set_movement (self->physics, movement);
		lieng_object_set_speed (self, speed);
		lieng_object_set_collision_group (self, colgroup);
		lieng_object_set_collision_mask (self, colmask);
		liphy_object_set_control_mode (self->physics, control);
		if (model != NULL)
			lieng_object_set_model_name (self, model);

		/* Process script values. */
		if (self->script != NULL)
		{
			script = server->script;
			liscr_pushdata (script->lua, self->script);
			lua_getfield (script->lua, -1, "read_cb");
			if (lua_type (script->lua, -1) == LUA_TFUNCTION)
			{
				lua_pushvalue (script->lua, -2);
				lua_remove (script->lua, -3);
				lua_pushstring (script->lua, (type != NULL)? type : "");
				lua_pushstring (script->lua, (extra != NULL)? extra : "");
				if (lua_pcall (script->lua, 3, 0, 0) != 0)
				{
					lisys_error_set (LISYS_ERROR_UNKNOWN, "Object.read_cb: %s", lua_tostring (script->lua, -1));
					lua_pop (script->lua, 1);
					lisys_error_report ();
				}
			}
			else
				lua_pop (script->lua, 2);
		}
		sqlite3_finalize (statement);
	}
	else
	{
		/* Collect values. */
		flags = 0;
		if (lieng_object_get_realized (self))
			sector = self->sector->sector->index;
		else
			sector = -1;
		model = (self->model != NULL)? self->model->name : NULL;
		mass = lieng_object_get_mass (self);
		movement = liphy_object_get_movement (self->physics);
		speed = lieng_object_get_speed (self);
		step = 0.0f;
		colgroup = lieng_object_get_collision_group (self);
		colmask = lieng_object_get_collision_mask (self);
		control = liphy_object_get_control_mode (self->physics);
		lieng_object_get_transform (self, &transform);
		lieng_object_get_angular (self, &angular);
		type = NULL;
		extra = NULL;

		/* Prepare statement. */
		query = "INSERT OR REPLACE INTO objects "
			"(id,sector,flags,angx,angy,angz,posx,posy,posz,rotx,roty,rotz,rotw,"
			"mass,move,speed,step,colgrp,colmsk,control,model,type,extra) VALUES "
			"(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			return 0;
		}

		/* Bind values. */
		col = 1;
		ret = (sqlite3_bind_int (statement, col++, self->id) != SQLITE_OK ||
			sqlite3_bind_int (statement, col++, sector) != SQLITE_OK ||
			sqlite3_bind_int (statement, col++, flags) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, angular.x) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, angular.y) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, angular.x) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, transform.position.x) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, transform.position.y) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, transform.position.z) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, transform.rotation.x) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, transform.rotation.y) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, transform.rotation.z) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, transform.rotation.w) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, mass) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, movement) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, speed) != SQLITE_OK ||
			sqlite3_bind_double (statement, col++, step) != SQLITE_OK ||
			sqlite3_bind_int (statement, col++, colgroup) != SQLITE_OK ||
			sqlite3_bind_int (statement, col++, colmask) != SQLITE_OK ||
			sqlite3_bind_int (statement, col++, control) != SQLITE_OK);
		if (!ret)
		{
			if (model != NULL)
				ret = (sqlite3_bind_text (statement, col++, model, -1, SQLITE_TRANSIENT) != SQLITE_OK);
			else
				ret = (sqlite3_bind_null (statement, col++) != SQLITE_OK);
		}
		if (ret)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Bind script values. */
		script = NULL;
		if (self->script != NULL)
		{
			script = server->script;
			liscr_pushdata (script->lua, self->script);
			lua_getfield (script->lua, -1, "write_cb");
			if (lua_type (script->lua, -1) == LUA_TFUNCTION)
			{
				lua_pushvalue (script->lua, -2);
				lua_remove (script->lua, -3);
				if (lua_pcall (script->lua, 1, 2, 0) != 0)
				{
					lisys_error_set (LISYS_ERROR_UNKNOWN, "Object.write_cb: %s", lua_tostring (script->lua, -1));
					lua_pop (script->lua, 1);
					return 0;
				}
				if (lua_type (script->lua, -2) == LUA_TSTRING)
					type = lua_tostring (script->lua, -2);
				else
					lua_remove (script->lua, -2);
				if (lua_type (script->lua, -1) == LUA_TSTRING)
					extra = lua_tostring (script->lua, -1);
				else
					lua_remove (script->lua, -1);
			}
			else
				lua_pop (script->lua, 2);
		}
		if (type != NULL)
		{
			assert (script != NULL);
			ret = (sqlite3_bind_text (statement, col++, type, -1, SQLITE_TRANSIENT) != SQLITE_OK);
			lua_pop (script->lua, 1);
		}
		else
			ret = (sqlite3_bind_null (statement, col++) != SQLITE_OK);
		if (extra != NULL)
		{
			assert (script != NULL);
			ret = (ret || sqlite3_bind_text (statement, col++, extra, -1, SQLITE_TRANSIENT) != SQLITE_OK);
			lua_pop (script->lua, 1);
		}
		else
			ret = (ret || sqlite3_bind_null (statement, col++) != SQLITE_OK);
		if (ret)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Write values. */
		if (sqlite3_step (statement) != SQLITE_DONE)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		sqlite3_finalize (statement);
	}

	return 1;
}

/** @} */
/** @} */
