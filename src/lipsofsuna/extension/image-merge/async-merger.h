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

#ifndef __EXT_IMAGE_MERGE_ASYNC_MERGER_H__
#define __EXT_IMAGE_MERGE_ASYNC_MERGER_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/extension/image/module.h"

enum
{
	LIIMG_ASYNC_MERGER_ADD_HSV,
	LIIMG_ASYNC_MERGER_ADD_HSV_WEIGHTV,
	LIIMG_ASYNC_MERGER_BLIT,
	LIIMG_ASYNC_MERGER_BLIT_HSV_ADD,
	LIIMG_ASYNC_MERGER_BLIT_HSV_ADD_WEIGHTV,
	LIIMG_ASYNC_MERGER_FINISH,
	LIIMG_ASYNC_MERGER_REPLACE
};

typedef union _LIImgAsyncMergerTask LIImgAsyncMergerTask;
union _LIImgAsyncMergerTask
{
	int type;
	struct
	{
		int type;
		LIImgImage* image;
		LIImgColorHSV hsv;
	} add_hsv;
	struct
	{
		int type;
		LIImgImage* image;
		LIImgColorHSV hsv;
		float val_range;
	} add_hsv_weightv;
	struct
	{
		int type;
		LIImgImage* image;
		LIImgImage* blit;
		LIMatRectInt dst_rect;
		LIMatRectInt src_rect;
	} blit;
	struct
	{
		int type;
		LIImgImage* image;
		LIImgImage* blit;
		LIImgColorHSV hsv;
		LIMatRectInt dst_rect;
		LIMatRectInt src_rect;
	} blit_hsv_add;
	struct
	{
		int type;
		LIImgImage* image;
		LIImgImage* blit;
		LIImgColorHSV hsv;
		float val_range;
		LIMatRectInt dst_rect;
		LIMatRectInt src_rect;
	} blit_hsv_add_weightv;
	struct
	{
		int type;
		LIImgImage* image;
	} finish;
	struct
	{
		int type;
		LIImgImage* image;
		LIImgImage* replace;
	} replace;
};

typedef struct _LIImgAsyncMerger LIImgAsyncMerger;
struct _LIImgAsyncMerger
{
	LIImgImage* image;
	LISysSerialWorker* worker;
};

LIAPICALL (LIImgAsyncMerger*, liimg_async_merger_new, ());

LIAPICALL (void, liimg_async_merger_free, (
	LIImgAsyncMerger* self));

LIAPICALL (int, liimg_async_merger_add_hsv, (
	LIImgAsyncMerger*    self,
	const LIImgColorHSV* hsv));

LIAPICALL (int, liimg_async_merger_add_hsv_weightv, (
	LIImgAsyncMerger*    self,
	const LIImgColorHSV* hsv,
	float                val_range));

LIAPICALL (int, liimg_async_merger_blit, (
	LIImgAsyncMerger*   self,
	const LIImgImage*   image,
	const LIMatRectInt* dst_rect,
	const LIMatRectInt* src_rect));

LIAPICALL (int, liimg_async_merger_blit_hsv_add, (
	LIImgAsyncMerger*    self,
	const LIImgImage*    image,
	const LIMatRectInt*  dst_rect,
	const LIMatRectInt*  src_rect,
	const LIImgColorHSV* hsv));

LIAPICALL (int, liimg_async_merger_blit_hsv_add_weightv, (
	LIImgAsyncMerger*    self,
	const LIImgImage*    image,
	const LIMatRectInt*  dst_rect,
	const LIMatRectInt*  src_rect,
	const LIImgColorHSV* hsv,
	float                val_range));

LIAPICALL (int, liimg_async_merger_finish, (
	LIImgAsyncMerger* self));

LIAPICALL (int, liimg_async_merger_replace, (
	LIImgAsyncMerger* self,
	const LIImgImage* image));

LIAPICALL (LIImgImage*, liimg_async_merger_pop_image, (
	LIImgAsyncMerger* self));

#endif
