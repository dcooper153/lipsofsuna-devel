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
 * \addtogroup liscrScript Script
 * @{
 */

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <system/lips-system.h>
#include "script.h"
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"

/**
 * \brief Creates a new script.
 *
 * \return New script or NULL.
 */
liscrScript*
liscr_script_new ()
{
	liscrScript* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liscrScript));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Allocate class tree. */
	self->classes = lialg_strdic_new ();
	if (self->classes == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self);
		return NULL;
	}

	/* Allocate object lookup. */
	self->objects = lialg_ptrdic_new ();
	if (self->objects == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lialg_strdic_free (self->classes);
		free (self);
		return NULL;
	}

	/* Allocate script. */
	self->lua = lua_open ();
	if (self->lua == NULL)
	{
		lisys_error_set (ENOMEM, "cannot allocate script");
		lialg_ptrdic_free (self->objects);
		lialg_strdic_free (self->classes);
		free (self);
		return NULL;
	}

	/* Load libraries. */
	luaL_openlibs (self->lua);

	/* Create shortcut to self. */
	lua_pushlightuserdata (self->lua, LISCR_SCRIPT_SELF);
	lua_pushlightuserdata (self->lua, self);
	lua_settable (self->lua, LUA_REGISTRYINDEX);

	/* Create pointer->userdata lookup table. */
	lua_newtable (self->lua);
	lua_newtable (self->lua);
	lua_pushstring (self->lua, "v");
	lua_setfield (self->lua, -2, "__mode");
	lua_setmetatable (self->lua, -2);
	self->userdata.lookup = luaL_ref (self->lua, LUA_REGISTRYINDEX);

	/* Crash at errors. */
	lua_atpanic (self->lua, (lua_CFunction) abort);

	return self;
}

/**
 * \brief Frees the script.
 *
 * \param self Script.
 */
void
liscr_script_free (liscrScript* self)
{
	lialgStrdicIter iter;

	/* Free all objects. */
	lua_close (self->lua);
	self->lua = NULL;
	lialg_ptrdic_free (self->objects);

	/* Free classes. */
	LI_FOREACH_STRDIC (iter, self->classes)
		liscr_class_free (iter.value);
	lialg_strdic_free (self->classes);

	free (self);
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
liscrClass*
liscr_script_create_class (liscrScript*   self,
                           const char*    name,
                           liscrClassInit init,
                           void*          data)
{
	liscrClass* clss;

	/* Create class. */
	clss = liscr_class_new (self, name);
	if (clss == NULL)
		return NULL;

	/* Store to list. */
	if (!lialg_strdic_insert (self->classes, clss->meta, clss))
	{
		liscr_class_free (clss);
		return NULL;
	}

	/* Call initializer. */
	init (clss, data);

	return clss;
}

/**
 * \brief Finds a class by name.
 *
 * \param self Script.
 * \param name Class identifier.
 * \return Class or NULL.
 */
liscrClass*
liscr_script_find_class (liscrScript* self,
                         const char*  name)
{
	return lialg_strdic_find (self->classes, name);
}

/**
 * \brief Finds a user data by object it wraps.
 *
 * \param self Script.
 * \param data Pointer to wrapped data.
 * \return User data or NULL.
 */
void*
liscr_script_find_data (liscrScript* self,
                        const void*  data)
{
	return lialg_ptrdic_find (self->objects, (void*) data);
}

/**
 * \brief Attaches a file to the script.
 *
 * \param self Script.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int
liscr_script_load (liscrScript* self,
                   const char*  path)
{
	int ret;
	char* inc;

	/* Set include path. */
	inc = lisys_path_format (LISYS_PATH_PATHNAME, path, LISYS_PATH_SEPARATOR, "?.lua", NULL);
	if (inc == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	lua_getfield (self->lua, LUA_GLOBALSINDEX, "package");
	lua_pushstring (self->lua, inc);
	lua_setfield (self->lua, -2, "path");
	lua_pop (self->lua, 1);
	free (inc);

	/* Load the file. */
	ret = luaL_loadfile (self->lua, path);
	if (ret)
	{
		lisys_error_set (EIO, "%s", lua_tostring (self->lua, -1));
		lua_pop (self->lua, 1);
		return 0;
	}

	/* Parse the file. */
	ret = lua_pcall (self->lua, 0, 0, 0);
	if (ret)
	{
		lisys_error_set (EINVAL, "%s", lua_tostring (self->lua, -1));
		lua_pop (self->lua, 1);
		return 0;
	}

	return 1;
}

/**
 * \brief Inserts a new class to the script.
 *
 * \param self Script.
 * \param clss Class.
 * \return Nonzero on success.
 */
int
liscr_script_insert_class (liscrScript* self,
                           liscrClass*  clss)
{
	if (!lialg_strdic_insert (self->classes, clss->meta, clss))
		return 0;

	return 1;
}

/**
 * \brief Inserts a new userdata to the script.
 *
 * \param self Script.
 * \param data Data.
 */
void
liscr_script_insert_data (liscrScript* self,
                          liscrData*   data)
{
	lialg_ptrdic_insert (self->objects, data->data, data);
	self->userdata.count++;
}

/**
 * \brief Removes a userdata.
 *
 * \param self Script.
 * \param data Data.
 */
void
liscr_script_remove_data (liscrScript* self,
                          liscrData*   data)
{
	lialg_ptrdic_remove (self->objects, data->data);
	self->userdata.count--;
}

/**
 * \brief Updates the script.
 *
 * \param self Script.
 * \param secs Duration of the tick in seconds.
 */
void
liscr_script_update (liscrScript* self,
                     float        secs)
{
}

void*
liscr_script_get_userdata (liscrScript* self)
{
	return self->userpointer;
}

void
liscr_script_set_userdata (liscrScript* self,
                           void*        data)
{
	self->userpointer = data;
}

/** @} */
/** @} */
