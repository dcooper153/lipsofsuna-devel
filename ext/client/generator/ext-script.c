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
#include "ext-generator.h"
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
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_GENERATOR);
	if (!liext_generator_load (module->generator, NULL))
		lisys_error_report ();

	return 0;
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
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_GENERATOR);
	if (!liext_generator_save (module->generator))
		lisys_error_report ();

	return 0;
}

/* @luadoc
 * ---
 * -- Visibility of the generator window.
 * -- @name Generator.visible
 * -- @class table
 */
static int
Generator_getter_visible (lua_State* lua)
{
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_GENERATOR);

	lua_pushboolean (lua, liwdg_widget_get_visible (module->dialog));
	return 1;
}
static int
Generator_setter_visible (lua_State* lua)
{
	int value;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_GENERATOR);
	value = lua_toboolean (lua, 3);

	liwdg_widget_set_visible (module->dialog, value);
	return 0;
}

/*****************************************************************************/

void
liextGeneratorScript (liscrClass* self,
                      void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_GENERATOR, data);
	liscr_class_insert_func (self, "load", Generator_load);
	liscr_class_insert_func (self, "save", Generator_save);
	liscr_class_insert_getter (self, "visible", Generator_getter_visible);
	liscr_class_insert_setter (self, "visible", Generator_setter_visible);
}

/** @} */
/** @} */
/** @} */
