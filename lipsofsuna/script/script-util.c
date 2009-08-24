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
 * \addtogroup liscr Script
 * @{
 * \addtogroup liscrUtil Util
 * @{
 */

#include <assert.h>
#include "script.h"
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

liscrClass*
liscr_isanyclass (lua_State*  lua,
                  int         arg)
{
	int ret;
	lialgStrdicIter iter;
	liscrClass* clss;
	liscrScript* script = liscr_script (lua);

	LI_FOREACH_STRDIC (iter, script->classes)
	{
		clss = iter.value;
		lua_pushvalue (lua, arg);
		lua_getfield (lua, LUA_REGISTRYINDEX, clss->meta);
		ret = lua_rawequal (lua, -1, -2);
		lua_pop (lua, 2);
		if (ret)
			return clss;
	}

	return NULL;
}

liscrData*
liscr_isanydata (lua_State* lua,
                 int        arg)
{
	int ret;
	lialgStrdicIter iter;
	liscrClass* clss;
	liscrData* object;
	liscrScript* script = liscr_script (lua);

	object = lua_touserdata (lua, arg);
	if (object == NULL)
		return NULL;
	if (!lua_getmetatable (lua, arg))
		return NULL;
	LI_FOREACH_STRDIC (iter, script->classes)
	{
		clss = iter.value;
		lua_getfield (lua, LUA_REGISTRYINDEX, clss->meta);
		ret = lua_rawequal (lua, -1, -2);
		lua_pop (lua, 1);
		if (!ret)
		{
			lua_pop (lua, 1);
			return object;
		}
	}
	lua_pop (lua, 1);

	return NULL;
}

liscrClass*
liscr_isclass (lua_State*  lua,
               int         arg,
               const char* meta)
{
	int ret;
	lialgStrdicIter iter;
	liscrClass* clss;
	liscrScript* script = liscr_script (lua);

	lua_pushvalue (lua, arg);
	lua_getfield (lua, LUA_REGISTRYINDEX, meta);
	ret = lua_rawequal (lua, -1, -2);
	lua_pop (lua, 2);
	if (!ret)
		return NULL;
	LI_FOREACH_STRDIC (iter, script->classes)
	{
		clss = iter.value;
		if (!strcmp (clss->meta, meta))
			return clss;
	}

	return NULL;
}

/**
 * \brief Gets a userdata from stack.
 *
 * If the type check fails, NULL is returned.
 *
 * Consumes: 0.
 * Returns: 0.
 *
 * \param lua Lua state.
 * \param arg Stack index.
 * \param meta Class type.
 * \return Userdata owned by Lua or NULL.
 */
liscrData*
liscr_isdata (lua_State*  lua,
              int         arg,
              const char* meta)
{
	liscrData* data;

	data = liscr_isanydata (lua, arg);
	if (data == NULL)
		return NULL;
	if (!liscr_class_get_interface (data->clss, meta))
		return NULL;

	return data;
}

liscrClass*
liscr_checkanyclass (lua_State* lua,
                     int        arg)
{
	char msg[256];
	liscrClass* clss;

	clss = liscr_isanyclass (lua, arg);
	if (clss == NULL)
	{
		snprintf (msg, 255, "expected wrapped class");
		luaL_argcheck (lua, 0, arg, msg);
	}

	return clss;
}

liscrData*
liscr_checkanydata (lua_State* lua,
                    int        arg)
{
	char msg[256];
	liscrData* object;

	object = liscr_isanydata (lua, arg);
	if (object == NULL)
	{
		snprintf (msg, 255, "expected wrapped userdata");
		luaL_argcheck (lua, 0, arg, msg);
	}
	if (object->invalid)
	{
		snprintf (msg, 255, "invalid %s", object->clss->meta);
		luaL_argcheck (lua, 0, arg, msg);
	}

	return object;
}

liscrClass*
liscr_checkclass (lua_State*  lua,
                  int         arg,
                  const char* meta)
{
	char msg[256];
	liscrClass* clss;

	clss = liscr_isclass (lua, arg, meta);
	if (clss == NULL)
	{
		snprintf (msg, 255, "expected class %s", meta);
		luaL_argcheck (lua, 0, arg, msg);
	}

	return clss;
}

void*
liscr_checkclassdata (lua_State*  lua,
                      int         arg,
                      const char* meta)
{
	void* data;
	liscrClass* clss;

	clss = liscr_checkclass (lua, arg, meta);
	data = liscr_class_get_userdata (clss, meta);
	assert (data != NULL);

	return data;
}

/**
 * \brief Gets a userdata from stack.
 *
 * If the type check fails, a Lua error is raised.
 *
 * Consumes: 0.
 * Returns: 0.
 *
 * \param lua Lua state.
 * \param arg Stack index.
 * \param meta Class type.
 * \return Userdata owned by Lua.
 */
liscrData*
liscr_checkdata (lua_State*  lua,
                 int         arg,
                 const char* meta)
{
	char msg[256];
	liscrData* object;

	object = liscr_isdata (lua, arg, meta);
	if (object == NULL)
	{
		snprintf (msg, 255, "expected interface %s", meta);
		luaL_argcheck (lua, 0, arg, msg);
	}
	if (object->invalid)
	{
		snprintf (msg, 255, "invalid interface %s", meta);
		luaL_argcheck (lua, 0, arg, msg);
	}

	return object;
}

/**
 * \brief Copies fields from a table to a userdata.
 *
 * \param lua Lua state.
 * \param data Destination userdata.
 * \param arg Index of the source table.
 * \return Nonzero on success.
 */
int
liscr_copyargs (lua_State* lua,
                liscrData* data,
                int        arg)
{
	if (lua_type (lua, arg) != LUA_TTABLE)
		return 0;

	/* Get write indexer. */
	liscr_pushdata (lua, data);
	lua_getfield (lua, -1, "__newindex");
	if (lua_type (lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (lua, 2);
		assert (0);
		return 0;
	}

	/* Call it for each table value. */
	lua_pushnil (lua);
	while (lua_next (lua, arg) != 0)
	{
		lua_pushvalue (lua, -3);
		lua_pushvalue (lua, -5);
		lua_pushvalue (lua, -4);
		lua_pushvalue (lua, -4);
		if (lua_pcall (lua, 3, 0, 0))
		{
			lisys_error_set (EINVAL, lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
		}
		lua_pop (lua, 1);
	}
	lua_pop (lua, 3);

	return 1;
}

/**
 * \brief Pushes the class to stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param clss Pointer to script class.
 */
void
liscr_pushclass (lua_State*  lua,
                 liscrClass* clss)
{
	lua_getfield (lua, LUA_REGISTRYINDEX, clss->meta);
	assert (lua_type (lua, -1) == LUA_TTABLE);
}

/**
 * \brief Pushes the userdata to stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param object Pointer to script userdata.
 */
void
liscr_pushdata (lua_State* lua,
                liscrData* object)
{
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	assert (lua_type (lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (lua, object);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);
	assert (lua_isuserdata (lua, -1));
}

/**
 * \brief Pushes the environment table of the userdata to stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param object Pointer to script userdata.
 */
void
liscr_pushpriv (lua_State* lua,
                liscrData* object)
{
	liscr_pushdata (lua, object);
	lua_getfenv (lua, -1);
	lua_remove (lua, -2);
	assert (lua_istable (lua, -1));
}

/**
 * \brief Returns the current script.
 *
 * \param lua Lua state.
 * \return Script.
 */
liscrScript*
liscr_script (lua_State* lua)
{
	liscrScript* script;

	lua_pushlightuserdata (lua, LISCR_SCRIPT_SELF);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	script = lua_touserdata (lua, -1);
	lua_pop (lua, 1);

	return script;
}

/** @} */
/** @} */
