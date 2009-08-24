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
static int
Generator_load (lua_State* lua)
{
	liscrData* data;
	liwdgWidget* widget;

	data = liscr_checkdata (lua, 1, LIEXT_SCRIPT_GENERATOR);
	widget = data->data;
#warning FIXME: Generator_load not implemented

	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new generator widget.
 * --
 * -- @param self Generator class.
 * -- @param table Optional table of parameters.
 * -- @return New generator widget.
 * function Generator.new(self, table)
 */
static int
Generator_new (lua_State* lua)
{
	liextModule* module;
	liscrScript* script;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_GENERATOR);
	script = liscr_script (lua);

	/* Allocate userdata. */
	if (module->script == NULL)
	{
		module->script = liscr_data_new (script, module->editor, LIEXT_SCRIPT_GENERATOR);
		if (module->script == NULL)
			return 0;
		if (!lua_isnoneornil (lua, 2))
			liscr_copyargs (lua, module->script, 2);
		liwdg_widget_set_userdata (module->editor, module->script);
	}

	liscr_pushdata (lua, module->script);
	return 1;
}

/* @luadoc
 * ---
 * -- Saves the generator rules to disk.
 * --
 * -- @param self Generator class.
 * function Generator.save(self)
 */
static int
Generator_save (lua_State* lua)
{
	liscrData* data;
	liwdgWidget* widget;

	data = liscr_checkdata (lua, 1, LIEXT_SCRIPT_GENERATOR);
	widget = data->data;

	if (!liext_editor_save (LIEXT_EDITOR (widget)))
		lisys_error_report ();

	return 0;
}

/*****************************************************************************/

void
liextGeneratorScript (liscrClass* self,
                      void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_GENERATOR, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "load", Generator_load);
	liscr_class_insert_func (self, "new", Generator_new);
	liscr_class_insert_func (self, "save", Generator_save);
}

/** @} */
/** @} */
/** @} */
