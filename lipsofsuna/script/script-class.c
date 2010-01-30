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
 * \addtogroup liscr Script
 * @{
 * \addtogroup LIScrClass Class
 * @{
 */

#include <lipsofsuna/system.h>
#include "script-args.h"
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

static LIScrClassMemb*
private_find_var (LIScrClass* self,
                  const char* name);

static int
private_insert_func (LIScrClass*   self,
                     int           member,
                     const char*   name,
                     LIScrArgsFunc func);

static int
private_insert_var (LIScrClass*   self,
                    int           member,
                    const char*   name,
                    LIScrArgsFunc args,
                    LIScrArgsFunc setter);

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
LIScrClass*
liscr_class_new (LIScrScript* script,
                 const char*  name)
{
	return liscr_class_new_full (script, NULL, name, 1);
}

/**
 * \brief Creates a new class.
 *
 * \param script Script.
 * \param base Base class or NULL for none.
 * \param name Class name.
 * \param global Nonzero if a global variables should be allocated as well.
 * \return New class or NULL.
 */
LIScrClass*
liscr_class_new_full (LIScrScript* script,
                      LIScrClass*  base,
                      const char*  name,
                      int          global)
{
	LIScrClass* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIScrClass));
	if (self == NULL)
		return NULL;
	self->script = script;
	self->base = base;

	/* Allocate class name. */
	self->name = listr_dup (name);
	if (self->name == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate metatable name. */
	self->meta = listr_dup (name);
	if (self->meta == NULL)
	{
		lisys_free (self->name);
		lisys_free (self);
		return NULL;
	}

	/* Allocate userdata. */
	self->userdata = lialg_strdic_new ();
	if (self->userdata == NULL)
	{
		lisys_free (self->name);
		lisys_free (self->meta);
		lisys_free (self);
		return NULL;
	}

	/* Create metatable. */
	luaL_newmetatable (self->script->lua, self->meta);
	if (global)
	{
		lua_pushvalue (self->script->lua, -1);
		lua_setglobal (self->script->lua, self->name);
	}

	/* Set own metatable. */
	lua_pushvalue (self->script->lua, -1);
	lua_setmetatable (self->script->lua, -2);
	lua_pop (self->script->lua, -1);

	/* Default meta functions. */
	liscr_class_insert_func (self, "__call", liscr_class_default___call);
	liscr_class_insert_func (self, "__gc", liscr_class_default___gc);
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
liscr_class_free (LIScrClass* self)
{
	int i;

	/* Free accessors. */
	if (self->vars.array != NULL)
	{
		for (i = 0 ; i < self->vars.count ; i++)
			lisys_free (self->vars.array[i].name);
		lisys_free (self->vars.array);
	}

	/* Free interfaces. */
	if (self->interfaces.array != NULL)
	{
		for (i = 0 ; i < self->interfaces.count ; i++)
			lisys_free (self->interfaces.array[i]);
		lisys_free (self->interfaces.array);
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

	lisys_free (self->meta);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Inherits members from another class.
 *
 * \param self Class.
 * \param init Class initializer.
 * \param data Class userdata.
 */
void
liscr_class_inherit (LIScrClass*    self,
                     LIScrClassInit init,
                     void*          data)
{
	init (self, data);
}

/**
 * \brief Inserts a class function to the class.
 *
 * \param self Class.
 * \param name Function name.
 * \param func Function pointer.
 */
int
liscr_class_insert_cfunc (LIScrClass*   self,
                          const char*   name,
                          LIScrArgsFunc func)
{
	return private_insert_func (self, 0, name, func);
}

/**
 * \brief Inserts a class variable accessor to the class.
 *
 * \param self Class.
 * \param name Name for the accessor.
 * \param getter Function pointer or NULL.
 * \param setter Function pointer or NULL.
 * \return Nonzero on success.
 */
int
liscr_class_insert_cvar (LIScrClass*   self,
                         const char*   name,
                         LIScrArgsFunc getter,
                         LIScrArgsFunc setter)
{
	return private_insert_var (self, 0, name, getter, setter);
}

/**
 * \brief Inserts an enumeration value to the class.
 *
 * \param self Class.
 * \param name Name for the value.
 * \param value Integer value.
 */
void
liscr_class_insert_enum (LIScrClass* self,
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
liscr_class_insert_func (LIScrClass*  self,
                         const char*  name,
                         liscrMarshal value)
{
	luaL_getmetatable (self->script->lua, self->meta);
	lua_pushstring (self->script->lua, name);
	lua_pushlightuserdata (self->script->lua, self);
	lua_pushcclosure (self->script->lua, value, 1);
	lua_rawset (self->script->lua, -3);
	lua_pop (self->script->lua, 1);
}

/**
 * \brief Register the class as an implementer of an interface.
 *
 * \param self Class.
 * \param name Type name.
 * \return Nonzero on success.
 */
int
liscr_class_insert_interface (LIScrClass* self,
                              const char* name)
{
	char** tmp;

	tmp = lisys_realloc (self->interfaces.array, (self->interfaces.count + 1) * sizeof (char*));
	if (tmp == NULL)
		return 0;
	self->interfaces.array = tmp;
	tmp += self->interfaces.count;
	*tmp = listr_dup (name);
	if (*tmp == NULL)
		return 0;
	self->interfaces.count++;
	qsort (self->interfaces.array, self->interfaces.count,
		sizeof (char*), private_string_compare);

	return 1;
}

/**
 * \brief Inserts a member function to the class.
 *
 * \param self Class.
 * \param name Function name.
 * \param func Function pointer.
 */
int
liscr_class_insert_mfunc (LIScrClass*   self,
                          const char*   name,
                          LIScrArgsFunc func)
{
	return private_insert_func (self, 1, name, func);
}

/**
 * \brief Inserts a member variable accessor to the class.
 *
 * \param self Class.
 * \param name Name for the accessor.
 * \param getter Function pointer or NULL.
 * \param setter Function pointer or NULL.
 * \return Nonzero on success.
 */
int
liscr_class_insert_mvar (LIScrClass*  self,
                        const char*   name,
                        LIScrArgsFunc getter,
                        LIScrArgsFunc setter)
{
	return private_insert_var (self, 1, name, getter, setter);
}

/**
 * \brief Checks if the class implements an interface.
 *
 * \param self Class.
 * \param name Type name.
 * \return Nonzero if implements.
 */
int
liscr_class_get_interface (const LIScrClass* self,
                           const char*       name)
{
	char* ret;

	/* Check self. */
	if (!strcmp (self->meta, name))
		return 1;

	/* Check interfaces. */
	ret = bsearch (&name, self->interfaces.array, self->interfaces.count,
		sizeof (char*), private_string_compare);
	if (ret != NULL)
		return 1;

	/* Check base. */
	if (self->base != NULL && liscr_class_get_interface (self->base, name))
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
liscr_class_get_name (const LIScrClass* self)
{
	return self->name;
}

/**
 * \brief Gets an user pointer from the class.
 *
 * \param self Class.
 * \param key Key of the data.
 * \return User pointer.
 */
void*
liscr_class_get_userdata (LIScrClass* self,
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
liscr_class_set_userdata (LIScrClass* self,
                          const char* key,
                          void*       value)
{
	lialg_strdic_insert (self->userdata, key, value);
}

/*****************************************************************************/

/**
 * \brief Default call function.
 *
 * \param lua Lua state.
 * \return Zero.
 */
int
liscr_class_default___call (lua_State* lua)
{
	LIScrClass* clss;
	LIScrClass* clss1;

	/* Get class data. */
	clss = liscr_isanyclass (lua, 1);
	if (clss == NULL)
		return 0;

	/* Check for class. */
	clss1 = lua_touserdata (lua, lua_upvalueindex (1));
	if (!liscr_class_get_interface (clss, clss1->meta))
		return 0;

	/* Call new. */
	lua_getfield (lua, 1, "new");
	if (lua_type (lua, -1) != LUA_TFUNCTION)
		return 0;
	lua_insert (lua, 1);
	lua_call (lua, lua_gettop (lua) - 1, 10);

	return lua_gettop (lua);
}

/**
 * \brief Default garbage collection function.
 *
 * \param lua Lua state.
 * \return Zero.
 */
int
liscr_class_default___gc (lua_State* lua)
{
	LIScrData* self;

	self = liscr_checkanydata (lua, 1);
	liscr_data_free (self);

	return 0;
}

/**
 * \brief Default getter function.
 *
 * \param lua Lua state.
 * \return One.
 */
int
liscr_class_default___index (lua_State* lua)
{
	LIScrClass* ptr;
	LIScrClass* clss;
	LIScrClass* clss1;
	LIScrClassMemb* func;
	LIScrData* self;
	LIScrArgs args;
	LIScrScript* script;

	/* Get class data. */
	script = liscr_script (lua);
	clss = liscr_isanyclass (lua, 1);
	if (clss == NULL)
	{
		self = liscr_isanydata (lua, 1);
		if (self == NULL)
			return 0;
		clss = liscr_data_get_class (self);
	}
	else
		self = NULL;
	luaL_checkany (lua, 2);

	/* Check for class. */
	clss1 = lua_touserdata (lua, lua_upvalueindex (1));
	if (!liscr_class_get_interface (clss, clss1->meta))
		return 0;

	/* Getters. */
	if (lua_isstring (lua, 2))
	{
		func = private_find_var (clss, lua_tostring (lua, 2));
		if (func != NULL)
		{
			if (func->getter == NULL)
				return 0;
			if (func->member && self == NULL)
				return 0;
			if (!func->member && self != NULL)
			{
				liscr_pushclass (lua, clss);
				lua_replace (lua, 1);
				self = NULL;
			}
			liscr_args_init_getter (&args, lua, clss, self);
			func->getter (&args);
			if (!args.output_table)
				return args.ret;
			else
				return 1;
		}
	}

	/* Custom instance variables. */
	if (self != NULL)
	{
		liscr_pushpriv (lua, self);
		lua_pushvalue (lua, 2);
		lua_gettable (lua, -2);
		if (!lua_isnil (lua, -1))
		{
			lua_remove (lua, -2);
			return 1;
		}
		lua_pop (lua, 2);
	}

	/* Custom class variables. */
	for (ptr = clss ; ptr != NULL ; ptr = ptr->base)
	{
		luaL_getmetatable (lua, ptr->meta);
		assert (!lua_isnil (lua, -1));
		lua_pushvalue (lua, 2);
		lua_rawget (lua, -2);
		lua_remove (lua, -2);
		if (!lua_isnil (lua, -1))
			return 1;
		lua_pop (lua, 1);
	}

	/* None found. */
	return 0;
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
	LIScrClass* clss;
	LIScrClass* clss1;
	LIScrClassMemb* func;
	LIScrData* self;
	LIScrArgs args;
	LIScrScript* script;

	/* Get class data. */
	script = liscr_script (lua);
	clss = liscr_isanyclass (lua, 1);
	if (clss == NULL)
	{
		self = liscr_isanydata (lua, 1);
		if (self == NULL)
			return 0;
		clss = liscr_data_get_class (self);
	}
	else
		self = NULL;
	luaL_checkany (lua, 2);
	luaL_checkany (lua, 3);

	/* Check for class. */
	clss1 = lua_touserdata (lua, lua_upvalueindex (1));
	if (!liscr_class_get_interface (clss, clss1->meta))
		return 0;

	/* Setters. */
	if (lua_isstring (lua, 2))
	{
		func = private_find_var (clss, lua_tostring (lua, 2));
		if (func != NULL)
		{
			if (func->setter == NULL)
				return 0;
			if (func->member && self == NULL)
				return 0;
			if (!func->member && self != NULL)
			{
				liscr_pushclass (lua, clss);
				lua_replace (lua, 1);
				self = NULL;
			}
			liscr_args_init_setter (&args, lua, clss, self);
			func->setter (&args);
			return 0;
		}
	}

	/* Custom instance variables. */
	if (self != NULL)
	{
		liscr_pushpriv (lua, self);
		lua_pushvalue (lua, 2);
		lua_pushvalue (lua, 3);
		lua_settable (lua, -3);
		lua_pop (lua, 1);
		return 0;
	}

	/* Custom class variables. */
	luaL_getmetatable (lua, clss->meta);
	assert (!lua_isnil (lua, -1));
	lua_pushvalue (lua, 2);
	lua_pushvalue (lua, 3);
	lua_rawset (lua, -3);
	lua_pop (lua, 1);

	return 0;
}

/*****************************************************************************/

static LIScrClassMemb*
private_find_var (LIScrClass* self,
                  const char* name)
{
	LIScrClass* ptr;
	LIScrClassMemb tmp;
	LIScrClassMemb* func;

	tmp.name = (char*) name;
	for (ptr = self ; ptr != NULL ; ptr = ptr->base)
	{
		if (ptr->flags & LISCR_CLASS_FLAG_SORT_VARS)
		{
			ptr->flags &= ~LISCR_CLASS_FLAG_SORT_VARS;
			qsort (ptr->vars.array, ptr->vars.count,
				sizeof (LIScrClassMemb), private_member_compare);
		}
		func = bsearch (&tmp, ptr->vars.array, ptr->vars.count,
			sizeof (LIScrClassMemb), private_member_compare);
		if (func != NULL)
			return func;
	}

	return NULL;
}

static int
private_insert_func (LIScrClass*   self,
                     int           member,
                     const char*   name,
                     LIScrArgsFunc func)
{
	luaL_getmetatable (self->script->lua, self->meta);
	lua_pushstring (self->script->lua, name);
	lua_pushlightuserdata (self->script->lua, self);
	lua_pushlightuserdata (self->script->lua, func);
	if (member)
		lua_pushcclosure (self->script->lua, liscr_marshal_DATA, 2);
	else
		lua_pushcclosure (self->script->lua, liscr_marshal_CLASS, 2);
	lua_rawset (self->script->lua, -3);
	lua_pop (self->script->lua, 1);

	return 1;
}

static int
private_insert_var (LIScrClass*   self,
                    int           member,
                    const char*   name,
                    LIScrArgsFunc getter,
                    LIScrArgsFunc setter)
{
	int i;
	LIScrClassMemb* tmp;

	/* Overwrite existing. */
	for (i = 0 ; i < self->vars.count ; i++)
	{
		tmp = self->vars.array + i;
		if (!strcmp (tmp->name, name))
		{
			tmp->member = member;
			tmp->getter = getter;
			tmp->setter = setter;
			return 1;
		}
	}

	/* Create new. */
	tmp = lisys_realloc (self->vars.array, (self->vars.count + 1) * sizeof (LIScrClassMemb));
	if (tmp == NULL)
		return 0;
	self->vars.array = tmp;
	tmp += self->vars.count;
	tmp->member = member;
	tmp->getter = getter;
	tmp->setter = setter;
	tmp->name = listr_dup (name);
	if (tmp->name == NULL)
		return 0;
	self->vars.count++;
	self->flags |= LISCR_CLASS_FLAG_SORT_VARS;

	return 1;
}

static int
private_member_compare (const void* a,
                        const void* b)
{
	const LIScrClassMemb* aa = a;
	const LIScrClassMemb* bb = b;

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
