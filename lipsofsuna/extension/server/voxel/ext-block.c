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
 * \addtogroup LIExtVoxel Voxel
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

LIExtBlock*
liext_block_new (LIExtModule* module)
{
	LIExtBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBlock));
	if (self == NULL)
		return NULL;
	self->module = module;

	return self;
}

void
liext_block_free (LIExtBlock* self)
{
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	lisys_free (self);
}

int
liext_block_build (LIExtBlock*     self,
                   LIVoxBlockAddr* addr)
{
	/* Free old object. */
	if (self->physics != NULL)
	{
		liphy_object_free (self->physics);
		self->physics = NULL;
	}

	/* Build new object. */
	if (!livox_build_block (self->module->voxels, self->module->program->engine, addr, NULL, &self->physics))
		return 0;

	/* Realize if not empty. */
	if (self->physics != NULL)
		liphy_object_set_realized (self->physics, 1);

	return 1;
}

/** @} */
/** @} */
