/* Lips of Suna
 * Copyright© 2007-2014 Lips of Suna development team.
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
#include "image-loader.h"

static void ImageLoader_new (LIScrArgs* args)
{
	const char* path;
	const char* name;
	LIImgImageLoader* loader;
	LIExtImageModule* module;
	LIScrData* data;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_IMAGE_LOADER);
	if (!liscr_args_geti_string (args, 0, &name))
		return;

	/* Create the path. */
	path = lipth_paths_find_file (module->program->paths, name);
	if (path == NULL)
		return;

	/* Allocate the image loader. */
	loader = liimg_image_loader_new (path);
	if (loader == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, loader, LIEXT_SCRIPT_IMAGE_LOADER, liimg_image_loader_free);
	if (data == NULL)
	{
		liimg_image_loader_free (loader);
		return;
	}
	liscr_args_seti_stack (args);
}

static void ImageLoader_get_image (LIScrArgs* args)
{
	LIImgImage* image;
	LIImgImageLoader* self;
	LIScrData* data;

	/* Get the image. */
	self = args->self;
	image = liimg_image_loader_get_image (self);
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

static void ImageLoader_get_done (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liimg_image_loader_get_done (args->self));
}

/*****************************************************************************/

void liext_script_image_loader (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_IMAGE_LOADER, "image_loader_new", ImageLoader_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_LOADER, "image_loader_get_done", ImageLoader_get_done);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_LOADER, "image_loader_get_image", ImageLoader_get_image);
}

/** @} */
/** @} */
