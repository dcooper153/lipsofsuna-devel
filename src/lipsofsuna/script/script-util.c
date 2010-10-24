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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrUtil Util
 * @{
 */

#include "script.h"
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

LIScrClass* liscr_isanyclass (
	lua_State*  lua,
	int         arg)
{
	int ret;
	void* ptr;

	/* Check if a userdata. */
	ptr = lua_touserdata (lua, arg);
	if (ptr == NULL)
		return NULL;

	/* Get the class lookup table. */
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP_CLASS);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (lua, -1) == LUA_TTABLE);

	/* Check if in the lookup table. */
	lua_pushlightuserdata (lua, ptr);
	lua_gettable (lua, -2);
	ret = lua_isuserdata (lua, -1);
	lua_pop (lua, 2);
	if (!ret)
		return NULL;

	return ptr;
}

LIScrData* liscr_isanydata (
	lua_State* lua,
	int        arg)
{
	int ret;
	void* ptr;

	/* Check if userdata. */
	ptr = lua_touserdata (lua, arg);
	if (ptr == NULL)
		return NULL;

	/* Get the data lookup table. */
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (lua, -1) == LUA_TTABLE);

	/* Check if in the lookup table. */
	lua_pushlightuserdata (lua, ptr);
	lua_gettable (lua, -2);
	ret = lua_isuserdata (lua, -1);
	lua_pop (lua, 2);
	if (!ret)
		return NULL;

	return ptr;
}

/**
 * \brief Gets a class from stack.
 *
 * If the type check fails, NULL is returned.
 *
 * Consumes: 0.
 * Returns: 0.
 *
 * \param lua Lua state.
 * \param arg Stack index.
 * \param meta Class type.
 * \return Class owned by Lua or NULL.
 */
LIScrClass* liscr_isclass (
	lua_State*  lua,
	int         arg,
	const char* meta)
{
	LIScrClass* clss;

	clss = liscr_isanyclass (lua, arg);
	if (clss == NULL)
		return NULL;
	if (!liscr_class_get_interface (clss, meta))
		return NULL;

	return clss;
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
LIScrData* liscr_isdata (
	lua_State*  lua,
	int         arg,
	const char* meta)
{
	LIScrData* data;

	data = liscr_isanydata (lua, arg);
	if (data == NULL)
		return NULL;
	if (!liscr_class_get_interface (data->clss, meta))
		return NULL;

	return data;
}

LIScrClass* liscr_checkanyclass (
	lua_State* lua,
	int        arg)
{
	char msg[256];
	LIScrClass* clss;

	clss = liscr_isanyclass (lua, arg);
	if (clss == NULL)
	{
		snprintf (msg, 255, "expected wrapped class");
		luaL_argcheck (lua, 0, arg, msg);
	}

	return clss;
}

LIScrData* liscr_checkanydata (
	lua_State* lua,
	int        arg)
{
	char msg[256];
	LIScrData* object;

	object = liscr_isanydata (lua, arg);
	if (object == NULL)
	{
		snprintf (msg, 255, "expected wrapped userdata");
		luaL_argcheck (lua, 0, arg, msg);
	}

	return object;
}

LIScrClass* liscr_checkclass (
	lua_State*  lua,
	int         arg,
	const char* meta)
{
	char msg[256];
	LIScrClass* clss;

	clss = liscr_isclass (lua, arg, meta);
	if (clss == NULL)
	{
		snprintf (msg, 255, "expected class %s", meta);
		luaL_argcheck (lua, 0, arg, msg);
	}

	return clss;
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
LIScrData* liscr_checkdata (
	lua_State*  lua,
	int         arg,
	const char* meta)
{
	char msg[256];
	LIScrData* object;

	object = liscr_isdata (lua, arg, meta);
	if (object == NULL)
	{
		snprintf (msg, 255, "expected interface %s", meta);
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
int liscr_copyargs (
	lua_State* lua,
	LIScrData* data,
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
		lisys_assert (0);
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
void liscr_pushclass (
	lua_State*  lua,
	LIScrClass* clss)
{
	/* Get the class lookup table. */
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP_CLASS);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (lua, -1) == LUA_TTABLE);

	/* Fetch by pointer. */
	lua_pushlightuserdata (lua, clss);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);
	lisys_assert (lua_type (lua, -1) == LUA_TUSERDATA);
}

/**
 * \brief Pushes the environment table of the clss to stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param clss Class.
 */
void liscr_pushclasspriv (
	lua_State*  lua,
	LIScrClass* clss)
{
	liscr_pushclass (lua, clss);
	lua_getfenv (lua, -1);
	lua_remove (lua, -2);
	lisys_assert (lua_istable (lua, -1));
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
void liscr_pushdata (
	lua_State* lua,
	LIScrData* object)
{
	lua_pushlightuserdata (lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (lua, object);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);
	lisys_assert (lua_isuserdata (lua, -1));
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
void liscr_pushpriv (
	lua_State* lua,
	LIScrData* object)
{
	liscr_pushdata (lua, object);
	lua_getfenv (lua, -1);
	lua_remove (lua, -2);
	lisys_assert (lua_istable (lua, -1));
}

/**
 * \brief Returns the current script.
 *
 * \param lua Lua state.
 * \return Script.
 */
LIScrScript* liscr_script (
	lua_State* lua)
{
	LIScrScript* script;

	lua_pushlightuserdata (lua, LISCR_SCRIPT_SELF);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	script = lua_touserdata (lua, -1);
	lua_pop (lua, 1);

	return script;
}

/**
 * \brief Prints the traceback to the terminal.
 *
 * This function is intended to be used for debugging. It helps with locating
 * crashes and other problems that involve Lua calling the C functions.
 *
 * \param lua Lua state.
 */
void liscr_traceback (
	lua_State* lua)
{
	/* Get debug table. */
	lua_getfield (lua, LUA_GLOBALSINDEX, "debug");
	if (lua_type (lua, -1) != LUA_TTABLE)
	{
		lisys_error_set (EINVAL, "invalid debug table");
		lua_pop (lua, 1);
		return;
	}

	/* Get traceback function. */
	lua_getfield (lua, -1, "traceback");
	lua_remove (lua, -2);
	if (lua_type (lua, -1) != LUA_TFUNCTION)
	{
		lisys_error_set (EINVAL, "invalid traceback function");
		lua_pop (lua, 1);
		return;
	}

	/* Call traceback excluding itself from the trace. */
	lua_pushstring (lua, "");
	lua_pushinteger (lua, 1);
	if (lua_pcall (lua, 2, 1, 0) != 0)
	{
		lisys_error_set (EINVAL, lua_tostring (lua, -1));
		lisys_error_report ();
		lua_pop (lua, 1);
		return;
	}

	/* Print the traceback to the terminal. */
	printf ("\nLua traceback:\n%s\n\n", lua_tostring (lua, -1));
	lua_pop (lua, 1);
}

/** @} */
/** @} */
