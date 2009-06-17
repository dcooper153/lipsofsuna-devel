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
 * \addtogroup lisrvscrObject Object
 * @{
 */

#include <network/lips-network.h>
#include <script/common/lips-common-script.h>
#include <server/lips-server.h>
#include "lips-server-script.h"

/* @luadoc
 * module "Core.Server.Object"
 * ---
 * -- Create and manipulate server side objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Sets or clears an animation.
 * --
 * -- @param self Object.
 * -- @param name Animation name or nil.
 * -- @param channel Animation channel or nil for any channel.
 * -- @param priority Animation priority.
 * -- @param permanent Boolean indicating whether the animation is permanent or temporary.
 * function Object.animate(self, name, channel, priority, permanent)
 */
static int
Object_animate (lua_State* lua)
{
	int channel = -1;
	int permanent = 0;
	float priority = 1.0f;
	const char* name = NULL;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	if (!lua_isnil (lua, 2))
		name = luaL_checkstring (lua, 2);
	if (!lua_isnoneornil (lua, 3))
	{
		channel = luaL_checkinteger (lua, 3);
		luaL_argcheck (lua, channel >= 0 && channel < 254, 3, "invalid animation channel");
	}
	if (!lua_isnoneornil (lua, 4))
	{
		priority = luaL_checknumber (lua, 4);
		luaL_argcheck (lua, priority >= 0.0f, 4, "invalid animation priority");
	}
	if (!lua_isnoneornil (lua, 5))
		permanent = lua_toboolean (lua, 5);

	lisrv_object_animate (object->data, name, channel, priority, permanent);
	return 0;
}

/* @luadoc
 * ---
 * -- Disconnects any client controlling the object.
 * --
 * -- @param self Object.
 * function Object.disconnect(self)
 */
static int
Object_disconnect (lua_State* lua)
{
	liscrData* self;
	lisrvObject* object;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	object = LISRV_OBJECT (self->data);

	if (object->client != NULL)
	{
		lisrv_client_free (object->client);
		object->client = NULL;
	}
	return 0;
}

/* @luadoc
 * ---
 * -- FIXME
 * --
 * -- @param self Object.
 * -- @param name String.
 * function Object.effect(self, name)
 */
static int
Object_effect (lua_State* lua)
{
	int flags;
	const char* text;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	text = luaL_checkstring (lua, 2);

	if (lua_isnumber (lua, 3))
		flags = lua_tointeger (lua, 3);
	else
		flags = LI_EFFECT_DEFAULT;
	lisrv_object_effect (object->data, text, flags);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new object.
 * --
 * -- @param self Object class.
 * -- @param table Optional table of parameters.
 * -- @return New object.
 * function Object.new(self, table)
 */
static int
Object_new (lua_State* lua)
{
	int realized = 0;
	const char* name;
	liengObject* object;
	liscrScript* script = liscr_script (lua);
	lisrvServer* server = liscr_script_get_userdata (script);

	/* Allocate object. */
	object = lieng_object_new (server->engine, NULL,
		LIPHY_SHAPE_MODE_CONVEX, LIPHY_CONTROL_MODE_RIGID, 0, NULL);
	if (object == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	liscr_pushdata (lua, object->script);

	/* Copy attributes. */
	if (lua_istable (lua, 2))
	{
		lua_pushnil (lua);
		while (lua_next (lua, 2) != 0)
		{
			if (lua_isstring (lua, -2))
			{
				name = lua_tostring (lua, -2);
				if (!strcmp (name, "realized"))
				{
					realized = lua_toboolean (lua, -2);
					lua_pop (lua, 1);
					continue;
				}
			}
			lua_pushvalue (lua, -2);
			lua_pushvalue (lua, -2);
			lua_settable (lua, -5);
			lua_pop (lua, 1);
		}
	}

	/* Realize the object. */
	if (realized && !lieng_object_set_realized (object, 1))
	{
		lua_pop (lua, 1);
		lua_pushnil (lua);
		return 1;
	}

	return 1;
}

/* @luadoc
 * ---
 * -- Purges the object from the database.
 * --
 * -- @param self Object.
 * function Object.purge(self)
 */
static int
Object_purge (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lisrv_object_purge (self->data);
	return 0;
}

/* @luadoc
 * ---
 * -- Sends a network packet to the client controlling the object.
 * --
 * -- @param self Object.
 * -- @param packet Packet.
 * function Object.send(self, packet)
 */
static int
Object_send (lua_State* lua)
{
	liscrData* object;
	liscrData* packet;
	liscrPacket* data;
	lisrvClient* client;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	packet = liscr_checkdata (lua, 2, LICOM_SCRIPT_PACKET);
	client = LISRV_OBJECT (object->data)->client;
	data = packet->data;
	luaL_argcheck (lua, data->writer != NULL, 2, "packet is not writable");

	if (client != NULL)
		lisrv_client_send (client, data->writer, GRAPPLE_RELIABLE);
	return 0;
}

/* @luadoc
 * ---
 * -- FIXME
 * --
 * -- @param self Object.
 * -- @param vector Vector.
 * function Object.resolve_path(self, path)
 */
static int
Object_solve_path (lua_State* lua)
{
	liaiPath* tmp;
	liscrData* path;
	liscrData* object;
	liscrData* vector;
	liscrScript* script = liscr_script (lua);

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	vector = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	/* Solve path. */
	tmp = lisrv_object_solve_path (object->data, vector->data);
	if (tmp == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Create path object. */
	path = liscr_data_new (script, tmp, LICOM_SCRIPT_PATH);
	if (path == NULL)
	{
		liai_path_free (tmp);
		lua_pushnil (lua);
		return 1;
	}

	liscr_pushdata (lua, path);
	liscr_data_unref (path, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Swaps the clients of the objects.
 * --
 * -- Switches the clients of the passed objects so that the client of the first
 * -- object controls the second object and vice versa. This is typically used when
 * -- you need to destroy the object of a player without disconnecting the client.
 * --
 * -- @param self Object.
 * -- @param object Object.
 * function Object.swap_clients(self, packet)
 */
static int
Object_swap_clients (lua_State* lua)
{
	liscrData* object0;
	liscrData* object1;

	object0 = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	object1 = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);

	lisrv_object_swap (object0->data, object1->data);
	return 0;
}

/* @luadoc
 * ---
 * -- FIXME
 * --
 * -- @param self Object.
 * -- @param start Vector.
 * -- @param end Vector.
 * -- @param radius Number.
 * function Object.sweep_sphere(self, start, end, radius)
 */
static int
Object_sweep_sphere (lua_State* lua)
{
	float radius;
	liscrData* object;
	liscrData* start;
	liscrData* end;
	liscrData* tmp;
	liphyCollision result;
	liscrScript* script = liscr_script (lua);

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	start = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	end = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);
	radius = luaL_checknumber (lua, 4);

	if (liphy_object_sweep_sphere (LIENG_OBJECT (object->data)->physics, start->data, end->data, radius, &result))
	{
		lua_newtable (lua);
		lua_pushnumber (lua, result.fraction);
		lua_setfield (lua, -2, "fraction");
		tmp = liscr_vector_new (script, &result.point);
		if (tmp != NULL)
		{
			liscr_pushdata (lua, tmp);
			liscr_data_unref (tmp, NULL);
			lua_setfield (lua, -2, "point");
		}
		tmp = liscr_vector_new (script, &result.normal);
		if (tmp != NULL)
		{
			liscr_pushdata (lua, tmp);
			liscr_data_unref (tmp, NULL);
			lua_setfield (lua, -2, "normal");
		}
		if (result.object != NULL)
		{
			/* The object field holds the userdata of the colliding engine
			   object or NULL if no collision occurred. */
			liscr_pushdata (lua, LIENG_OBJECT (result.object)->script);
			lua_setfield (lua, -2, "object");
		}
	}
	else
		lua_pushnil (lua);

	return 1;
}

/* @luadoc
 * ---
 * -- Client attached flag.
 * -- @name Object.client
 * -- @class table
 */
static int
Object_getter_client (lua_State* lua)
{
	lisrvClient* tmp;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	tmp = LISRV_OBJECT (object->data)->client;
	lua_pushboolean (lua, tmp != NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Name.
 * -- @name Object.name
 * -- @class table
 */
static int
Object_getter_name (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushstring (lua, lisrv_object_get_name (self->data));
	return 1;
}
static int
Object_setter_name (lua_State* lua)
{
	const char* value;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	value = luaL_checkstring (lua, 3);

	lisrv_object_set_name (object->data, value);
	return 0;
}

/*****************************************************************************/

void
lisrvObjectScript (liscrClass* self,
                   void*       data)
{
	liscr_class_inherit (self, licomObjectScript, NULL);
	liscr_class_set_userdata (self, LISRV_SCRIPT_OBJECT, data);
	liscr_class_insert_interface (self, LISRV_SCRIPT_OBJECT);
	liscr_class_insert_func (self, "animate", Object_animate);
	liscr_class_insert_func (self, "disconnect", Object_disconnect);
	liscr_class_insert_func (self, "effect", Object_effect);
	liscr_class_insert_func (self, "new", Object_new);
	liscr_class_insert_func (self, "purge", Object_purge);
	liscr_class_insert_func (self, "send", Object_send);
	liscr_class_insert_func (self, "solve_path", Object_solve_path);
	liscr_class_insert_func (self, "swap_clients", Object_swap_clients);
	liscr_class_insert_func (self, "sweep_sphere", Object_sweep_sphere);
	liscr_class_insert_getter (self, "client", Object_getter_client);
	liscr_class_insert_getter (self, "name", Object_getter_name);
	liscr_class_insert_setter (self, "name", Object_setter_name);
}

/** @} */
/** @} */
/** @} */
