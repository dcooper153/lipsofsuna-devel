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
 * \addtogroup liscrClass Class
 * @{
 */

#include <assert.h>
#include <stdlib.h>
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"

static int
private_member_compare (const void* a,
                        const void* b);

static int
private_string_compare (const void* a,
                        const void* b);

/*****************************************************************************/

/**
 * \brief Creates a new class.
 *
 * \param script Script.
 * \param name Class name.
 * \return New class or NULL.
 */
liscrClass*
liscr_class_new (liscrScript* script,
                 const char*  name)
{
	liscrClass* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liscrClass));
	if (self == NULL)
		return NULL;
	self->script = script;
	self->name = strdup (name);
	if (self->name == NULL)
	{
		free (self);
		return NULL;
	}

	/* Allocate class name. */
	self->meta = malloc (strlen (name) + 6);
	if (self->meta == NULL)
	{
		free (self->name);
		free (self);
		return NULL;
	}
	strcpy (self->meta, "Lips.");
	strcat (self->meta, name);

	/* Allocate userdata. */
	self->userdata = lialg_strdic_new ();
	if (self->userdata == NULL)
	{
		free (self->name);
		free (self->meta);
		free (self);
		return NULL;
	}

	/* Create metatable. */
	luaL_newmetatable (self->script->lua, self->meta);
	lua_pushvalue (self->script->lua, -1);
	lua_setglobal (self->script->lua, self->name);

	/* Set metatable. */
	lua_pushvalue (self->script->lua, -1);
	lua_setmetatable (self->script->lua, -2);
	lua_pop (self->script->lua, -1);

	/* Default indexers. */
	liscr_class_insert_func (self, "__index", liscr_class_default___index);
	liscr_class_insert_func (self, "__newindex", liscr_class_default___newindex);

	return self;
}

/**
 * \brief Unregisters and frees the class.
 *
 * \param self Class.
 */
void
liscr_class_free (liscrClass* self)
{
	int i;

	/* Free getters. */
	if (self->getters.getters != NULL)
	{
		for (i = 0 ; i < self->getters.count ; i++)
			free (self->getters.getters[i].name);
		free (self->getters.getters);
	}

	/* Free setters. */
	if (self->setters.setters != NULL)
	{
		for (i = 0 ; i < self->setters.count ; i++)
			free (self->setters.setters[i].name);
		free (self->setters.setters);
	}

	/* Free interfaces. */
	if (self->interfaces.array != NULL)
	{
		for (i = 0 ; i < self->interfaces.count ; i++)
			free (self->interfaces.array[i]);
		free (self->interfaces.array);
	}

	/* Free class variable. */
	if (self->script->lua != NULL)
	{
		lua_pushnil (self->script->lua);
		lua_setglobal (self->script->lua, self->name);
	}

	/* Free userdata. */
	if (self->userdata != NULL)
		lialg_strdic_free (self->userdata);

	free (self->meta);
	free (self->name);
	free (self);
}

void*
liscr_class_convert (liscrClass* self,
                     void*       data)
{
	return self->convert (self->script, data);
}

/**
 * \brief Inherits members from another class.
 *
 * \param self Class.
 * \param init Class initializer.
 */
void
liscr_class_inherit (liscrClass*    self,
                     liscrClassInit init)
{
	init (self, NULL);
}

/**
 * \brief Inserts an enumeration value to the class.
 *
 * \param self Class.
 * \param name Name for the value.
 * \param value Integer value.
 */
void
liscr_class_insert_enum (liscrClass* self,
                         const char* name,
                         int         value)
{
	luaL_getmetatable (self->script->lua, self->meta);
	lua_pushstring (self->script->lua, name);
	lua_pushnumber (self->script->lua, value);
	lua_rawset (self->script->lua, -3);
	lua_pop (self->script->lua, 1);
}

/**
 * \brief Inserts a member function to the class.
 *
 * \param self Class.
 * \param name Name for the function.
 * \param value Function pointer.
 */
void
liscr_class_insert_func (liscrClass*    self,
                         const char*    name,
                         liscrClassFunc value)
{
	luaL_getmetatable (self->script->lua, self->meta);
	lua_pushstring (self->script->lua, name);
	lua_pushcfunction (self->script->lua, value);
	lua_rawset (self->script->lua, -3);
	lua_pop (self->script->lua, 1);
}

/**
 * \brief Inserts a read accessor to the class.
 *
 * \param self Class.
 * \param name Name for the accessor.
 * \param value Function pointer.
 * \return Nonzero on success.
 */
