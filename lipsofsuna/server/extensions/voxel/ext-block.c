/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvVoxel Voxel
 * @{
 */

#include <network/lips-network.h>
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-block.h"

liextBlock*
liext_block_new (liextModule* module)
{
	liextBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextBlock));
	if (self == NULL)
		return NULL;
	self->module = module;

	return self;
}

void
liext_block_free (liextBlock* self)
{
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	lisys_free (self);
}

int
liext_block_build (liextBlock*  self,
                   liextModule* module,
                   livoxBlock*  block,
                   limatVector* offset)
{
	int x;
	int y;
	int z;
	limatTransform transform;
	limatVector vector;
	liengModel* model;
	livoxMaterial* material;
	livoxVoxel* voxel;

	/* Free old object. */
	if (self->physics != NULL)
	{
		liphy_object_free (self->physics);
		self->physics = NULL;
	}

	/* Create new shape. */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		/* Type check. */
		voxel = livox_block_get_voxel (block, x, y, z);
		if (!voxel->type)
			continue;

		/* Get model. */
		material = livox_manager_find_material (module->voxels, voxel->type);
		if (material == NULL)
			continue;
		model = lieng_engine_find_model_by_name (self->module->server->engine, material->model);
		if (model == NULL || model->physics == NULL)
			continue;

		/* Add to physics object. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		transform.position = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		livox_voxel_get_quaternion (voxel, &transform.rotation);
		if (self->physics == NULL)
			self->physics = liphy_object_new (self->module->server->engine->physics, 0, NULL, LIPHY_CONTROL_MODE_STATIC);
		if (self->physics != NULL)
			liphy_object_insert_shape (self->physics, model->physics, &transform);
	}

	/* Realize if not empty. */
	if (self->physics != NULL)
	{
		liphy_object_set_collision_group (self->physics, LIPHY_GROUP_TILES);
		liphy_object_set_collision_mask (self->physics, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
		transform = limat_convert_vector_to_transform (*offset);
		liphy_object_set_transform (self->physics, &transform);
		liphy_object_set_realized (self->physics, 1);
	}

	return self->physics != NULL;
}

/** @} */
/** @} */
/** @} */
