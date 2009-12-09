/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndImage Image
 * @{
 */

#ifndef __RENDER_IMAGE_H__
#define __RENDER_IMAGE_H__

#include <image/lips-image.h>
#include "render-types.h"

typedef struct _lirndImage lirndImage;
struct _lirndImage
{
	int added;
	char* name;
	char* path;
	liimgTexture* texture;
	lirndRender* render;
};

lirndImage*
lirnd_image_new (lirndRender* render,
                 const char*  name);

lirndImage*
lirnd_image_new_from_file (lirndRender* render,
                           const char*  name);

void
lirnd_image_free (lirndImage* self);

int
lirnd_image_load (lirndImage* self);

#endif

/** @} */
/** @} */
