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

static void Image_new_empty (LIScrArgs* args)
{
	int w;
	int h;
	LIImgImage* image;
	LIScrData* data;

	/* Get arguments. */
	if (!liscr_args_geti_int (args, 0, &w) || w < 0 || w > 65536)
		return;
	if (!liscr_args_geti_int (args, 1, &h) || h < 0 || h > 65536)
		return;

	/* Allocate the image. */
	image = liimg_image_new ();
	if (image == NULL)
		return;
	if (!liimg_image_alloc (image, w, h))
	{
		liimg_image_free (image);
		return;
	}

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
	src = args->self;

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

static void Image_fill (LIScrArgs* args)
{
	int f;
	LIImgColor color = { 0, 0, 0, 255 };
	LIImgImage* image;

	/* Get arguments. */
	image = args->self;
	if (liscr_args_geti_int (args, 0, &f))
		color.r = (uint8_t) LIMAT_CLAMP (f, 0, 255);
	if (liscr_args_geti_int (args, 1, &f))
		color.g = (uint8_t) LIMAT_CLAMP (f, 0, 255);
	if (liscr_args_geti_int (args, 2, &f))
		color.b = (uint8_t) LIMAT_CLAMP (f, 0, 255);
	if (liscr_args_geti_int (args, 3, &f))
		color.a = (uint8_t) LIMAT_CLAMP (f, 0, 255);

	/* Set the color. */
	liimg_image_fill (image, &color);
}

static void Image_get_pixel (LIScrArgs* args)
{
	int x;
	int y;
	LIImgColor color;
	LIImgImage* image;

	/* Get arguments. */
	image = args->self;
	if (!liscr_args_geti_int (args, 0, &x) || x < 0 || x >= image->width)
		return;
	if (!liscr_args_geti_int (args, 1, &y) || y < 0 || y >= image->height)
		return;

	/* Get the color. */
	color = liimg_image_get_pixel (image, x, y);
	liscr_args_seti_int (args, color.r);
	liscr_args_seti_int (args, color.g);
	liscr_args_seti_int (args, color.b);
	liscr_args_seti_int (args, color.a);
}

static void Image_set_pixel (LIScrArgs* args)
{
	int x;
	int y;
	int f;
	LIImgColor color = { 0, 0, 0, 255 };
	LIImgImage* image;

	/* Get arguments. */
	image = args->self;
	if (!liscr_args_geti_int (args, 0, &x) || x < 0 || x >= image->width)
		return;
	if (!liscr_args_geti_int (args, 1, &y) || y < 0 || y >= image->width)
		return;
	if (liscr_args_geti_int (args, 2, &f))
		color.r = (uint8_t) LIMAT_CLAMP (f, 0, 255);
	if (liscr_args_geti_int (args, 3, &f))
		color.g = (uint8_t) LIMAT_CLAMP (f, 0, 255);
	if (liscr_args_geti_int (args, 4, &f))
		color.b = (uint8_t) LIMAT_CLAMP (f, 0, 255);
	if (liscr_args_geti_int (args, 5, &f))
		color.a = (uint8_t) LIMAT_CLAMP (f, 0, 255);

	/* Set the color. */
	liimg_image_set_pixel (image, x, y, &color);
	liscr_args_seti_bool (args, 1);
}

static void Image_get_width (LIScrArgs* args)
{
	LIImgImage* self;

	self = args->self;
	liscr_args_seti_int (args, self->width);
}

static void Image_get_height (LIScrArgs* args)
{
	LIImgImage* self;

	self = args->self;
	liscr_args_seti_int (args, self->height);
}

static void Image_get_size (LIScrArgs* args)
{
	LIImgImage* self;

	self = args->self;
	liscr_args_seti_int (args, self->width);
	liscr_args_seti_int (args, self->height);
}

/*****************************************************************************/

void liext_script_image (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_IMAGE, "image_new", Image_new);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_IMAGE, "image_new_empty", Image_new_empty);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_blit", Image_blit);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_copy", Image_copy);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_fill", Image_fill);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_get_pixel", Image_get_pixel);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_set_pixel", Image_set_pixel);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_get_width", Image_get_width);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_get_height", Image_get_height);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE, "image_get_size", Image_get_size);
}

/** @} */
/** @} */
