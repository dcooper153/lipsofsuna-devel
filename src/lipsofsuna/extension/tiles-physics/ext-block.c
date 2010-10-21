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
 * \addtogroup LIExtTilesPhysics TilesPhysics
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

LIExtBlock* liext_tiles_physics_block_new (
	LIExtModule* module)
{
	LIExtBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBlock));
	if (self == NULL)
		return NULL;
	self->module = module;

	return self;
}

void liext_tiles_physics_block_free (
	LIExtBlock* self)
{
	if (self->object != NULL)
		liphy_object_free (self->object);
	if (self->shape != NULL)
		liphy_shape_free (self->shape);
	lisys_free (self);
}

int liext_tiles_physics_block_build (
	LIExtBlock*     self,
	LIVoxBlockAddr* addr)
{
	/* Free old object and shape. */
	if (self->object != NULL)
	{
		liphy_object_free (self->object);
		self->object = NULL;
	}
	if (self->shape != NULL)
	{
		liphy_shape_free (self->shape);
		self->shape = NULL;
	}

	/* Build new shape. */
	if (!livox_build_block (self->module->voxels, self->module->program->engine,
	     self->module->physics, addr, NULL, &self->shape))
		return 0;

	/* Create an object and realize if not empty. */
	if (self->shape != NULL)
	{
		self->object = liphy_object_new (self->module->physics, 0, self->shape,
			LIPHY_CONTROL_MODE_STATIC);
		if (self->object == NULL)
		{
			liphy_shape_free (self->shape);
			self->shape = NULL;
		}
		liphy_object_set_collision_group (self->object, LIPHY_GROUP_TILES);
		liphy_object_set_collision_mask (self->object, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
		liphy_object_set_realized (self->object, 1);
	}

	return 1;
}

/** @} */
/** @} */
