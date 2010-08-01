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
	/* Extend engine object. */
	liphy_object_set_userdata (object->physics, object);
	liphy_object_set_contact_call (object->physics, private_contact_callback);

	return 1;
}

int
liser_server_init_callbacks_client (LISerServer* server)
{
	lical_callbacks_insert (server->callbacks, server->engine, "object-free", 65535, private_object_free, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "object-new", -65535, private_object_new, server, NULL);
	return 1;
}

/** @} */
/** @} */
