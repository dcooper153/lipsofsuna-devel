/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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

#ifndef __RENDER_IMAGE_H__
#define __RENDER_IMAGE_H__

#include <lipsofsuna/image.h>
#include "render-types.h"

typedef struct _LIRenImage LIRenImage;
struct _LIRenImage
{
	int added;
	char* name;
	char* path;
	LIImgTexture* texture;
	LIRenRender* render;
};

LIRenImage*
liren_image_new (LIRenRender* render,
                 const char*  name);

LIRenImage*
liren_image_new_from_file (LIRenRender* render,
                           const char*  name);

void
liren_image_free (LIRenImage* self);

int
liren_image_load (LIRenImage* self);

#endif
