/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#include "lipsofsuna/system.h"
#include "lipsofsuna/archive.h"
#include "lipsofsuna/main.h"
#include "script.h"
#include "script-args.h"
#include "script-data.h"
#include "script-library.h"
#include "script-private.h"
#include "script-util.h"

static int private_exec_script (
	LIScrScript* self);

static int private_load_file (
	lua_State*  lua,
	const char* root,
	const char* path);

static int private_lua_require (
	lua_State* lua);

static int private_lua_require_load (
	lua_State*  lua,
	const char* root,
	const char* path);

/*****************************************************************************/

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
#if LUA_VERSION_NUM > 501
	self->lua = luaL_newstate ();
#else
	self->lua = lua_open ();
#endif
	if (self->lua == NULL)
	{
		lisys_error_set (ENOMEM, "cannot allocate script");
		lialg_strdic_free (self->userdata);
		lisys_free (self);
		return NULL;
	}

	/* Load libraries. */
#if LUA_VERSION_NUM > 501
	luaL_requiref (self->lua, "", luaopen_base, 1);
	lua_pop (self->lua, 1);
	luaL_requiref (self->lua, LUA_COLIBNAME, luaopen_coroutine, 1);
	lua_pop (self->lua, 1);
	luaL_requiref (self->lua, LUA_TABLIBNAME, luaopen_table, 1);
	lua_pop (self->lua, 1);
	luaL_requiref (self->lua, LUA_STRLIBNAME, luaopen_string, 1);
	lua_pop (self->lua, 1);
	luaL_requiref (self->lua, LUA_MATHLIBNAME, luaopen_math, 1);
	lua_pop (self->lua, 1);
	luaL_requiref (self->lua, LUA_DBLIBNAME, luaopen_debug, 1);
	lua_pop (self->lua, 1);
#else
	lua_pushcfunction (self->lua, luaopen_base);
	lua_pushstring (self->lua, "");
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
#endif

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

	/* Register the custom require function. */
	lua_pushcclosure (self->lua, private_lua_require, 0);
	lua_setglobal (self->lua, "require");

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
 * \brief Executes a file.
 * \param self Script.
 * \param path Path to the file.
 * \param path_mod Module include path.
 * \param path_core Core include path.
 * \return Nonzero on success.
 */
int liscr_script_load_file (
	LIScrScript* self,
	const char*  path,
	const char*  path_mod,
	const char*  path_core)
{
	int ret;

	/* Load the file. */
	ret = private_load_file (self->lua, path_mod, path);
	if (ret == -1)
	{
		lisys_error_set (EIO, "%s", lua_tostring (self->lua, -1));
		lua_pop (self->lua, 1);
		return 0;
	}
	if (ret != 1)
		return 0;

	/* Execute the file. */
	if (!private_exec_script (self))
		return 0;

	return 1;
}

/**
 * \brief Executes a string.
 * \param self Script.
 * \param code String containing the executed code.
 * \param path_mod Module include path.
 * \param path_core Core include path.
 * \return Nonzero on success.
 */
int liscr_script_load_string (
	LIScrScript* self,
	const char*  code,
	const char*  path_mod,
	const char*  path_core)
{
	int ret;

	/* Load the string. */
	ret = luaL_loadstring (self->lua, code);
	if (ret)
	{
		lisys_error_set (EIO, "%s", lua_tostring (self->lua, -1));
		lua_pop (self->lua, 1);
		return 0;
	}

	/* Execute the string. */
	if (!private_exec_script (self))
		return 0;

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

/*****************************************************************************/

static int private_exec_script (
	LIScrScript* self)
{
	int ret;

	/* Set the error handler. */
	lua_pushvalue (self->lua, 1);
	lua_getglobal (self->lua, "debug");
	lua_getfield (self->lua, 3, "traceback");
	lua_replace (self->lua, 1);
	lua_pop (self->lua, 1);

	/* Execute the script. */
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

static int private_load_file (
	lua_State*  lua,
	const char* root,
	const char* path)
{
	int ret;
	char* tmp;
	char* path_abs;
	char* path_err;
	LIArcReader* reader;

	/* Construct the absolute path. */
	tmp = lisys_path_concat (root, path, NULL);
	if (tmp == NULL)
		return 0;
	path_abs = lisys_string_concat (tmp, ".lua");
	lisys_free (tmp);
	if (path_abs == NULL)
		return 0;

	/* Construct the name used for error reporting. */
	path_err = lisys_string_format ("@%s.lua", path);
	if (path_err == NULL)
	{
		lisys_free (path_abs);
		return 0;
	}

	/* Open the file. */
	reader = liarc_reader_new_from_file (path_abs);
	lisys_free (path_abs);
	if (reader == NULL)
	{
		lisys_free (path_err);
		return 0;
	}

	/* Load the file. */
	ret = luaL_loadbuffer (lua, reader->buffer, reader->length, path_err);
	liarc_reader_free (reader);
	lisys_free (path_err);
	if (ret != 0)
		return -1;

	return 1;
}

static int private_lua_require (
	lua_State* lua)
{
	int i;
	const char* path;
	LIMaiProgram* program;
	LIScrScript* script;

	/* Get the program pointer. */
	script = liscr_script (lua);
	program = liscr_script_get_userdata (script, LISCR_SCRIPT_PROGRAM);

	/* Validate the path. */
	path = luaL_checkstring (lua, 1);
	if (strstr (path, "..") != NULL)
	{
		luaL_error (lua, "invalid module path \"%s\"", path);
		return 0;
	}
	for (i = 0 ; i < strlen (path) ; i++)
	{
		if ((path[i] < 'a' || path[i] > 'z') && (path[i] < '0' || path[i] > '9') && 
		    (path[i] != '/' && path[i] != '.' && path[i] != '-' && path[i] != '_'))
		{
			luaL_error (lua, "invalid module path \"%s\"", path);
			return 0;
		}
	}

	/* Check if already loaded. */
	lua_getfield (lua, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield (lua, -1, path);
	if (lua_toboolean (lua, -1))
		return 1;
	lua_pop (lua, 3);

	/* Load the file. */
	/* This will push the result to the stack upon success. */
	/* TODO: Also support additions in the home directory. */
	if (!private_lua_require_load (lua, program->paths->module_data, path) &&
	    !private_lua_require_load (lua, program->paths->global_data, path))
	{
		luaL_error (lua, "failed to open module \"%s\"", path);
		return 0;
	}

	/* Mark as loaded. */
	if (!lua_isnil (lua, -1))
	{
		lua_getfield (lua, LUA_REGISTRYINDEX, "_LOADED");
		lua_insert (lua, -2);
		lua_setfield (lua, -2, path);
		lua_pop (lua, 1);
	}
	else
	{
		lua_getfield (lua, LUA_REGISTRYINDEX, "_LOADED");
		lua_pushboolean (lua, 1);
		lua_setfield (lua, -2, path);
		lua_pop (lua, 2);
		lua_pushboolean (lua, 1);
	}

	return 1;
}

static int private_lua_require_load (
	lua_State*  lua,
	const char* root,
	const char* path)
{
	int ret;

	/* Load and parse the file. */
	ret = private_load_file (lua, root, path);
	if (ret == -1)
		lua_error (lua);
	if (ret != 1)
		return 0;

	/* Run the function. */
	lua_call (lua, 0, 1);

	return 1;
}

/** @} */
/** @} */
