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
 * \addtogroup LIScrClass Class
 * @{
 */

#include "lipsofsuna/script.h"
#include "script-private.h"

static int private_member_compare (
	const void* a,
	const void* b)
{
	const LIScrClassMemb* aa = a;
	const LIScrClassMemb* bb = b;

	return strcmp (aa->name, bb->name);
}

static LIScrClassMemb* private_find_var (
	LIScrClass* self,
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

/*****************************************************************************/

/* @luadoc
 * module "builtin/class"
 * --- Inherit classes.
 * -- @name Class
 * -- @class table
 */

/* @luadoc
 * --- Checks if an object is an instance of a class.
 * --
 * -- @param clss Class class.
 * -- @param args Arguments.<ul>
 * --   <li>data: Userdata.</li>
 * --   <li>name: Class name.</li></ul>
 * -- @return Boolean.
 * function Class.check(clss, args)
 */
static void Class_check (LIScrArgs* args)
{
	const char* name;
	LIScrData* data;

	if (liscr_args_gets_data (args, "data", NULL, &data) &&
	    liscr_args_gets_string (args, "name", &name))
		liscr_args_seti_bool (args, !strcmp (data->clss->name, name));
}

/* @luadoc
 * --- Creates a new class or an instance.<br/>
 * -- If called with Class as clss, inherits a new class from another. If called
 * -- with any other class as clss, creates an instance of that class.
 * -- @param clss Class class.
 * -- @param args Arguments.<ul>
 * --   <li>1,base: Base class or nil for Class.</li></ul>
 * -- @return New class.
 * function Class.new(clss, args)
 */
static void Class_new (LIScrArgs* args)
{
	char name[16];
	LIAlgRandom rng;
	LIScrClass* base;
	LIScrClass* clss;
	LIScrData* data;

	/* Get real class. */
	clss = liscr_isanyclass (args->lua, 1);
	if (clss == NULL)
		return;

	/* If called with an already inherited class as clss,
	 * create an instance of that class. */
	if (strcmp (clss->name, LISCR_SCRIPT_CLASS))
	{
		data = liscr_data_new_alloc (args->script, 1, clss);
		if (data == NULL)
			return;
		liscr_args_call_setters (args, data);
		liscr_args_seti_data (args, data);
		return;
	}

	/* Find the base class for the new class we are inheriting. */
	if (!liscr_args_geti_class (args, 0, NULL, &base) &&
	    !liscr_args_gets_class (args, "base", NULL, &base))
	{
		base = liscr_script_find_class (args->script, LISCR_SCRIPT_CLASS);
		if (base == NULL)
			return;
	}

	/* Find a free class name. */
	lialg_random_init (&rng, lisys_time (NULL));
	do
	{
		snprintf (name, 16, "__%4X", lialg_random_max (&rng, 0xFFFF));
		clss = liscr_script_find_class (args->script, name);
	}
	while (clss != NULL);

	/* Create a new class. */
	liscr_script_set_gc (args->script, 0);
	clss = liscr_class_new_full (args->script, base, name, 0);
	if (clss == NULL)
	{
		liscr_script_set_gc (args->script, 1);
		return;
	}

	/* Return the class. */
	liscr_args_seti_class (args, clss);
	liscr_script_set_gc (args->script, 1);
}

/* @luadoc
 * --- Called when the value of a field is being queried.
 * -- @param self Class class or instance.
 * -- @param key Key of any type..
 * -- @return Value of any type.
 * function Class.getter(self, key)
 */
static int Class_getter (lua_State* lua)
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
	if (!liscr_class_get_interface (clss, clss1->name))
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
		liscr_pushclasspriv (lua, ptr);
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

/* @luadoc
 * --- Called when the value of a field is being set.
 * -- @param self Class class or instance.
 * -- @param key Key of any type..
 * -- @param value Value of any type.
 * -- @return New data.
 * function Class.setter(self, key, value)
 */
static int Class_setter (lua_State* lua)
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
	if (!liscr_class_get_interface (clss, clss1->name))
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
	liscr_pushclasspriv (lua, clss);
	lua_pushvalue (lua, 2);
	lua_pushvalue (lua, 3);
	lua_rawset (lua, -3);
	lua_pop (lua, 1);

	return 0;
}

/* @luadoc
 * --- Class of the type.
 * --
 * -- @name Class.class
 * -- @class table
 */
static void Class_get_class (LIScrArgs* args)
{
	liscr_args_seti_class (args, args->clss);
}

/*****************************************************************************/

void liscr_script_class (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_CLASS, self->script);
	liscr_class_insert_cfunc (self, "check", Class_check);
	liscr_class_insert_cfunc (self, "new", Class_new);
	liscr_class_insert_func (self, "getter", Class_getter);
	liscr_class_insert_func (self, "setter", Class_setter);
	liscr_class_insert_mfunc (self, "get_class", Class_get_class);
}

/** @} */
/** @} */
