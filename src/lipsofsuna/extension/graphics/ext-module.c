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
 * \addtogroup LIExtGraphics Graphics
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_graphics_info =
{
	LIMAI_EXTENSION_VERSION, "Graphics",
	liext_graphics_new,
	liext_graphics_free
};

LIExtModule* liext_graphics_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate client. */
	self->client = licli_client_new (program);
	if (self->client == NULL)
	{
		liext_graphics_free (self);
		return NULL;
	}

	return self;
}

void liext_graphics_free (
	LIExtModule* self)
{
	if (self->client != NULL)
		licli_client_free (self->client);
	lisys_free (self);
}

/** @} */
/** @} */
