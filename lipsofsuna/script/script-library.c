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

#include <script/lips-script.h>

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
static void Class_check (liscrArgs* args)
{
	const char* name;
	liscrData* data;

	if (liscr_args_gets_data (args, "data", NULL, &data) &&
	    liscr_args_gets_string (args, "name", &name))
		liscr_args_seti_bool (args, !strcmp (data->clss->name, name));
}

/* @luadoc
 * ---
 * -- Inherits a class from another.
 * --
 * -- Arguments:
 * -- base: Base class.
 * -- name: Class name.
 * --
 * -- @param self Class class.
 * -- @param args Arguments.
 * -- @return New class.
 * function Class.new(self, base, name)
 */
static void Class_new (liscrArgs* args)
{
	const char* name;
	liscrClass* base;
	liscrClass* clss;

	if (liscr_args_gets_class (args, "base", NULL, &base) &&
	    liscr_args_gets_string (args, "name", &name))
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

/*****************************************************************************/

void
licomClassScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LICOM_SCRIPT_CLASS, data);
	liscr_class_insert_cfunc (self, "check", Class_check);
	liscr_class_insert_cfunc (self, "new", Class_new);
}

/** @} */
/** @} */
