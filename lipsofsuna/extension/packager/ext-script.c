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
 * \addtogroup liext Extension
 * @{
 * \addtogroup LIExtPackager Packager
 * @{
 */

#include "ext-module.h"
#include "ext-packager.h"

/* @luadoc
 * module "Extension.Client.Packager"
 * --- Automatize data package creation.
 * -- @name Packager
 * -- @class table
 */

/* @luadoc
 * --- Cancels any ongoing packaging process.
 * --
 * -- @param clss Packager class.
 * function Packager.cancel(clss)
 */
static void Packager_cancel (LIScrArgs* args)
{
	LIExtPackager* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PACKAGER);
	liext_packager_cancel (self);
}

/* @luadoc
 * --- Creates a data package.
 * --
 * -- @param clss Packager class.
 * -- @param args Arguments.<ul>
 * --   <li>dir: Directory name.</li>
 * --   <li>file: File name.</li></ul>
 * -- @return True on success.
 * function Packager.save(self, clss)
 */
static void Packager_save (LIScrArgs* args)
{
	const char* dir = "lipsofsuna-data";
	const char* name = "lipsofsuna-data.tar.gz";
	LIExtPackager* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PACKAGER);
	liscr_args_gets_string (args, "dir", &dir);
	liscr_args_gets_string (args, "file", &name);
	liscr_args_seti_bool (args, liext_packager_save (self, name, dir));
}

/* @luadoc
 * --- Boolean switch for verbose debug messages.
 * -- @name Packager.verbose
 * -- @class table
 */
static void Packager_getter_verbose (LIScrArgs* args)
{
	LIExtPackager* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PACKAGER);
	liscr_args_seti_bool (args, liext_packager_get_verbose (self));
}
static void Packager_setter_verbose (LIScrArgs* args)
{
	int value;
	LIExtPackager* self;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_PACKAGER);
		liext_packager_set_verbose (self, value);
	}
}

/*****************************************************************************/

void liext_script_packager (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_PACKAGER, data);
	liscr_class_inherit (self, liscr_script_class, NULL);
	liscr_class_insert_cfunc (self, "cancel", Packager_cancel);
	liscr_class_insert_cfunc (self, "save", Packager_save);
	liscr_class_insert_cvar (self, "verbose", Packager_getter_verbose, Packager_setter_verbose);
}

/** @} */
/** @} */
