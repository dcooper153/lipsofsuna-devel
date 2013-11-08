/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtImageMerge ImageMerge
 * @{
 */

#include "async-merger.h"
#include "module.h"

static void ImageMerger_new (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIScrData* data;

	/* Allocate the merger. */
	self = liimg_async_merger_new ();
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_IMAGE_MERGER, liimg_async_merger_free);
	if (data == NULL)
	{
		liimg_async_merger_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void ImageMerger_add_hsv (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIImgColorHSV hsv;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_float (args, 0, &hsv.h))
		hsv.h = 0.0f;
	if (!liscr_args_geti_float (args, 1, &hsv.s))
		hsv.s = 0.0f;
	if (!liscr_args_geti_float (args, 2, &hsv.v))
		hsv.v = 0.0f;

	/* Add the task. */
	liimg_async_merger_add_hsv (self, &hsv);
}

static void ImageMerger_add_hsv_weightv (LIScrArgs* args)
{
	float val_range;
	LIImgAsyncMerger* self;
	LIImgColorHSV hsv;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_float (args, 0, &hsv.h))
		hsv.h = 0.0f;
	if (!liscr_args_geti_float (args, 1, &hsv.s))
		hsv.s = 0.0f;
	if (!liscr_args_geti_float (args, 2, &hsv.v))
		hsv.v = 0.0f;
	if (!liscr_args_geti_float (args, 3, &val_range))
		val_range = 0.2f;

	/* Add the task. */
	liimg_async_merger_add_hsv_weightv (self, &hsv, val_range);
}

static void ImageMerger_blit (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIImgImage* image;
	LIScrData* data;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	image = liscr_data_get_data (data);

	/* Add the task. */
	liimg_async_merger_blit (self, image, NULL, NULL);
}

static void ImageMerger_blit_hsv_add (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIImgColorHSV hsv;
	LIImgImage* image;
	LIScrData* data;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	if (!liscr_args_geti_float (args, 1, &hsv.h))
		hsv.h = 0.0f;
	if (!liscr_args_geti_float (args, 2, &hsv.s))
		hsv.s = 0.0f;
	if (!liscr_args_geti_float (args, 3, &hsv.v))
		hsv.v = 0.0f;
	image = liscr_data_get_data (data);

	/* Add the task. */
	liimg_async_merger_blit_hsv_add (self, image, NULL, NULL, &hsv);
}

static void ImageMerger_blit_hsv_add_weightv (LIScrArgs* args)
{
	float val_range;
	LIImgAsyncMerger* self;
	LIImgColorHSV hsv;
	LIImgImage* image;
	LIScrData* data;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	if (!liscr_args_geti_float (args, 1, &hsv.h))
		hsv.h = 0.0f;
	if (!liscr_args_geti_float (args, 2, &hsv.s))
		hsv.s = 0.0f;
	if (!liscr_args_geti_float (args, 3, &hsv.v))
		hsv.v = 0.0f;
	if (!liscr_args_geti_float (args, 4, &val_range))
		val_range = 0.2f;
	image = liscr_data_get_data (data);

	/* Add the task. */
	liimg_async_merger_blit_hsv_add_weightv (self, image, NULL, NULL, &hsv, val_range);
}

static void ImageMerger_blit_rect (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIImgImage* image;
	LIScrData* data;
	LIMatRectInt dst_rect;
	LIMatRectInt src_rect;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	image = liscr_data_get_data (data);
	if (!liscr_args_geti_int (args, 1, &dst_rect.x))
		return;
	if (!liscr_args_geti_int (args, 2, &dst_rect.y))
		return;
	if (!liscr_args_geti_int (args, 3, &dst_rect.width))
		return;
	if (!liscr_args_geti_int (args, 4, &dst_rect.height))
		return;
	if (!liscr_args_geti_int (args, 5, &src_rect.x))
		return;
	if (!liscr_args_geti_int (args, 6, &src_rect.y))
		return;
	src_rect.width = dst_rect.width;
	src_rect.height = dst_rect.height;

	/* Add the task. */
	liimg_async_merger_blit (self, image, &dst_rect, &src_rect);
}

