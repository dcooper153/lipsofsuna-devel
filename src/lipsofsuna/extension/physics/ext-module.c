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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtPhysics Physics
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_physics_info =
{
	LIMAI_EXTENSION_VERSION, "Physics",
	liext_physics_new,
	liext_physics_free
};

LIExtModule* liext_physics_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Initialize physics. */
	self->physics = liphy_physics_new (program->callbacks);
	if (self->physics == NULL)
	{
		liext_physics_free (self);
		return NULL;
	}

	/* Register component. */
	if (!limai_program_insert_component (program, "physics", self->physics))
	{
		liext_physics_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Physics", liext_script_physics, self);

	return self;
}

void liext_physics_free (
	LIExtModule* self)
{
	/* Unregister component. */
	if (self->physics != NULL)
		limai_program_remove_component (self->program, "physics");

	/* Free physics. */
	if (self->physics != NULL)
		liphy_physics_free (self->physics);

	lisys_free (self);
}

/** @} */
/** @} */
