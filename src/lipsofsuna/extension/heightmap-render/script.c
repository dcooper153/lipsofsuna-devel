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
 * \addtogroup LIExtHeightmapRender HeightmapRender
 * @{
 */

#include "module.h"
#include "heightmap.h"

static void Heightmap_add_texture_layer (LIScrArgs* args)
{
	float size;
	const char* name;
	const char* diffuse;
	const char* specular;
	const char* normal;
	const char* height;
	const char* blend;

	/* Get arguments. */
	if (!liscr_args_geti_float (args, 0, &size))
		size = 1.0f;
	if (!liscr_args_geti_string (args, 1, &name) ||
	    !liscr_args_geti_string (args, 2, &diffuse) ||
	    !liscr_args_geti_string (args, 3, &specular) ||
	    !liscr_args_geti_string (args, 4, &normal) ||
	    !liscr_args_geti_string (args, 5, &height))
		return;
	if (!liscr_args_geti_string (args, 6, &blend))
		blend = NULL;

	/* Create the layer. */
	if (!liext_heightmap_add_texture_layer (args->self, size, name, diffuse, specular, normal, height, blend))
		lisys_error_report ();
}

static void Heightmap_rebuild (LIScrArgs* args)
{
	liext_heightmap_rebuild (args->self);
}

/*****************************************************************************/

void liext_script_heightmap_render (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_HEIGHTMAP, "heightmap_add_texture_layer", Heightmap_add_texture_layer);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_HEIGHTMAP, "heightmap_rebuild", Heightmap_rebuild);
}

/** @} */
/** @} */
