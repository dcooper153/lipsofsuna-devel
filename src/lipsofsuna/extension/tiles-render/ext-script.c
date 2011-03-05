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
 * \addtogroup LIExtTilesRender TilesRender
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

/* @luadoc
 * module "core/tiles-render"
 * ---
 * -- Control rendering of tiles.
 * -- @name Voxel
 * -- @class table
 */

/* @luadoc
 * --- Intersects a ray with map tiles.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>1,start_point: Ray start point in world space.</li>
 * --   <li>2,end_point: Ray end point in world space.</li></ul>
 * -- @return Position vector in world space, tile index vector.
 * function Voxel.intersect_render_ray(self, args)
 */
static void Voxel_intersect_render_ray (LIScrArgs* args)
{
	int index[3];
	float d;
	LIAlgMemdicIter iter;
	LIExtBlock* block;
	LIExtModule* module;
	LIMatVector ray0;
	LIMatVector ray1;
	LIMatVector result;
	struct
	{
		float dist;
		LIExtBlock* block;
		LIMatVector point;
	} best = { .block = NULL };

	/* Get arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_TILES_RENDER);
	if (!liscr_args_geti_vector (args, 0, &ray0) &&
	    !liscr_args_gets_vector (args, "start_point", &ray0))
		return;
	if (!liscr_args_geti_vector (args, 1, &ray1) &&
	    !liscr_args_gets_vector (args, "end_point", &ray1))
		return;

	/* Calculate the closest intersection point. */
#warning Voxel_intersect_render_ray is unusable slow because it iterates through all blocks.
	LIALG_MEMDIC_FOREACH (iter, module->blocks)
	{
		block = iter.value;
		if (block->model == NULL)
			continue;

		/* Get the intersection point in the model space. */
		if (!liren_model_intersect_ray (block->model, &ray0, &ray1, &result))
			continue;

		/* Update the result if closer than the old one. */
		d = limat_vector_get_length (limat_vector_subtract (result, ray0));
		if (best.block == NULL || d < best.dist)
		{
			best.block = block;
			best.dist = d;
			best.point = result;
		}
	}

	/* Return the hit tile, if any. */
	if (best.block != NULL)
	{
		/* TODO: Blocks should maintain a list of triangles that tells to which tile
		 * each triangle belongs to. Then we could determine the actual tiles instead
		 * of guesstimating. */
		if (livox_manager_find_voxel (module->voxels, LIVOX_FIND_FULL, &best.point, 0.0f, index) == NULL)
			return;
		result = limat_vector_init (index[0], index[1], index[2]);
		liscr_args_seti_vector (args, &best.point);
		liscr_args_seti_vector (args, &result);
	}
}

/*****************************************************************************/

void liext_script_tiles_render (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_TILES_RENDER, data);
	liscr_class_insert_cfunc (self, "intersect_render_ray", Voxel_intersect_render_ray);
}

/** @} */
/** @} */
