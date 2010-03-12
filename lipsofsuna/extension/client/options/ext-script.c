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
 * \addtogroup liextcliOptions Options
 * @{
 */

#include "ext-module.h"
#include "ext-options.h"

/* @luadoc
 * module "Extension.Client.Options"
 * ---
 * -- Modify client options.
 * -- @name Options
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new options widget.
 * --
 * -- @param self Options class.
 * -- @param args Arguments.
 * -- @return New options widget.
 * function Options.new(self, table)
 */
static void Options_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_OPTIONS);
	self = liext_options_new (module->client);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_OPTIONS, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/*****************************************************************************/

void
liext_script_options (LIScrClass* self,
                      void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_OPTIONS, data);
	liscr_class_inherit (self, licli_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", Options_new);
}

/** @} */
/** @} */
/** @} */
