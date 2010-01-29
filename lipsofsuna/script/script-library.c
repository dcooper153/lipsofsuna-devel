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

#include <lipsofsuna/script.h>

/* @luadoc
 * module "Core.Common.Class"
 * ---
 * -- Inherit classes.
 * -- @name Class
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Checks if an object is an instance of a class.
 * --
 * -- Arguments:
 * -- data: Userdata.
 * -- name: Class name.
 * --
 * -- @param self Class class.
 * -- @param args Arguments.
 * -- @return Boolean.
 * function Class.check(self, args)
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
 * ---
 * -- Inherits a class from another.
 * --
 * -- Arguments:
 * -- 1,name: Class name. (required)
 * -- 2,base: Base class.
 * --
 * -- @param self Class class.
 * -- @param args Arguments.
 * -- @return New class.
 * function Class.new(self, base, name)
 */
static void Class_new (LIScrArgs* args)
{
	const char* name;
	LIScrClass* base;
	LIScrClass* clss;

	name = NULL;
	liscr_args_gets_string (args, "name", &name);

	if (!liscr_args_gets_class (args, "base", NULL, &base) &&
	    !liscr_args_geti_class (args, 1, NULL, &base))
	{
		base = liscr_script_find_class (args->script, "Data");
		if (base == NULL)
			return;
	}
	if (liscr_args_gets_string (args, "name", &name) ||
	    liscr_args_geti_string (args, 0, &name))
	{
		/* Check for duplicates. */
		clss = liscr_script_find_class (args->script, name);
		if (clss != NULL)
			return;

		/* Create a new class. */
		clss = liscr_class_new_full (args->script, base, name, 0);
		if (clss == NULL)
			return;
		if (!liscr_script_insert_class (args->script, clss))
		{
			liscr_class_free (clss);
			return;
		}

		/* Return class. */
		liscr_args_seti_class (args, clss);
	}
}

/* @luadoc
 * ---
 * -- Name of the class.
 * --
 * -- @name Class.name
 * -- @class table
 */
static void Class_getter_name (LIScrArgs* args)
{
	liscr_args_seti_string (args, liscr_class_get_name (args->self));
}

/*****************************************************************************/

/* @luadoc
 * module "Core.Common.Data"
 * ---
 * -- Default base class.
 * -- @name Data
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates an instance of the default base class.
 * --
 * -- @param self Data class.
 * -- @param args Arguments.
 * -- @return New data.
 * function Class.new(self, args)
 */
static void Data_new (LIScrArgs* args)
{
	LIScrData* data;
	LIScrClass* clss;

	/* Get real class. */
	clss = liscr_isanyclass (args->lua, 1);
	if (clss == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new_alloc (args->script, 1, clss->meta);
	if (data == NULL)
		return;

	/* Initialize userdata. */
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
}

/*****************************************************************************/

void
liscr_script_class (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_CLASS, data);
	liscr_class_insert_cfunc (self, "check", Class_check);
	liscr_class_insert_cfunc (self, "new", Class_new);
	liscr_class_insert_mvar (self, "name", Class_getter_name, NULL);
}

void
liscr_script_data (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_DATA, data);
	liscr_class_insert_cfunc (self, "new", Data_new);
}

/** @} */
/** @} */