int
liscr_class_insert_getter (liscrClass*    self,
                           const char*    name,
                           liscrClassFunc value)
{
	int i;
	liscrClassMemb* tmp;

	/* Overwrite existing. */
	for (i = 0 ; i < self->getters.count ; i++)
	{
		tmp = self->getters.getters + i;
		if (!strcmp (tmp->name, name))
		{
			tmp->call = value;
			return 1;
		}
	}

	/* Create new. */
	tmp = realloc (self->getters.getters, (self->getters.count + 1) * sizeof (liscrClassMemb));
	if (tmp == NULL)
		return 0;
	self->getters.getters = tmp;
	tmp += self->getters.count;
	tmp->call = value;
	tmp->name = strdup (name);
	if (tmp->name == NULL)
		return 0;
	self->getters.count++;
	self->flags |= LISCR_CLASS_FLAG_SORT_GETTERS;

	return 1;
}

/**
 * \brief Register the class as an implementer of an interface.
 *
 * \param self Class.
 * \param name Type name.
 * \return Nonzero on success.
 */
int
liscr_class_insert_interface (liscrClass* self,
                              const char* name)
{
	char** tmp;

	tmp = realloc (self->interfaces.array, (self->interfaces.count + 1) * sizeof (char*));
	if (tmp == NULL)
		return 0;
	self->interfaces.array = tmp;
	tmp += self->interfaces.count;
	*tmp = strdup (name);
	if (*tmp == NULL)
		return 0;
	self->interfaces.count++;
	qsort (self->interfaces.array, self->interfaces.count,
		sizeof (char*), private_string_compare);

	return 1;
}

/**
 * \brief Inserts a write accessor to the class.
 *
 * \param self Class.
 * \param name Name for the accessor.
 * \param value Function pointer.
 * \return Nonzero on success.
 */
int
liscr_class_insert_setter (liscrClass*    self,
                           const char*    name,
                           liscrClassFunc value)
{
	int i;
	liscrClassMemb* tmp;

	/* Overwrite existing. */
	for (i = 0 ; i < self->setters.count ; i++)
	{
		tmp = self->setters.setters + i;
		if (!strcmp (tmp->name, name))
		{
			tmp->call = value;
			return 1;
		}
	}

	/* Create new. */
	tmp = realloc (self->setters.setters, (self->setters.count + 1) * sizeof (liscrClassMemb));
	if (tmp == NULL)
		return 0;
	self->setters.setters = tmp;
	tmp += self->setters.count;
	tmp->call = value;
	tmp->name = strdup (name);
	if (tmp->name == NULL)
		return 0;
	self->setters.count++;
	self->flags |= LISCR_CLASS_FLAG_SORT_SETTERS;

	return 1;
}

/**
 * \brief Sets the converter function for the class.
 *
 * The converter function is used for converting native objects to
 * script objects. This is primarily used for achieving convenenient
 * and extensible interfacing with scripts.
 *
 * \param self Class.
 * \param value Converter function.
 */
void
liscr_class_set_convert (liscrClass* self,
                         liscrData* (*value)(liscrScript*, void*))
{
	self->convert = value;
}

/**
 * \brief Checks if the class implements an interface.
 *
 * \param self Class.
 * \param name Type name.
 * \return Nonzero if implements.
 */
int
liscr_class_get_interface (const liscrClass* self,
                           const char*       name)
{
	char* ret;

	if (!strcmp (self->meta, name))
		return 1;
	ret = bsearch (&name, self->interfaces.array, self->interfaces.count,
		sizeof (char*), private_string_compare);
	if (ret != NULL)
		return 1;

	return 0;
}

/**
 * \brief Gets the name of the class.
 *
 * \param self Class.
 * \return Name string.
 */
const char*
liscr_class_get_name (const liscrClass* self)
{
	return self->name;
}

/**
 * \brief Sets the serialization method of the class.
 *
 * \param self Class.
 * \param value Serialize function.
 */
void
liscr_class_set_serialize (liscrClass*         self,
                           liscrClassSerialize value)
{
	self->serialize = value;
}

/**
 * \brief Gets an user pointer from the class.
 *
 * \param self Class.
 * \param key Key of the data.
 * \return User pointer.
 */
void*
liscr_class_get_userdata (liscrClass* self,
                          const char* key)
{
	return lialg_strdic_find (self->userdata, key);
}

/**
 * \brief Stores a user pointer to the class.
 *
 * \param self Class.
 * \param key Key of the data.
 * \param value Value to assign to the key.
 */
void
liscr_class_set_userdata (liscrClass* self,
                          const char* key,
                          void*       value)
{
	lialg_strdic_insert (self->userdata, key, value);
}

/*****************************************************************************/

/**
 * \brief Default getter function.
 *
 * \param lua Lua state.
 * \return One.
 */
