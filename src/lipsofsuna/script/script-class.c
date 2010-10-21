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

void private_inherit (
	LIScrClass* self,
	LIScrClass* base);

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
 * \param script Script.
 * \param base Base class or NULL for none.
 * \param name Class name.
 * \param global Nonzero if a global variables should be allocated as well.
 * \return New class or NULL.
 */
LIScrClass* liscr_class_new_full (
	LIScrScript* script,
	LIScrClass*  base,
	const char*  name,
	int          global)
{
	char* name1;
	LIAlgStrdic* dict;
	LIScrClass* self;

	/* Allocate data. */
	name1 = listr_dup (name);
	if (name1 == NULL)
		return NULL;
	dict = lialg_strdic_new ();
	if (dict == NULL)
	{
		lisys_free (name1);
		return NULL;
	}

	/* Allocate self. */
	self = lua_newuserdata (script->lua, sizeof (LIScrClass));
	if (self == NULL)
		return NULL;
	memset (self, 0, sizeof (LIScrClass));
	self->script = script;
	self->name = name1;
	self->userdata = dict;

	/* Create metatable. */
	/* The metatable is also used as the environment table. Custom
	   class variables can be stored to it by the user. */
	lua_newtable (script->lua);
	lua_pushvalue (script->lua, -1);
	lua_setfenv (script->lua, -3);
	lua_pushvalue (script->lua, -1);
	lua_setmetatable (script->lua, -3);
	lua_pop (script->lua, 1);

	/* Setup inheritance. */
	if (base != NULL)
		private_inherit (self, base);

	/* Add to registry if a built-in class. */
	/* This stops built-in classes from being garbage collected as well as
	   allows built-in classes to be searched with liscr_script_find_class(). */
	if (name[0] != '_')
	{
		lua_pushvalue (script->lua, -1);
		lua_setfield (script->lua, LUA_REGISTRYINDEX, name);
	}
	if (global)
	{
		lua_pushvalue (script->lua, -1);
		lua_setglobal (script->lua, name);
	}

	/* Add to class lookup. */
	lua_pushlightuserdata (self->script->lua, LISCR_SCRIPT_LOOKUP_CLASS);
	lua_gettable (self->script->lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (self->script->lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (self->script->lua, self);
	lua_pushvalue (self->script->lua, -3);
	lua_settable (self->script->lua, -3);
	lua_pop (self->script->lua, 2);

	/* Setup default meta functions. */
	liscr_class_insert_func (self, "__call", liscr_class_default___call);
	liscr_class_insert_func (self, "__gc", liscr_class_default___gc);
	liscr_class_insert_func (self, "__index", liscr_class_default___index);
	liscr_class_insert_func (self, "__newindex", liscr_class_default___newindex);

	return self;
}

/**
 * \brief Unregisters and frees the class.
 * \param self Class.
 */
void liscr_class_free (
	LIScrClass* self)
{
	int i;

	/* Remove from class lookup. */
	lua_pushlightuserdata (self->script->lua, LISCR_SCRIPT_LOOKUP_CLASS);
	lua_gettable (self->script->lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (self->script->lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (self->script->lua, self);
	lua_pushnil (self->script->lua);
	lua_settable (self->script->lua, -3);
	lua_pop (self->script->lua, 1);

	/* Free accessors. */
	if (self->vars.array != NULL)
	{
		for (i = 0 ; i < self->vars.count ; i++)
			lisys_free (self->vars.array[i].name);
		lisys_free (self->vars.array);
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

	lisys_free (self->name);
}

/**
 * \brief Inherits from another class.
 * \param self Class.
 * \param meta Class meta string.
 * \return Nonzero on success.
 */
int liscr_class_inherit (
	LIScrClass* self,
	const char* meta)
{
	LIScrClass* base;

	base = liscr_script_find_class (self->script, meta);
	if (base == NULL)
		return 0;
	private_inherit (self, base);

	return 1;
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
 * \brief Inserts a member function to the class.
 * \param self Class.
 * \param name Name for the function.
 * \param value Function pointer.
 */
void liscr_class_insert_func (
	LIScrClass*  self,
	const char*  name,
	liscrMarshal value)
{
	liscr_pushclasspriv (self->script->lua, self);
	lua_pushstring (self->script->lua, name);
	lua_pushlightuserdata (self->script->lua, self);
	lua_pushcclosure (self->script->lua, value, 1);
	lua_rawset (self->script->lua, -3);
	lua_pop (self->script->lua, 1);
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
 * \param self Class.
 * \param name Type name.
 * \return Nonzero if implements.
 */
int liscr_class_get_interface (
	const LIScrClass* self,
	const char*       name)
{
	const LIScrClass* ptr;

	for (ptr = self ; ptr != NULL ; ptr = ptr->base)
	{
		if (!strcmp (ptr->name, name))
			return 1;
	}

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
	void* value;

	value = lialg_strdic_find (self->userdata, key);
	if (value != NULL)
		return value;
	if (self->base != NULL)
		return liscr_class_get_userdata (self->base, key);

	return NULL;
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
	if (!liscr_class_get_interface (clss, clss1->name))
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
	LIScrClass* clss;
	LIScrData* self;

	clss = liscr_isanyclass (lua, 1);
	if (clss != NULL)
		liscr_class_free (clss);
	self = liscr_isanydata (lua, 1);
	if (self != NULL)
		liscr_data_free (self);

	return 0;
}

/**
 * \brief Default getter function.
 *
 * \param lua Lua state.
 * \return One.
 */
int liscr_class_default___index (
	lua_State* lua)
{
	LIScrClass* ptr;
	LIScrClass* clss;
	LIScrData* self;
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

	/* Custom getter. */
	for (ptr = clss ; ptr != NULL ; ptr = ptr->base)
	{
		/* Get private table. */
		liscr_pushclasspriv (lua, ptr);

		/* Get getter. */
		lua_pushstring (lua, "getter");
		lua_rawget (lua, -2);
		if (lua_type (lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (lua, 2);
			continue;
		}
		lua_remove (lua, -2);

		/* Call getter. */
		lua_pushvalue (lua, 1);
		lua_pushvalue (lua, 2);
		if (lua_pcall (lua, 2, 1, 0) != 0)
		{
			lisys_error_set (EINVAL, lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
			break;
		}
		return 1;
	}

	return 0;
}

/**
 * \brief Default setter function.
 *
 * \param lua Lua state.
 * \return Zero.
 */
int liscr_class_default___newindex (
	lua_State* lua)
{
	LIScrClass* ptr;
	LIScrClass* clss;
	LIScrData* self;
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

	/* Custom setter. */
	for (ptr = clss ; ptr != NULL ; ptr = ptr->base)
	{
		/* Get private table. */
		liscr_pushclasspriv (lua, ptr);

		/* Get setter. */
		lua_pushstring (lua, "setter");
		lua_rawget (lua, -2);
		if (lua_type (lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (lua, 2);
			continue;
		}
		lua_remove (lua, -2);

		/* Call setter. */
		lua_pushvalue (lua, 1);
		lua_pushvalue (lua, 2);
		lua_pushvalue (lua, 3);
		if (lua_pcall (lua, 3, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, lua_tostring (self->script->lua, -1));
			lisys_error_report ();
			lua_pop (self->script->lua, 1);
		}
		break;
	}

	return 0;
}

/*****************************************************************************/

void private_inherit (
	LIScrClass* self,
	LIScrClass* base)
{
	LIAlgStrdicIter iter;

	/* Set the base class. */
	self->base = base;

	/* Copy userdata from the base class. This allows the methods
	   of the base classes to successfully access their data. */
	LIALG_STRDIC_FOREACH (iter, self->base->userdata)
		liscr_class_set_userdata (self, iter.key, iter.value);
}

static int
private_insert_func (LIScrClass*   self,
                     int           member,
                     const char*   name,
                     LIScrArgsFunc func)
{
	liscr_pushclasspriv (self->script->lua, self);
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

/** @} */
/** @} */
