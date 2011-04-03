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
 * \addtogroup LIScrScript Script
 * @{
 */

#include <lipsofsuna/system.h>
#include "script.h"
#include "script-args.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

/**
 * \brief Creates a new script.
 * \return New script or NULL.
 */
LIScrScript* liscr_script_new ()
{
	LIScrScript* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIScrScript));
	if (self == NULL)
		return NULL;
	self->userdata = lialg_strdic_new ();
	if (self->userdata == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate script. */
	self->lua = lua_open ();
	if (self->lua == NULL)
	{
		lisys_error_set (ENOMEM, "cannot allocate script");
		lialg_strdic_free (self->userdata);
		lisys_free (self);
		return NULL;
	}

	/* Load libraries. */
	lua_pushcfunction (self->lua, luaopen_base);
	lua_pushstring (self->lua, "");
	lua_call (self->lua, 1, 0);
	lua_pushcfunction (self->lua, luaopen_package);
	lua_pushstring (self->lua, LUA_LOADLIBNAME);
	lua_call (self->lua, 1, 0);
	lua_pushcfunction (self->lua, luaopen_table);
	lua_pushstring (self->lua, LUA_TABLIBNAME);
	lua_call (self->lua, 1, 0);
	lua_pushcfunction (self->lua, luaopen_string);
	lua_pushstring (self->lua, LUA_STRLIBNAME);
	lua_call (self->lua, 1, 0);
	lua_pushcfunction (self->lua, luaopen_math);
	lua_pushstring (self->lua, LUA_MATHLIBNAME);
	lua_call (self->lua, 1, 0);
	lua_pushcfunction (self->lua, luaopen_debug);
	lua_pushstring (self->lua, LUA_DBLIBNAME);
	lua_call (self->lua, 1, 0);

	/* Create shortcut to self. */
	lua_pushlightuserdata (self->lua, LISCR_SCRIPT_SELF);
	lua_pushlightuserdata (self->lua, self);
	lua_settable (self->lua, LUA_REGISTRYINDEX);

	/* Create internal reference table. */
	lua_pushlightuserdata (self->lua, LISCR_SCRIPT_REFS);
	lua_newtable (self->lua);
	lua_settable (self->lua, LUA_REGISTRYINDEX);

	/* Create pointer->data lookup table. */
	lua_pushlightuserdata (self->lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_newtable (self->lua);
	lua_newtable (self->lua);
	lua_pushstring (self->lua, "v");
	lua_setfield (self->lua, -2, "__mode");
	lua_setmetatable (self->lua, -2);
	lua_settable (self->lua, LUA_REGISTRYINDEX);

	/* Initialize the function table. */
	lua_newtable (self->lua);
	lua_setglobal (self->lua, "Los");

	/* Initialize random numbers. */
	lua_getglobal (self->lua, "math");
	lua_getfield (self->lua, -1, "randomseed");
	lua_remove (self->lua, -2);
	lua_pushnumber (self->lua, lisys_time (NULL));
	if (lua_pcall (self->lua, 1, 0, 0) != 0)
	{
		lisys_error_set (EINVAL, lua_tostring (self->lua, -1));
		lisys_error_report ();
		lua_pop (self->lua, 1);
	}

	return self;
}

/**
 * \brief Frees the script.
 * \param self Script.
 */
void liscr_script_free (LIScrScript* self)
{
	/* Grabage collect everything. */
	lua_close (self->lua);
	self->lua = NULL;

	lialg_strdic_free (self->userdata);
	lisys_free (self);
}

void liscr_script_insert_cfunc (
	LIScrScript*  self,
	const char*   clss,
	const char*   name,
	LIScrArgsFunc func)
{
	lua_getglobal (self->lua, "Los");
	lua_pushstring (self->lua, name);
	lua_pushlightuserdata (self->lua, self);
	lua_pushlightuserdata (self->lua, func);
	lua_pushcclosure (self->lua, liscr_marshal_CLASS, 2);
	lua_rawset (self->lua, -3);
	lua_pop (self->lua, 1);
}

void liscr_script_insert_mfunc (
	LIScrScript*  self,
	const char*   clss,
	const char*   name,
	LIScrArgsFunc func)
{
	lua_getglobal (self->lua, "Los");
	lua_pushstring (self->lua, name);
	lua_pushlightuserdata (self->lua, self);
	lua_pushstring (self->lua, clss);
	lua_pushlightuserdata (self->lua, func);
	lua_pushcclosure (self->lua, liscr_marshal_DATA, 3);
	lua_rawset (self->lua, -3);
	lua_pop (self->lua, 1);
}

/**
 * \brief Attaches a file to the script.
 * \param self Script.
 * \param path Path to the file.
 * \param path_core Core include path.
 * \return Nonzero on success.
 */
int liscr_script_load (
	LIScrScript* self,
	const char*  path,
	const char*  path_core)
{
	int ret;
	char* inc;
	char* inc1;
	char* inc2;

	/* Construct the include path. */
	inc1 = lisys_path_format (LISYS_PATH_PATHNAME, path, LISYS_PATH_SEPARATOR, "?.lua", NULL);
	inc2 = lisys_path_format (path_core, LISYS_PATH_SEPARATOR, "?.lua", NULL);
	if (inc1 == NULL || inc2 == NULL)
	{
		lisys_free (inc1);
		lisys_free (inc2);
		return 0;
	}
	inc = listr_format ("%s;%s", inc1, inc2);
	lisys_free (inc1);
	lisys_free (inc2);
	if (inc == NULL)
		return 0;

	/* Set the include path. */
	lua_getfield (self->lua, LUA_GLOBALSINDEX, "package");
	lua_pushstring (self->lua, inc);
	lua_setfield (self->lua, -2, "path");
	lua_pop (self->lua, 1);
	lisys_free (inc);

	/* Load the file. */
	ret = luaL_loadfile (self->lua, path);
	if (ret)
	{
		lisys_error_set (EIO, "%s", lua_tostring (self->lua, -1));
		lua_pop (self->lua, 1);
		return 0;
	}

	/* Set the error handler. */
	lua_pushvalue (self->lua, 1);
	lua_getglobal (self->lua, "debug");
	lua_getfield (self->lua, 3, "traceback");
	lua_replace (self->lua, 1);
	lua_pop (self->lua, 1);

	/* Parse the file. */
	ret = lua_pcall (self->lua, 0, 0, 1);
	if (ret)
	{
		lisys_error_set (EINVAL, "%s", lua_tostring (self->lua, -1));
		lua_pop (self->lua, 2);
		return 0;
	}
	lua_pop (self->lua, 1);

	return 1;
}

/**
 * \brief Updates the script.
 * \param self Script.
 * \param secs Duration of the tick in seconds.
 */
void liscr_script_update (
	LIScrScript* self,
	float        secs)
{
}

/**
 * \brief Enables or disables garbage collection.
 * \param self Script.
 * \param value Nonzero to enable.
 */
void liscr_script_set_gc (
	LIScrScript* self,
	int          value)
{
	if (value)
		lua_gc (self->lua, LUA_GCRESTART, 0);
	else
		lua_gc (self->lua, LUA_GCSTOP, 0);
}

lua_State* liscr_script_get_lua (
	LIScrScript* self)
{
	return self->lua;
}

void* liscr_script_get_userdata (
	LIScrScript* self,
	const char*  key)
{
	return lialg_strdic_find (self->userdata, key);
}

void liscr_script_set_userdata (
	LIScrScript* self,
	const char*  key,
	void*        value)
{
	lialg_strdic_insert (self->userdata, key, value);
}

/** @} */
/** @} */