int
liscr_class_default___index (lua_State* lua)
{
	liscrClass* clss;
	liscrClassMemb tmp;
	liscrClassMemb* func;
	liscrData* self;
	liscrScript* script;

	/* Get class data. */
	script = liscr_script (lua);
	clss = liscr_isanyclass (lua, 1);
	if (clss == NULL)
	{
		self = liscr_isanydata (lua, 1);
		if (self == NULL)
		{
			lua_pushnil (lua);
			return 1;
		}
		clss = liscr_data_get_class (self);
	}
	else
		self = NULL;
	luaL_checkany (lua, 2);

	/* Sort on demand. */
	if (clss->flags & LISCR_CLASS_FLAG_SORT_GETTERS)
	{
		clss->flags &= ~LISCR_CLASS_FLAG_SORT_GETTERS;
		qsort (clss->getters.getters, clss->getters.count,
			sizeof (liscrClassMemb), private_member_compare);
	}

	/* Getters. */
	if (lua_isstring (script->lua, 2))
	{
		tmp.name = (char*) lua_tostring (script->lua, 2);
		func = bsearch (&tmp, clss->getters.getters, clss->getters.count,
			sizeof (liscrClassMemb), private_member_compare);
		if (func != NULL)
		{
			func->call (script->lua);
			return 1;
		}
	}

	/* Custom values. */
	if (self != NULL)
	{
		liscr_pushpriv (script->lua, self);
		lua_pushvalue (script->lua, 2);
		lua_gettable (script->lua, -2);
		if (!lua_isnil (script->lua, -1))
		{
			lua_remove (script->lua, -2);
			return 1;
		}
		lua_pop (script->lua, 2);
	}

	/* Class values. */
	luaL_getmetatable (script->lua, clss->meta);
	assert (!lua_isnil (script->lua, -1));
	lua_pushvalue (script->lua, 2);
	lua_rawget (script->lua, -2);
	lua_remove (script->lua, -2);

	return 1;
}

/**
 * \brief Default setter function.
 *
 * \param lua Lua state.
 * \return Zero.
 */
int
liscr_class_default___newindex (lua_State* lua)
{
	liscrClass* clss;
	liscrClassMemb tmp;
	liscrClassMemb* func;
	liscrData* self;
	liscrScript* script;

	/* Get class data. */
	script = liscr_script (lua);
	clss = liscr_isanyclass (lua, 1);
	if (clss == NULL)
	{
		self = liscr_isanydata (lua, 1);
		if (self == NULL)
		{
			lua_pushnil (lua);
			return 1;
		}
		clss = liscr_data_get_class (self);
	}
	else
		self = NULL;
	luaL_checkany (lua, 2);
	luaL_checkany (lua, 3);

	/* Sort on demand. */
	if (clss->flags & LISCR_CLASS_FLAG_SORT_SETTERS)
	{
		clss->flags &= ~LISCR_CLASS_FLAG_SORT_SETTERS;
		qsort (clss->setters.setters, clss->setters.count,
			sizeof (liscrClassMemb), private_member_compare);
	}

	/* Setters. */
	if (lua_isstring (script->lua, 2))
	{
		tmp.name = (char*) lua_tostring (script->lua, 2);
		func = bsearch (&tmp, clss->setters.setters, clss->setters.count,
			sizeof (liscrClassMemb), private_member_compare);
		if (func != NULL)
		{
			func->call (script->lua);
			return 0;
		}
	}

	/* Custom values. */
	if (self != NULL)
	{
		liscr_pushpriv (script->lua, self);
		lua_pushvalue (script->lua, 2);
		lua_pushvalue (script->lua, 3);
		lua_settable (script->lua, -3);
		lua_pop (script->lua, 1);
		return 0;
	}

	/* Protect reserved names. */
#warning FIXME: Is this enough protection?
	if (lua_isstring (script->lua, 2))
	{
		tmp.name = (char*) lua_tostring (script->lua, 2);
		if (!strncmp (tmp.name, "__", 2))
			return 0;
		func = bsearch (&tmp, clss->getters.getters, clss->getters.count,
			sizeof (liscrClassMemb), private_member_compare);
		if (func != NULL)
			return 0;
	}

	/* Custom class values. */
	luaL_getmetatable (script->lua, clss->meta);
	assert (!lua_isnil (script->lua, -1));
	lua_pushvalue (script->lua, 2);
	lua_pushvalue (script->lua, 3);
	lua_rawset (script->lua, -3);
	lua_pop (script->lua, 1);

	return 0;
}

/*****************************************************************************/

static int
private_member_compare (const void* a,
                        const void* b)
{
	const liscrClassMemb* aa = a;
	const liscrClassMemb* bb = b;

	return strcmp (aa->name, bb->name);
}

static int
private_string_compare (const void* a,
                        const void* b)
{
	const char* const* aa = a;
	const char* const* bb = b;

	return strcmp (*aa, *bb);
}

/** @} */
/** @} */
