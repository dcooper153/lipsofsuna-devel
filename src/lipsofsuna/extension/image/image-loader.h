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

#ifndef __EXT_IMAGE_IMAGE_LOADER_H__
#define __EXT_IMAGE_IMAGE_LOADER_H__

#include "lipsofsuna/system.h"
#include "image.h"

typedef struct _LIImgImageLoader LIImgImageLoader;
struct _LIImgImageLoader
{
	int done;
	char* path;
	LIImgImage* result;
	LISysMutex* mutex;
	LISysThread* thread;
};

LIAPICALL (LIImgImageLoader*, liimg_image_loader_new, (
	const char* path));

LIAPICALL (void, liimg_image_loader_free, (
	LIImgImageLoader* self));

LIAPICALL (int, liimg_image_loader_get_done, (
	LIImgImageLoader* self));

LIAPICALL (LIImgImage*, liimg_image_loader_get_image, (
	LIImgImageLoader* self));

#endif
