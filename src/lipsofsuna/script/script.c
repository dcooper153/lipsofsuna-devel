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
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

/**
 * \brief Creates a new script.
 *
 * \return New script or NULL.
 */
LIScrScript*
liscr_script_new ()
{
	LIScrScript* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIScrScript));
	if (self == NULL)
		return NULL;

	/* Allocate script. */
	self->lua = lua_open ();
	if (self->lua == NULL)
	{
		lisys_error_set (ENOMEM, "cannot allocate script");
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

	/* Create pointer->class lookup table. */
	lua_pushlightuserdata (self->lua, LISCR_SCRIPT_LOOKUP_CLASS);
	lua_newtable (self->lua);
	lua_newtable (self->lua);
	lua_pushstring (self->lua, "v");
	lua_setfield (self->lua, -2, "__mode");
	lua_setmetatable (self->lua, -2);
	lua_settable (self->lua, LUA_REGISTRYINDEX);

	/* Create pointer->data lookup table. */
	lua_pushlightuserdata (self->lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_newtable (self->lua);
	lua_newtable (self->lua);
	lua_pushstring (self->lua, "v");
	lua_setfield (self->lua, -2, "__mode");
	lua_setmetatable (self->lua, -2);
	lua_settable (self->lua, LUA_REGISTRYINDEX);

	return self;
}

/**
 * \brief Frees the script.
 *
 * \param self Script.
 */
void
liscr_script_free (LIScrScript* self)
{
	/* Grabage collect everything. */
	lua_close (self->lua);
	self->lua = NULL;

	lisys_free (self);
}

/**
 * \brief Creates a new class.
 *
 * \param self Script.
 * \param name Name for the class.
 * \param init Initialization function.
 * \param data Data passed to the function.
 * \return Class owned by the script or NULL.
 */
LIScrClass*
liscr_script_create_class (LIScrScript*   self,
                           const char*    name,
                           LIScrClassInit init,
                           void*          data)
{
	LIScrClass* clss;

	/* Create class. */
	clss = liscr_class_new (self, name);
	if (clss == NULL)
		return NULL;

	/* Call initializer. */
	init (clss, data);

	return clss;
}

/**
 * \brief Finds a built-in class by name.
 *
 * \param self Script.
 * \param name Class identifier.
 * \return Class or NULL.
 */
LIScrClass*
liscr_script_find_class (LIScrScript* self,
                         const char*  name)
{
	LIScrClass* clss;

	/* Search from the registry by name. */
	lua_getfield (self->lua, LUA_REGISTRYINDEX, name);
	clss = liscr_isclass (self->lua, -1, name);
	lua_pop (self->lua, 1);

	return clss;
}

/**
 * \brief Attaches a file to the script.
 *
 * \param self Script.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int
liscr_script_load (LIScrScript* self,
                   const char*  path)
{
	int ret;
	char* inc;

	/* Set include path. */
	inc = lisys_path_format (LISYS_PATH_PATHNAME, path,
		LISYS_PATH_STRIPLAST, LISYS_PATH_SEPARATOR, "?.lua", NULL);
	if (inc == NULL)
		return 0;
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
 *
 * \param self Script.
 * \param secs Duration of the tick in seconds.
 */
void
liscr_script_update (LIScrScript* self,
                     float        secs)
{
}

/**
 * \brief Enables or disables garbage collection.
 *
 * \param self Script.
 * \param value Nonzero to enable.
 */
void
liscr_script_set_gc (LIScrScript* self,
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

void*
liscr_script_get_userdata (LIScrScript* self)
{
	return self->userpointer;
}

void
liscr_script_set_userdata (LIScrScript* self,
                           void*        data)
{
	self->userpointer = data;
}

/** @} */
/** @} */
