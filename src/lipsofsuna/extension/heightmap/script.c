/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIExtHeightmap Heightmap
 * @{
 */

#include "module.h"

static void Heightmap_new (LIScrArgs* args)
{
	int i;
	int size;
	float spacing;
	float scaling;
	LIExtHeightmap* heightmap;
	LIExtHeightmapModule* module;
	LIImgImage* image;
	LIMatVector position;
	LIScrData* data;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_HEIGHTMAP);
	if (!liscr_args_geti_vector (args, 0, &position))
		position = limat_vector_init (0.0f, 0.0f, 0.0f);
	if (!liscr_args_geti_int (args, 1, &size))
		size = 33;
	else if (size < 0)
		size = 0;
	if (!liscr_args_geti_float (args, 2, &spacing))
		spacing = 1.0f;
	else if (spacing <= 0.0f)
		spacing = 1.0f;
	if (!liscr_args_geti_float (args, 3, &scaling))
		scaling = 1.0f;
	else if (scaling <= 0.0f)
		scaling = 1.0f;
	if (liscr_args_geti_data (args, 4, LIEXT_SCRIPT_IMAGE, &data))
		image = liscr_data_get_data (data);
	else
		image = NULL;

	/* Ensure that the size is valid. */
	for (i = 32 ; i < 65536 ; i *= 2)
	{
		if (size == i + 1)
			break;
	}
	if (size != i + 1)
	{
		lisys_error_set (EINVAL, "invalid heightmap size");
		lisys_error_report ();
		return;
	}

	/* Allocate the heightmap. */
	heightmap = liext_heightmap_new (module, image, &position, size, spacing, scaling);
	if (heightmap == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, heightmap, LIEXT_SCRIPT_HEIGHTMAP, liext_heightmap_free);
	if (data == NULL)
	{
		liext_heightmap_free (heightmap);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Heightmap_get_height (LIScrArgs* args)
{
	int clamp = 1;
	float height;
	LIMatVector vector;

	if (!liscr_args_geti_vector (args, 0, &vector))
		return;
	liscr_args_geti_bool (args, 1, &clamp);

	if (liext_heightmap_get_height (args->self, vector.x, vector.z, clamp, &height))
		liscr_args_seti_float (args, height);
}

/*****************************************************************************/

void liext_script_heightmap (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_HEIGHTMAP, "heightmap_new", Heightmap_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_HEIGHTMAP, "heightmap_get_height", Heightmap_get_height);
}

/** @} */
/** @} */
