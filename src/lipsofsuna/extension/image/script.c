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
 * \addtogroup LIExtImage Image
 * @{
 */

#include "module.h"
#include "image.h"

static void Image_new (LIScrArgs* args)
{
	const char* path;
	const char* name;
	LIImgImage* image;
	LIExtImageModule* module;
	LIScrData* data;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_IMAGE);
	if (!liscr_args_geti_string (args, 0, &name))
		return;

	/* Create the path. */
	path = lipth_paths_find_file (module->program->paths, name);
	if (path == NULL)
		return;

	/* Allocate the image. */
	image = liimg_image_new_from_file (path);
	if (image == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, image, LIEXT_SCRIPT_IMAGE, liimg_image_free);
	if (data == NULL)
	{
		liimg_image_free (image);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Image_blit (LIScrArgs* args)
{
	LIImgImage* self;
	LIImgImage* image;
	LIScrData* value;

	self = args->self;
	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &value))
	{
		image = liscr_data_get_data (value);
		liimg_image_blit (self, image);
	}
}

static void Image_copy (LIScrArgs* args)
{
	LIImgImage* image;
	LIImgImage* src;
	LIScrData* data;

	/* Get arguments. */
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	src = liscr_data_get_data (data);

	/* Allocate the image. */
	image = liimg_image_new_from_image (src);
	if (image == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, image, LIEXT_SCRIPT_IMAGE, liimg_image_free);
	if (data == NULL)
	{
		liimg_image_free (image);
		return;
	}
	liscr_args_seti_stack (args);
}

/*****************************************************************************/

void liext_script_image (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_IMAGE, "image_new", Image_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_blit", Image_blit);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_copy", Image_copy);
}

/** @} */
/** @} */
