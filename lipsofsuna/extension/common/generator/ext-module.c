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
 * \addtogroup LIExtGenerator Generator
 * @{
 */

#include <lipsofsuna/main.h>
#include "ext-module.h"

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
	self->program = program;

	/* Allocate callbacks. */
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Allocate sectors. */
	self->sectors = lialg_sectors_new (self->program->sectors->count, self->program->sectors->width);
	if (self->sectors == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Allocate generator. */
	self->generator = ligen_generator_new (program->paths, self->callbacks, self->sectors);
	if (self->generator == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Generator", liext_script_generator, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	if (self->generator != NULL)
		ligen_generator_free (self->generator);
	if (self->sectors != NULL)
		lialg_sectors_free (self->sectors);
	if (self->callbacks != NULL)
		lical_callbacks_free (self->callbacks);
	lisys_free (self);
}

/** @} */
/** @} */
