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

#include "ext-module.h"
#include "ext-dialog.h"

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Generator",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->client = limai_program_find_component (program, "client");

	/* Create dialog. */
	self->editor = liext_editor_new (self->client->widgets, self);
	if (self->editor == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register scripts. */
	liscr_script_create_class (program->script, "Generator", liext_script_generator, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	if (self->script == NULL)
		liwdg_widget_free (self->editor);
	lisys_free (self);
}

int
liext_module_save (LIExtModule* self)
{
	return liext_editor_save (LIEXT_EDITOR (self->editor));
}

/** @} */
/** @} */
/** @} */
