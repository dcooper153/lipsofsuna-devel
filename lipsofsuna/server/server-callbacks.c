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
 * \addtogroup liserCallbacks Callbacks
 * @{
 */

#include <lipsofsuna/network.h>
#include "server-callbacks.h"
#include "server-observer.h"
#include "server-script.h"

static void
private_contact_callback (LIPhyObject*  object,
                          LIPhyContact* contact)
{
	LIScrData* data;
	LIEngObject* engobj = liphy_object_get_userdata (object);
	LIMaiProgram* program = lieng_engine_get_userdata (engobj->engine);
	LISerServer* server = limai_program_find_component (program, "server");
	LIScrScript* script = server->script;

	/* Push callback. */
	liscr_pushdata (script->lua, engobj->script);
	lua_getfield (script->lua, -1, "contact_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return;
	}

	/* Push object. */
	lua_pushvalue (script->lua, -2);
	lua_remove (script->lua, -3);

	/* Push contact. */
	lua_newtable (script->lua);

	/* Convert impulse. */
	lua_pushnumber (script->lua, contact->impulse);
	lua_setfield (script->lua, -2, "impulse");

	/* Convert object. */
	if (contact->object != NULL)
	{
		engobj = liphy_object_get_userdata (contact->object);
		if (engobj != NULL && engobj->script != NULL)
		{
			liscr_pushdata (script->lua, engobj->script);
			lua_setfield (script->lua, -2, "object");
		}
	}

	/* Convert point. */
	data = liscr_vector_new (script, &contact->point);
	if (data != NULL)
	{
		liscr_pushdata (script->lua, data);
		liscr_data_unref (data, NULL);
	}
	else
		lua_pushnil (script->lua);
	lua_setfield (script->lua, -2, "point");

	/* Convert normal. */
	data = liscr_vector_new (script, &contact->normal);
	if (data != NULL)
	{
		liscr_pushdata (script->lua, data);
		liscr_data_unref (data, NULL);
	}
	else
		lua_pushnil (script->lua);
	lua_setfield (script->lua, -2, "normal");

	/* Call function. */
	if (lua_pcall (script->lua, 2, 0, 0) != 0)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Object.contact_cb: %s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}
}

static int
private_object_free (LISerServer* server,
                     LIEngObject* object)
{
	/* Unrealize before server data is freed. */
	lieng_object_set_realized (object, 0);

	return 1;
}

static int
private_object_new (LISerServer* server,
                    LIEngObject* object)
{
	/* Allocate script data. */
	object->script = liscr_data_new (server->script, object, LISCR_SCRIPT_OBJECT, lieng_object_free);
	if (object->script == NULL)
		return 0;

	/* Extend engine object. */
	liphy_object_set_userdata (object->physics, object);
	liphy_object_set_contact_call (object->physics, private_contact_callback);
	liscr_data_unref (object->script, NULL);

	return 1;
}

static int
private_sector_free (LISerServer* server,
                     LIEngSector* sector)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;

	LIALG_U32DIC_FOREACH (iter, sector->objects)
	{
		object = iter.value;
		if (object->flags & LIENG_OBJECT_FLAG_SAVE)
			liser_object_serialize (object, server, 1);
	}

	return 1;
}

static int
private_sector_load (LISerServer* server,
                     LIEngSector* sector)
{
	int id;
	int ret;
	const char* query;
	LIEngObject* object;
	sqlite3_stmt* statement;

	id = sector->sector->index;

	/* Prepare statement. */
	query = "SELECT id FROM objects WHERE sector=?;";
	if (sqlite3_prepare_v2 (server->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (server->sql));
		return 1;
	}
	if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (server->sql));
		sqlite3_finalize (statement);
		return 1;
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
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (server->sql));
			sqlite3_finalize (statement);
			break;
		}
		id = sqlite3_column_int (statement, 0);

		/* If we're reloading a sector unloaded recently, its object might not have
		   been garbage collected yet. In such a case, reuse the old objects. */
		liscr_script_set_gc (server->script, 0);
		object = lieng_engine_find_object (server->engine, id);
		if (object == NULL)
		{
			object = lieng_object_new (server->engine, NULL, LIPHY_CONTROL_MODE_RIGID, id);
			if (object != NULL)
			{
				if (liser_object_serialize (object, server, 0))
					lieng_object_set_realized (object, 1);
			}
		}
		else
			lieng_object_set_realized (object, 1);
		liscr_script_set_gc (server->script, 1);
	}

	return 1;
}

int
liser_server_init_callbacks_client (LISerServer* server)
{
	lical_callbacks_insert (server->callbacks, server->engine, "object-free", 65535, private_object_free, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "object-new", -65535, private_object_new, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "sector-free", -65535, private_sector_free, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "sector-load", -65535, private_sector_load, server, NULL);
	return 1;
}

/** @} */
/** @} */