static void ImageMerger_blit_rect_hsv_add (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIImgColorHSV hsv;
	LIImgImage* image;
	LIScrData* data;
	LIMatRectInt dst_rect;
	LIMatRectInt src_rect;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	if (!liscr_args_geti_int (args, 1, &dst_rect.x))
		return;
	if (!liscr_args_geti_int (args, 2, &dst_rect.y))
		return;
	if (!liscr_args_geti_int (args, 3, &dst_rect.width))
		return;
	if (!liscr_args_geti_int (args, 4, &dst_rect.height))
		return;
	if (!liscr_args_geti_int (args, 5, &src_rect.x))
		return;
	if (!liscr_args_geti_int (args, 6, &src_rect.y))
		return;
	src_rect.width = dst_rect.width;
	src_rect.height = dst_rect.height;
	if (!liscr_args_geti_float (args, 7, &hsv.h))
		hsv.h = 0.0f;
	if (!liscr_args_geti_float (args, 8, &hsv.s))
		hsv.s = 0.0f;
	if (!liscr_args_geti_float (args, 9, &hsv.v))
		hsv.v = 0.0f;
	image = liscr_data_get_data (data);

	/* Add the task. */
	liimg_async_merger_blit_hsv_add (self, image, &dst_rect, &src_rect, &hsv);
}

static void ImageMerger_blit_rect_hsv_add_weightv (LIScrArgs* args)
{
	float val_range;
	LIImgAsyncMerger* self;
	LIImgColorHSV hsv;
	LIImgImage* image;
	LIScrData* data;
	LIMatRectInt dst_rect;
	LIMatRectInt src_rect;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	if (!liscr_args_geti_int (args, 1, &dst_rect.x))
		return;
	if (!liscr_args_geti_int (args, 2, &dst_rect.y))
		return;
	if (!liscr_args_geti_int (args, 3, &dst_rect.width))
		return;
	if (!liscr_args_geti_int (args, 4, &dst_rect.height))
		return;
	if (!liscr_args_geti_int (args, 5, &src_rect.x))
		return;
	if (!liscr_args_geti_int (args, 6, &src_rect.y))
		return;
	src_rect.width = dst_rect.width;
	src_rect.height = dst_rect.height;
	if (!liscr_args_geti_float (args, 7, &hsv.h))
		hsv.h = 0.0f;
	if (!liscr_args_geti_float (args, 8, &hsv.s))
		hsv.s = 0.0f;
	if (!liscr_args_geti_float (args, 9, &hsv.v))
		hsv.v = 0.0f;
	if (!liscr_args_geti_float (args, 10, &val_range))
		val_range = 0.2f;
	image = liscr_data_get_data (data);

	/* Add the task. */
	liimg_async_merger_blit_hsv_add_weightv (self, image, &dst_rect, &src_rect, &hsv, val_range);
}

static void ImageMerger_finish (LIScrArgs* args)
{
	LIImgAsyncMerger* self;

	/* Get arguments. */
	self = args->self;

	/* Add the task. */
	liimg_async_merger_finish (self);
}

static void ImageMerger_pop_image (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIImgImage* image;
	LIScrData* data;

	/* Get arguments. */
	self = args->self;

	/* Get the image. */
	image = liimg_async_merger_pop_image (self);
	if (image == NULL)
		return;

	/* Create the script image. */
	data = liscr_data_new (args->script, args->lua, image, LIEXT_SCRIPT_IMAGE, liimg_image_free);
	if (data == NULL)
	{
		liimg_image_free (image);
		return;
	}
	liscr_args_seti_stack (args);
}

static void ImageMerger_replace (LIScrArgs* args)
{
	LIImgAsyncMerger* self;
	LIImgImage* image;
	LIScrData* data;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_IMAGE, &data))
		return;
	image = liscr_data_get_data (data);

	/* Add the task. */
	liimg_async_merger_replace (self, image);
}

/*****************************************************************************/

void liext_script_image_merger (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_new", ImageMerger_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_add_hsv", ImageMerger_add_hsv);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_add_hsv_weightv", ImageMerger_add_hsv_weightv);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_blit", ImageMerger_blit);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_blit_hsv_add", ImageMerger_blit_hsv_add);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_blit_hsv_add_weightv", ImageMerger_blit_hsv_add_weightv);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_blit_rect", ImageMerger_blit_rect);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_blit_rect_hsv_add", ImageMerger_blit_rect_hsv_add);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_blit_rect_hsv_add_weightv", ImageMerger_blit_rect_hsv_add_weightv);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_finish", ImageMerger_finish);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_pop_image", ImageMerger_pop_image);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_IMAGE_MERGER, "image_merger_replace", ImageMerger_replace);
}

/** @} */
/** @} */
