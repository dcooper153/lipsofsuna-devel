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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliReload Reload
 * @{
 */

#include "ext-module.h"
#include "ext-reload.h"

/* @luadoc
 * module "Extension.Client.Reload"
 * --- Reload modified data files without restarting.
 * -- @name Reload
 * -- @class table
 */

/* @luadoc
 * --- Automatic reload flag.
 * --
 * -- False by default.
 * --
 * -- @name Reload.enabled
 * -- @class table
 */
static void Reload_getter_enabled (LIScrArgs* args)
{
	LIExtReload* self;

	self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_RELOAD);
	liscr_args_seti_bool (args, liext_reload_get_enabled (self));
}
static void Reload_setter_enabled (LIScrArgs* args)
{
	int value;
	LIExtReload* self;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		self = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_RELOAD);
		liext_reload_set_enabled (self, value);
	}
}

/*****************************************************************************/

void
liext_script_reload (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_RELOAD, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cvar (self, "enabled", Reload_getter_enabled, Reload_setter_enabled);
}

/** @} */
/** @} */
/** @} */
