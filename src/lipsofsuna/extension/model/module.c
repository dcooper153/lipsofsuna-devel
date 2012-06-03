/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExtModel Model
 * @{
 */

#include "module.h"

LIMaiExtensionInfo liext_model_info =
{
	LIMAI_EXTENSION_VERSION, "Model",
	liext_model_new,
	liext_model_free
};

LIExtModelModule* liext_model_new (
	LIMaiProgram* program)
{
	LIExtModelModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModelModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_MODEL, self);
	liext_script_model (program->script);

	return self;
}

void liext_model_free (
	LIExtModelModule* self)
{
	lisys_free (self);
}

/** @} */
/** @} */
