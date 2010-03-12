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
 * \addtogroup liextcliWidgets Widgets
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Widgets",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIExtModule* self;

	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->client = limai_program_find_component (program, "client");

	liscr_script_create_class (program->script, "Button", liext_script_button, self);
	liscr_script_create_class (program->script, "Entry", liext_script_entry, self);
	liscr_script_create_class (program->script, "Image", liext_script_image, self);
	liscr_script_create_class (program->script, "Label", liext_script_label, self);
	liscr_script_create_class (program->script, "Menu", liext_script_menu, self);
	liscr_script_create_class (program->script, "Scroll", liext_script_scroll, self);
	liscr_script_create_class (program->script, "Spin", liext_script_spin, self);
	liscr_script_create_class (program->script, "Tree", liext_script_tree, self);
	liscr_script_create_class (program->script, "View", liext_script_view, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	/* FIXME: Remove the class here. */
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
