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
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-dialog.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Generator"
 * ---
 * -- Generate random maps.
 * -- @name Generator
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Loads the generator rules from disk.
 * --
 * -- @param self Generator class.
 * function Generator.load(self)
 */
static void Generator_load (liscrArgs* args)
{
#warning FIXME: Generator_load not implemented
}

/* @luadoc
 * ---
 * -- Creates a new generator widget.
 * --
 * -- @param self Generator class.
 * -- @param args Arguments.
 * -- @return New generator widget.
 * function Generator.new(self, args)
 */
static void Generator_new (liscrArgs* args)
{
	liextModule* module;

	/* Only supports one widget currently. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (module->script != NULL)
	{
		liscr_args_seti_data (args, module->script);
		return;
	}

	/* Allocate userdata. */
	module->script = liscr_data_new (args->script, module->editor, LIEXT_SCRIPT_GENERATOR, licli_script_widget_free);
	if (module->script == NULL)
		return;
	liwdg_widget_set_userdata (module->editor, module->script);
	liscr_args_call_setters (args, module->script);
	liscr_args_seti_data (args, module->script);
}

/* @luadoc
 * ---
 * -- Saves the generator rules to disk.
 * --
 * -- @param self Generator class.
 * function Generator.save(self)
 */
static void Generator_save (liscrArgs* args)
{
	if (!liext_editor_save (args->self))
		lisys_error_report ();
}

/*****************************************************************************/

void
liextGeneratorScript (liscrClass* self,
                      void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_GENERATOR, data);
	liscr_class_inherit (self, licliWidgetScript, module->client);
	liscr_class_insert_cfunc (self, "load", Generator_load);
	liscr_class_insert_cfunc (self, "new", Generator_new);
	liscr_class_insert_cfunc (self, "save", Generator_save);
}

/** @} */
/** @} */
/** @} */
