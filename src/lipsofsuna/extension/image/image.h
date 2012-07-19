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

#ifndef __EXT_IMAGE_IMAGE_H__
#define __EXT_IMAGE_IMAGE_H__

typedef struct _LIImgImage LIImgImage;
struct _LIImgImage
{
	int width;
	int height;
	void* pixels;
};

LIAPICALL (LIImgImage*, liimg_image_new, ());

LIAPICALL (LIImgImage*, liimg_image_new_from_file, (
	const char* path));

LIAPICALL (LIImgImage*, liimg_image_new_from_image, (
	LIImgImage* image));

LIAPICALL (void, liimg_image_free, (
	LIImgImage* self));

LIAPICALL (void, liimg_image_blit, (
	LIImgImage* self,
	LIImgImage* image));

LIAPICALL (int, liimg_image_load_dds, (
	LIImgImage* self,
	const char* path));

LIAPICALL (int, liimg_image_load_png, (
	LIImgImage* self,
	const char* path));

LIAPICALL (int, liimg_image_save_rgba, (
	LIImgImage* self,
	const char* path));

LIAPICALL (int, liimg_image_save_s3tc, (
	LIImgImage* self,
	const char* path));

LIAPICALL (void, liimg_image_shrink_half, (
	LIImgImage* self));

#endif
