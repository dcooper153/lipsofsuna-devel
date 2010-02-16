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

#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-module.h"
#include "ext-block.h"

#define LINE (LIVOX_TILES_PER_LINE + 2)

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
                   LIExtModule*    module,
                   LIVoxBlock*     block,
                   LIVoxBlockAddr* addr)
{
	int i;
	int x;
	int y;
	int z;
	int index;
	int occlusion[LINE * LINE * LINE];
	LIMatTransform transform;
	LIMatVector offset;
	LIMatVector vector;
	LIEngModel* model;
	LIVoxMaterial* material;
	LIVoxVoxel* voxel;
	LIVoxVoxel voxels[LINE * LINE * LINE];

	/* Free old object. */
	if (self->physics != NULL)
	{
		liphy_object_free (self->physics);
		self->physics = NULL;
	}

	/* Fetch voxel data. */
	livox_manager_copy_voxels (module->voxels,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[0] + addr->block[0]) - 1,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[1] + addr->block[1]) - 1,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[2] + addr->block[2]) - 1,
		LINE, LINE, LINE, voxels);

	/* Calculate occlusion. */
	i = livox_manager_solve_occlusion (module->voxels, LINE, LINE, LINE, voxels, occlusion);
	if (i == LIVOX_TILES_PER_BLOCK)
		return 0;

	/* Create new shape. */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		/* Type check. */
		index = (x + 1) + (y + 1) * LINE + (z + 1) * LINE * LINE;
		voxel = voxels + index;
		if (!voxel->type)
			continue;

		/* Occlusion check. */
		if (occlusion[index] & LIVOX_OCCLUDE_OCCLUDED)
			continue;

		/* Get model. */
		material = livox_manager_find_material (module->voxels, voxel->type);
		if (material == NULL)
			continue;
		model = lieng_engine_find_model_by_name (self->module->program->engine, material->model);
		if (model == NULL || model->physics == NULL)
			continue;

		/* Add to physics object. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		transform.position = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		livox_voxel_get_quaternion (voxel, &transform.rotation);
		if (self->physics == NULL)
			self->physics = liphy_object_new (self->module->program->engine->physics, 0, NULL, LIPHY_CONTROL_MODE_STATIC);
		if (self->physics != NULL)
			liphy_object_insert_shape (self->physics, model->physics, &transform);
	}

	/* Calculate offset. */
	vector = limat_vector_init (addr->sector[0], addr->sector[1], addr->sector[2]);
	vector = limat_vector_multiply (vector, LIVOX_SECTOR_WIDTH);
	offset = limat_vector_init (addr->block[0], addr->block[1], addr->block[2]);
	offset = limat_vector_multiply (offset, LIVOX_BLOCK_WIDTH);
	offset = limat_vector_add (offset, vector);
	transform = limat_transform_identity ();

	/* Realize if not empty. */
	if (self->physics != NULL)
	{
		liphy_object_set_collision_group (self->physics, LIPHY_GROUP_TILES);
		liphy_object_set_collision_mask (self->physics, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
		transform = limat_convert_vector_to_transform (offset);
		liphy_object_set_transform (self->physics, &transform);
		liphy_object_set_realized (self->physics, 1);
	}

	return self->physics != NULL;
}

/** @} */
/** @} */
