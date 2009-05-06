/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvscr Script
 * @{
 * \addtogroup lisrvscrServer Server
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "lips-server-script.h"

/* @luadoc
 * module "Core.Server.Server"
 * ---
 * -- Control the global server state.
 * -- @name Server
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Finds an object by ID.
 * --
 * -- @param self Server class.
 * -- @param id Object ID.
 * -- @return Object or nil.
 * function Server.find_object(self, id)
 */
static int
Server_find_object (lua_State* lua)
{
	uint32_t id;
	liengObject* object;
	lisrvServer* server;

	server = liscr_checkclassdata (lua, 1, LISRV_SCRIPT_SERVER);
	id = (uint32_t) luaL_checknumber (lua, 2);

	object = lieng_engine_find_object (server->engine, id);
	if (object != NULL)
		liscr_pushdata (lua, object->script);
	else
		lua_pushnil (lua);
	return 1;
}

/* @luadoc
 * ---
 * -- Request server save.
 * --
 * -- @param self Server class.
 * function Server.save(self)
 */
static int
Server_save (lua_State* lua)
{
	lisrvServer* server;

	server = liscr_checkclassdata (lua, 1, LISRV_SCRIPT_SERVER);

#warning Saving not implemented.
//	lua_pushnumber (lua, lisrv_server_save (server));
	lua_pushboolean (lua, 0);
	return 1;
}

/* @luadoc
 * ---
 * -- Request server shutdown.
 * --
 * -- @param self Server class.
 * function Server.shutdown(self)
 */
static int
Server_shutdown (lua_State* lua)
{
	lisrvServer* server;

	server = liscr_checkclassdata (lua, 1, LISRV_SCRIPT_SERVER);

	lisrv_server_shutdown (server);
	return 0;
}

/* @luadoc
 * ---
 * -- Debug flag.
 * -- @name Server.debug
 * -- @class table
 */
static int
Server_getter_debug (lua_State* lua)
{
	lisrvServer* server;

	server = liscr_checkclassdata (lua, 1, LISRV_SCRIPT_SERVER);
	liscr_checkclass (lua, 1, LISRV_SCRIPT_SERVER);

	lua_pushnumber (lua, server->debug);
	return 1;
}
static int
Server_setter_debug (lua_State* lua)
{
	uint32_t flags;
	lisrvServer* server;

	server = liscr_checkclassdata (lua, 1, LISRV_SCRIPT_SERVER);
	flags = (uint32_t) luaL_checknumber (lua, 2);

	server->debug = flags;
	return 0;
}

/* @luadoc
 * ---
 * -- Number of seconds the server has been running.
 * -- @name Server.time
 * -- @class table
 */
static int
Server_getter_time (lua_State* lua)
{
	lisrvServer* server;

	server = liscr_checkclassdata (lua, 1, LISRV_SCRIPT_SERVER);
	liscr_checkclass (lua, 1, LISRV_SCRIPT_SERVER);

	lua_pushnumber (lua, lisrv_server_get_time (server));
	return 1;
}

/*****************************************************************************/

void
lisrvServerScript (liscrClass* self,
                   void*       data)
{
#warning LISRV_SCRIPT_SERVER is a duplicate of LICOM_SCRIPT_SERVER
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LISRV_SCRIPT_SERVER, data);
	liscr_class_insert_func (self, "find_object", Server_find_object);
	liscr_class_insert_func (self, "save", Server_save);
	liscr_class_insert_func (self, "shutdown", Server_shutdown);
	liscr_class_insert_getter (self, "debug", Server_getter_debug);
	liscr_class_insert_getter (self, "time", Server_getter_time);
	liscr_class_insert_setter (self, "debug", Server_setter_debug);
}

/** @} */
/** @} */
/** @} */
