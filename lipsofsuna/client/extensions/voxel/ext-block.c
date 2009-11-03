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

liextBlock*
liext_block_new (licliModule* module)
{
	liextBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextBlock));
	if (self == NULL)
		return NULL;
	self->group = lirnd_group_new (module->scene);
	if (self->group == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	lirnd_group_set_realized (self->group, 1);

	return self;
}

void
liext_block_free (liextBlock* self)
{
	if (self->group != NULL)
		lirnd_group_free (self->group);
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
	lirndModel* model;
	livoxVoxel* voxel;

	/* Free old objects. */
	lirnd_group_clear (self->group);

	/* Create new objects. */
	/* FIXME */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		voxel = livox_block_get_voxel (block, x, y, z);
		if (!voxel->type)
			continue;

		snprintf (name, 16, "tile-%03d", voxel->type);
		lieng_engine_find_model_by_name (module->module->engine, name);
		model = lirnd_render_find_model (module->module->render, name);
		if (model == NULL)
			continue;

		vector = limat_vector_init (x + 0.5f, y, z + 0.5f);
		vector = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		vector = limat_vector_add (vector, *offset);
		transform = limat_convert_vector_to_transform (vector);
		lirnd_group_insert_model (self->group, model, &transform);
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
