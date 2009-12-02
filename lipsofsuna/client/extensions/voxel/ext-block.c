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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#include <network/lips-network.h>
#include "ext-module.h"

static int
private_check_occluder (liextModule* module,
                        livoxVoxel*  voxel);

/*****************************************************************************/

liextBlock*
liext_block_new (licliModule* module)
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
	if (self->group != NULL)
		lirnd_group_free (self->group);
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	lisys_free (self);
}

int
liext_block_build (liextBlock*     self,
                   liextModule*    module,
                   livoxBlock*     block,
                   livoxBlockAddr* addr)
{
#define LINE (LIVOX_TILES_PER_LINE + 2)
#define INDEX(x, y, z) ((x) + (y) * LINE + (z) * LINE * LINE)
	int x;
	int y;
	int z;
	liengModel* engmdl;
	limatTransform transform;
	limatVector vector;
	limatVector offset;
	lirndModel* model;
	livoxMaterial* material;
	livoxVoxel* voxel;
	livoxVoxel voxels[LINE * LINE * LINE];

	/* Calculate offset. */
	vector = limat_vector_init (addr->sector[0], addr->sector[1], addr->sector[2]);
	vector = limat_vector_multiply (vector, LIVOX_SECTOR_WIDTH);
	offset = limat_vector_init (addr->block[0], addr->block[1], addr->block[2]);
	offset = limat_vector_multiply (offset, LIVOX_BLOCK_WIDTH);
	offset = limat_vector_add (offset, vector);

	/* Prepare occlusion test. */
	livox_manager_copy_voxels (module->voxels,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[0] + addr->block[0]) - 1,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[1] + addr->block[1]) - 1,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[2] + addr->block[2]) - 1,
		LINE, LINE, LINE, voxels);

	/* Free old objects. */
	if (self->group != NULL)
	{
		lirnd_group_free (self->group);
		self->group = NULL;
	}
	if (self->physics != NULL)
	{
		liphy_object_free (self->physics);
		self->physics = NULL;
	}

	/* Create new objects. */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		/* Type and occlusion check. */
		voxel = voxels + INDEX (x + 1, y + 1, z + 1);
		if (!voxel->type)
			continue;
		if (private_check_occluder (module, voxels + INDEX (x    , y + 1, z + 1)) &&
		    private_check_occluder (module, voxels + INDEX (x + 2, y + 1, z + 1)) &&
		    private_check_occluder (module, voxels + INDEX (x + 1, y    , z + 1)) &&
		    private_check_occluder (module, voxels + INDEX (x + 1, y + 2, z + 1)) &&
		    private_check_occluder (module, voxels + INDEX (x + 1, y + 1, z    )) &&
		    private_check_occluder (module, voxels + INDEX (x + 1, y + 1, z + 2)))
		    continue;

		/* Get render model. */
		material = livox_manager_find_material (module->voxels, voxel->type);
		if (material == NULL)
			continue;
		engmdl = lieng_engine_find_model_by_name (module->module->engine, material->model);
		if (engmdl == NULL)
			continue;
		model = lirnd_render_find_model (module->module->render, material->model);
		if (model == NULL)
		{
			lirnd_render_load_model (module->module->render, engmdl->name, engmdl->model);
			model = lirnd_render_find_model (module->module->render, material->model);
			if (model == NULL)
				continue;
		}

		/* Add to render group. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		vector = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		transform.position = limat_vector_add (vector, offset);
		livox_voxel_get_quaternion (voxel, &transform.rotation);
		if (self->group == NULL)
			self->group = lirnd_group_new (self->module->scene);
		if (self->group != NULL)
			lirnd_group_insert_model (self->group, model, &transform);

		/* Add to physics object. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		vector = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		transform.position = vector;
		livox_voxel_get_quaternion (voxel, &transform.rotation);
		if (self->physics == NULL)
			self->physics = liphy_object_new (self->module->engine->physics, NULL, LIPHY_CONTROL_MODE_STATIC);
		if (self->physics != NULL)
			liphy_object_insert_shape (self->physics, engmdl->physics, &transform);
	}

	/* Realize if not empty. */
	if (self->group != NULL)
		lirnd_group_set_realized (self->group, 1);
	if (self->physics != NULL)
	{
		liphy_object_set_collision_group (self->physics, LIPHY_GROUP_TILES);
		liphy_object_set_collision_mask (self->physics, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
		transform = limat_convert_vector_to_transform (offset);
		liphy_object_set_transform (self->physics, &transform);
		liphy_object_set_realized (self->physics, 1);
	}

	return 1;
#undef INDEX
#undef LINE
}

/*****************************************************************************/

static int
private_check_occluder (liextModule* module,
                        livoxVoxel*  voxel)
{
	return livox_manager_check_occluder (module->voxels, voxel);
}

/** @} */
/** @} */
/** @} */
