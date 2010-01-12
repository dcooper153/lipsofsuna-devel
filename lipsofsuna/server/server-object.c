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
#include "server-client.h"
#include "server-object.h"
#include "server-observer.h"
#include "server-script.h"

static int
private_delete_animations (LIEngObject* self);

static int
private_read_animations (LIEngObject* self);

static int
private_write_animations (LIEngObject* self);

/*****************************************************************************/

/**
 * \brief Plays back an animation.
 *
 * \param self Object.
 * \param name Animation name or NULL.
 * \param channel Channel to use, -1 for the largest free channel number.
 * \param priority Blending priority.
 * \param permanent Nonzero if permanent, zero if temporary.
 * \return Nonzero on success.
 */
int
liser_object_animate (LIEngObject* self,
                      const char*  name,
                      int          channel,
                      float        priority,
                      int          permanent)
{
	LIEngAnimation* animation;
	LISerAniminfo tmp;
	LISerAniminfo* info;
	LISerObject* data = LISER_OBJECT (self);

	/* Find animation channel. */
	if (channel == -1)
	{
		for (channel = 254 ; channel > 0 ; channel--)
		{
			if (!lialg_u32dic_find (data->animations, channel))
				break;
		}
	}

	/* Find animation number. */
	if (name != NULL)
		animation = lieng_engine_find_animation_by_name (self->engine, name);
	else
		animation = NULL;

	/* Check for duplicate. */
	info = lialg_u32dic_find (data->animations, channel);
	if (info != NULL)
	{
		if (info->animation == animation &&
		    info->channel == channel &&
		    info->permanent == permanent &&
		    info->priority == priority)
			return 1;
	}

	/* Clear channel. */
	if (info != NULL)
	{
		lialg_u32dic_remove (data->animations, channel);
		lisys_free (info);
	}
	if (name != NULL && animation == NULL)
		return 0;

	/* Register channel if permanent. */
	if (permanent)
	{
		info = lisys_calloc (1, sizeof (LISerAniminfo));
		if (info == NULL)
			return 0;
		info->animation = animation;
		info->channel = channel;
		info->permanent = permanent;
		info->priority = priority;
		if (!lialg_u32dic_insert (data->animations, channel, info))
		{
			lisys_free (info);
			return 0;
		}
	}

	/* Animate pose. */
	lieng_object_set_animation (self, channel, name, permanent? -1 : 1, priority);

	/* Invoke callbacks. */
	if (lieng_object_get_realized (self))
	{
		tmp.animation = animation;
		tmp.channel = channel;
		tmp.permanent = permanent;
		tmp.priority = priority;
		lical_callbacks_call (self->engine->callbacks, self->engine, "object-animation", lical_marshal_DATA_PTR_PTR, self, &tmp);
	}

	return 1;
}

/**
 * \brief Disconnects the client controlling the object.
 *
 * \param self Object.
 */
void
liser_object_disconnect (LIEngObject* self)
{
	LISerObject* data = LISER_OBJECT (self);

	if (data->client != NULL)
	{
		lical_callbacks_call (self->engine->callbacks, self->engine, "client-logout", lical_marshal_DATA_PTR, self);
		liser_client_free (data->client);
		data->client = NULL;
		lieng_object_ref (self, -1);
	}
}

/**
 * \brief Emit a sound or graphical effect.
 *
 * \param self Object.
 * \param value Effect name.
 * \param flags Effect flags.
 */
void
liser_object_effect (LIEngObject* self,
                     const char*  value,
                     int          flags)
{
	LIEngSample* sample;
	LISerObject* data = LISER_OBJECT (self);

	/* Find effect. */
	if (!lieng_object_get_realized (self))
		return;
	sample = lieng_resources_find_sample_by_name (data->server->engine->resources, value);
	if (sample == NULL)
		return;

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-effect", lical_marshal_DATA_PTR_PTR_INT, self, sample, flags);
}

