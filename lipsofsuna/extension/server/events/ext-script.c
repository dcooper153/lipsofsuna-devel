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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtEvents Events
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Events"
 * ---
 * -- Handle events.
 * -- @name Events
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Registers an event handler.
 * --
 * -- @param self Events class.
 * -- @param type Event number.
 * -- @param handler Handler function.
 * function Events.insert(self, type, handler)
 */
static int
Events_insert (lua_State* lua)
{
	int type;
	LIExtModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EVENTS);
	type = luaL_checkinteger (lua, 2);
	luaL_checktype (lua, 3, LUA_TFUNCTION);

	/* Get event table. */
	lua_pushlightuserdata (lua, module);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	if (!lua_istable (lua, -1))
	{
		lua_pop (lua, 1);
		assert (0);
		return 0;
	}

	/* Get handler table. */
	lua_pushnumber (lua, type);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);
	if (!lua_istable (lua, -1))
	{
		lua_pop (lua, 1);
		luaL_argerror (lua, 2, "invalid event type");
	}

	/* Store function. */
	lua_pushvalue (lua, 3);
	lua_pushvalue (lua, 3);
	lua_settable (lua, -3);
	lua_pop (lua, 1);

	return 0;
}

/* @luadoc
 * ---
 * -- Unregisters an event handler.
 * --
 * -- @param self Events class.
 * -- @param type Event number.
 * -- @param handler Handler function.
 * function Events.remove(self, type, handler)
 */
static int
Events_remove (lua_State* lua)
{
	int type;
	LIExtModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EVENTS);
	type = luaL_checkinteger (lua, 2);
	luaL_checktype (lua, 3, LUA_TFUNCTION);

	/* Get event table. */
	lua_pushlightuserdata (lua, module);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	if (!lua_istable (lua, -1))
	{
		lua_pop (lua, 1);
		assert (0);
		return 0;
	}

	/* Get handler table. */
	lua_pushnumber (lua, type);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);
	if (!lua_istable (lua, -1))
	{
		lua_pop (lua, 1);
		luaL_argerror (lua, 2, "invalid event type");
	}

	/* Remove function. */
	lua_pushvalue (lua, 3);
	lua_pushnil (lua);
	lua_settable (lua, -3);
	lua_pop (lua, 1);

	return 0;
}

/*****************************************************************************/

void
liext_script_events (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EVENTS, data);
	liscr_class_insert_enum (self, "CONTROL", LIEXT_EVENT_CONTROL);
	liscr_class_insert_enum (self, "LOGIN", LIEXT_EVENT_LOGIN);
	liscr_class_insert_enum (self, "LOGOUT", LIEXT_EVENT_LOGOUT);
	liscr_class_insert_enum (self, "PACKET", LIEXT_EVENT_PACKET);
	liscr_class_insert_enum (self, "SIMULATE", LIEXT_EVENT_SIMULATE);
	liscr_class_insert_enum (self, "TICK", LIEXT_EVENT_TICK);
	liscr_class_insert_enum (self, "VISIBILITY", LIEXT_EVENT_VISIBILITY);
	liscr_class_insert_func (self, "insert", Events_insert);
	liscr_class_insert_func (self, "remove", Events_remove);
}

/** @} */
/** @} */
