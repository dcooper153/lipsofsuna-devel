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
	self->object = liphy_object_new (module->server->engine->physics, NULL, LIPHY_CONTROL_MODE_STATIC);
	if (self->object == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
liext_block_free (liextBlock* self)
{
	if (self->object != NULL)
		liphy_object_free (self->object);
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
	char name[16];
	limatTransform transform;
	limatVector vector;
	liengModel* model;
	livoxVoxel* voxel;

	/* Clear old shape. */
	liphy_object_set_realized (self->object, 0);
	liphy_object_clear_shape (self->object);
	self->empty = 1;

	/* Create new shape. */
	/* FIXME */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		voxel = livox_block_get_voxel (block, x, y, z);
		if (!voxel->type)
			continue;
		snprintf (name, 16, "tile-%03d", voxel->type);
		model = lieng_engine_find_model_by_name (self->module->server->engine, name);
		if (model == NULL || model->physics == NULL)
			continue;
		vector = limat_vector_init (x + 0.5f, y, z + 0.5f);
		vector = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		liphy_object_insert_shape (self->object, model->physics, &vector);
		self->empty = 0;
	}

	if (!self->empty)
	{
		liphy_object_set_collision_group (self->object, LIPHY_GROUP_TILES);
		liphy_object_set_collision_mask (self->object, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
		transform = limat_convert_vector_to_transform (*offset);
		liphy_object_set_transform (self->object, &transform);
		liphy_object_set_realized (self->object, 1);
	}

	return !self->empty;
}

/** @} */
/** @} */
/** @} */