/**
 * \brief Purges the object from the object database.
 *
 * \param self Object.
 * \return Nonzero on success.
 */
int
liser_object_purge (LIEngObject* self)
{
	int ret;
	const char* query;
	LIArcSql* sql;
	sqlite3_stmt* statement;

	ret = 1;
	sql = LISER_OBJECT (self)->server->sql;

	/* Remove from helper tables. */
	if (!private_delete_animations (self))
		ret = 0;

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
 * \param save Nonzero if saving, zero if reading.
 * \return Nonzero on success.
 */
int
liser_object_serialize (LIEngObject* self,
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
	const char* model;
	const char* type;
	const char* extras;
	const char* query;
	LIArcSql* sql;
	LIMatTransform transform;
	LIMatVector angular;
	LIScrScript* script;
	sqlite3_stmt* statement;

	sql = LISER_OBJECT (self)->server->sql;

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
			extras = (char*) sqlite3_column_text (statement, col++);
		else
			extras = NULL;

		/* Set state. */
		lieng_object_set_angular_momentum (self, &angular);
		lieng_object_set_transform (self, &transform);
		lieng_object_set_mass (self, mass);
		liphy_object_set_movement (self->physics, movement);
		lieng_object_set_speed (self, speed);
		lieng_object_set_collision_group (self, colgroup);
		lieng_object_set_collision_mask (self, colmask);
		liphy_object_set_control_mode (self->physics, control);
		if (model != NULL)
			lieng_object_set_model_name (self, model);

		/* Read animation data. */
		if (self->script != NULL)
			private_read_animations (self);

		/* Process script values. */
		if (self->script != NULL)
		{
			script = LISER_OBJECT (self)->server->script;
			liscr_pushdata (script->lua, self->script);
			lua_pushstring (script->lua, "read_cb");
			lua_gettable (script->lua, -2);
			if (lua_type (script->lua, -1) == LUA_TFUNCTION)
			{
				lua_pushvalue (script->lua, -2);
				lua_remove (script->lua, -3);
				lua_pushstring (script->lua, (type != NULL)? type : "");
				lua_pushstring (script->lua, (extras != NULL)? extras : "");
				if (lua_pcall (script->lua, 3, 0, 0) != 0)
				{
					lisys_error_set (LISYS_ERROR_UNKNOWN, "Object.read_cb: %s", lua_tostring (script->lua, -1));
					lua_pop (script->lua, 1);
				}
			}
			else
				lua_pop (script->lua, 2);
		}
		sqlite3_finalize (statement);
	}
	else
	{
		if (!private_delete_animations (self))
			return 0;
		if (lieng_object_get_realized (self))
		{
			/* Collect values. */
			flags = LISER_OBJECT (self)->flags;
			sector = self->sector->sector->index;
			model = (self->model != NULL)? self->model->name : NULL;
			mass = lieng_object_get_mass (self);
			movement = liphy_object_get_movement (self->physics);
			speed = lieng_object_get_speed (self);
			step = 0.0f;
			colgroup = lieng_object_get_collision_group (self);
			colmask = lieng_object_get_collision_mask (self);
			control = liphy_object_get_control_mode (self->physics);
			lieng_object_get_transform (self, &transform);
			lieng_object_get_angular_momentum (self, &angular);
			type = NULL;
			extras = NULL;

			/* Prepare statement. */
			query = "INSERT OR REPLACE INTO objects "
				"(id,sector,flags,angx,angy,angz,posx,posy,posz,rotx,roty,rotz,rotw,"
				"mass,move,speed,step,colgrp,colmsk,control,model,type,extra) VALUES "
				"(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
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
				script = LISER_OBJECT (self)->server->script;
				liscr_pushdata (script->lua, self->script);
				lua_pushstring (script->lua, "write_cb");
				lua_gettable (script->lua, -2);
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
						model = lua_tostring (script->lua, -1);
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
			if (model != NULL)
			{
				assert (script != NULL);
				ret = (ret || sqlite3_bind_text (statement, col++, model, -1, SQLITE_TRANSIENT) != SQLITE_OK);
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

			/* Write animation data. */
			if (self->script != NULL)
				private_write_animations (self);
		}
	}

	return 1;
}

int
liser_object_sees (const LIEngObject* self,
                   const LIEngObject* target)
{
	LISerObject* data = LISER_OBJECT (self);

	if (data->client == NULL)
		return 0;
	return (lialg_u32dic_find (data->client->vision, target->id) != NULL);
}

/**
 * \brief Swaps the clients of the objects.
 *
 * \param self Object.
 * \param object Object.
 */
void
liser_object_swap (LIEngObject* self,
                   LIEngObject* object)
{
	LISerClient* tmp;
	LISerObject* data = LISER_OBJECT (self);
	LISerObject* data1 = LISER_OBJECT (object);

	tmp = data->client;
	liser_object_set_client (self, data1->client);
	liser_object_set_client (object, tmp);
}

/**
 * \brief Sets the client controlling the object.
 *
 * \param self Object.
 * \param value Client.
 * \return Nonzero on success.
 */
int
liser_object_set_client (LIEngObject* self,
                         LISerClient* value)
{
	LISerObject* data = LISER_OBJECT (self);

	if (data->client == value)
		return 1;
	data->client = value;
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-client", lical_marshal_DATA_PTR, self);
	if (value != NULL)
	{
		lieng_object_ref (self, 1);
		return liser_client_set_object (value, self);
	}
	else
	{
		lieng_object_ref (self, -1);
		return 1;
	}
}

/*****************************************************************************/

static int
private_delete_animations (LIEngObject* self)
{
	const char* query;
	LIArcSql* sql;
	sqlite3_stmt* statement;

	sql = LISER_OBJECT (self)->server->sql;

	/* Clear animation data. */
	query = "DELETE from object_anims WHERE id=?;";
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

	return 1;
}

static int
private_read_animations (LIEngObject* self)
{
	int ret;
	int chan;
	float prio;
	const char* name;
	const char* query;
	LIArcSql* sql;
	sqlite3_stmt* statement;

	sql = LISER_OBJECT (self)->server->sql;

	/* Prepare statement. */
	query = "SELECT name,chan,prio FROM object_anims WHERE id=?;";
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
	while (1)
	{
		ret = sqlite3_step (statement);
		if (ret == SQLITE_DONE)
		{
			sqlite3_finalize (statement);
			break;
		}
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		name = (char*) sqlite3_column_text (statement, 0);
		chan = sqlite3_column_int (statement, 1);
		prio = sqlite3_column_double (statement, 2);
		if (name != NULL && *name != '\0' && chan >= 0 && prio > 0.0f)
			liser_object_animate (self, name, chan, prio, 1);
	}

	return 1;
}

static int
private_write_animations (LIEngObject* self)
{
	const char* query;
	LIAlgU32dicIter iter;
	LIArcSql* sql;
	LISerAniminfo* info;
	sqlite3_stmt* statement;

	sql = LISER_OBJECT (self)->server->sql;

	/* Save all permanent animations. */
	LIALG_U32DIC_FOREACH (iter, LISER_OBJECT (self)->animations)
	{
		info = iter.value;
		if (!info->permanent)
			continue;
		assert (info->animation != NULL);

		/* Prepare statement. */
		query = "INSERT INTO object_anims (id,name,chan,prio) VALUES (?,?,?,?);";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			return 0;
		}

		/* Bind values. */
		if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK ||
		    sqlite3_bind_text (statement, 2, info->animation->name, -1, SQLITE_TRANSIENT) != SQLITE_OK ||
		    sqlite3_bind_int (statement, 3, info->channel) != SQLITE_OK ||
		    sqlite3_bind_double (statement, 4, info->priority) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Execute statement. */
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
