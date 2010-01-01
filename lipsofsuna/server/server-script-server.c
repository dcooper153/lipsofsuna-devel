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
 * -- Arguments:
 * -- id: Object ID.
 * --
 * -- @param self Server class.
 * -- @param args Arguments.
 * -- @return Object or nil.
 * function Server.find_object(self, args)
 */
static void Server_find_object (liscrArgs* args)
{
	int id;
	liengObject* object;
	lisrvServer* server;

	if (liscr_args_gets_int (args, "id", &id))
	{
		server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_SERVER);
		object = lieng_engine_find_object (server->engine, id);
		if (object != NULL)
			liscr_args_seti_data (args, object->script);
	}
}

/* @luadoc
 * ---
 * -- Finds all objects inside a sphere.
 * --
 * -- Arguments:
 * -- point: Center point. (required)
 * -- match: Match type. ("all"/"clients")
 * -- radius: Search radius.
 * --
 * -- @param self Server class.
 * -- @param args Arguments.
 * -- @return Array of matching objects.
 * function Server.nearby_object(self, args)
 */
static void Server_nearby_objects (liscrArgs* args)
{
	int onlyclients = 0;
	float radius = 32.0f;
	const char* tmp;
	liengObjectIter iter;
	limatTransform transform;
	limatVector center;
	limatVector diff;
	lisrvServer* server;

	/* Check arguments. */
	server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_SERVER);
	if (!liscr_args_gets_vector (args, "point", &center))
		return;
	liscr_args_gets_float (args, "radius", &radius);
	if (liscr_args_gets_string (args, "match", &tmp))
	{
		if (!strcmp (tmp, "clients"))
			onlyclients = 1;
	}
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);

	/* Find objects. */
	LIENG_FOREACH_OBJECT (iter, server->engine, &center, radius)
	{
		if (onlyclients && LISRV_OBJECT (iter.object)->client == NULL)
			continue;
		lieng_object_get_transform (iter.object, &transform);
		diff = limat_vector_subtract (center, transform.position);
		if (limat_vector_dot (diff, diff) <= radius)
			liscr_args_seti_data (args, iter.object->script);
	}
}

/* @luadoc
 * ---
 * -- Request server save.
 * --
 * -- @param self Server class.
 * function Server.save(self)
 */
static void Server_save (liscrArgs* args)
{
	lisrvServer* server;

	server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_SERVER);
#warning Saving not implemented.
//	lua_pushnumber (lua, lisrv_server_save (server));
}

/* @luadoc
 * ---
 * -- Request server shutdown.
 * --
 * -- @param self Server class.
 * function Server.shutdown(self)
 */
static void Server_shutdown (liscrArgs* args)
{
	lisrvServer* server;

	server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_SERVER);
	lisrv_server_shutdown (server);
}

/* @luadoc
 * ---
 * -- Debug flag.
 * -- @name Server.debug
 * -- @class table
 */
static void Server_getter_debug (liscrArgs* args)
{
	lisrvServer* server;

	server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_SERVER);
	liscr_args_seti_int (args, server->debug);
}
static void Server_setter_debug (liscrArgs* args)
{
	int value;
	lisrvServer* server;

	server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_SERVER);
	if (liscr_args_geti_int (args, 0, &value))
		server->debug = value;
}

/* @luadoc
 * ---
 * -- Number of seconds the server has been running.
 * -- @name Server.time
 * -- @class table
 */
static void Server_getter_time (liscrArgs* args)
{
	lisrvServer* server;

	server = liscr_class_get_userdata (args->clss, LISRV_SCRIPT_SERVER);
	liscr_args_seti_float (args, lisrv_server_get_time (server));
}

/*****************************************************************************/

void
lisrvServerScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LISRV_SCRIPT_SERVER, data);
	liscr_class_insert_cfunc (self, "find_object", Server_find_object);
	liscr_class_insert_cfunc (self, "nearby_objects", Server_nearby_objects);
	liscr_class_insert_cfunc (self, "save", Server_save);
	liscr_class_insert_cfunc (self, "shutdown", Server_shutdown);
	liscr_class_insert_mvar (self, "debug", Server_getter_debug, Server_setter_debug);
	liscr_class_insert_mvar (self, "time", Server_getter_time, NULL);
}

/** @} */
/** @} */
/** @} */
