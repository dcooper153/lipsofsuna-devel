/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtBillboard Billboard
 * @{
 */

#include "module.h"

LIMaiExtensionInfo liext_billboard_info =
{
	LIMAI_EXTENSION_VERSION, "Billboard",
	liext_billboard_module_new,
	liext_billboard_module_free
};

LIExtBillboardModule* liext_billboard_module_new (
	LIMaiProgram* program)
{
	LIExtBillboardModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBillboardModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Find the render component. */
	self->render = limai_program_find_component (program, "render");
	if (self->render == NULL)
	{
		liext_billboard_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_BILLBOARD, self);
	liext_script_billboard (program->script);

	return self;
}

void liext_billboard_module_free (
	LIExtBillboardModule* self)
{
	lisys_free (self);
}

/** @} */
/** @} */
