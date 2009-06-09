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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvEvents Events
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Events"
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
	liextModule* module;

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
	liextModule* module;

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
liextEventsScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EVENTS, data);
	liscr_class_insert_enum (self, "ACTION", LIEXT_EVENT_ACTION);
	liscr_class_insert_enum (self, "PACKET", LIEXT_EVENT_PACKET);
	liscr_class_insert_enum (self, "SELECT", LIEXT_EVENT_SELECT);
	liscr_class_insert_enum (self, "TICK", LIEXT_EVENT_TICK);
	liscr_class_insert_func (self, "insert", Events_insert);
	liscr_class_insert_func (self, "remove", Events_remove);
}

/** @} */
/** @} */
/** @} */
