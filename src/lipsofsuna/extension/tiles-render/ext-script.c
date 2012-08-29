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
 * \addtogroup LIExtTilesRender TilesRender
 * @{
 */

#include "ext-module.h"

static void VoxelRender_set_viewer_position (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector value;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL_RENDER);
	if (liscr_args_geti_vector (args, 0, &value))
		liext_tiles_render_set_viewer_position (module, &value);
}

static void VoxelRender_set_viewer_rotation (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatQuaternion value;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_VOXEL_RENDER);
	if (liscr_args_geti_quaternion (args, 0, &value))
		liext_tiles_render_set_viewer_rotation (module, &value);
}

/*****************************************************************************/

void liext_script_voxel_render (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL_RENDER, "voxel_render_set_viewer_position", VoxelRender_set_viewer_position);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_VOXEL_RENDER, "voxel_render_set_viewer_rotation", VoxelRender_set_viewer_rotation);
}

/** @} */
/** @} */
