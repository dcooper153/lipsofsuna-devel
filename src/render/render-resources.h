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
 * \addtogroup lirndResources Resources
 * @{
 */

#ifndef __RENDER_RESOURCES_H__
#define __RENDER_RESOURCES_H__

#include <image/lips-image.h>
#include "render-image.h"
#include "render-shader.h"
#include "render-types.h"

struct _lirndResources
{
	lialgStrdic* images;
	lialgStrdic* shaders;
	lirndRender* render;
};

lirndResources*
lirnd_resources_new (lirndRender* render);

void
lirnd_resources_free (lirndResources* self);

lirndImage*
lirnd_resources_find_image (lirndResources* self,
                            const char*     name);

lirndShader*
lirnd_resources_find_shader (lirndResources* self,
                             const char*     name);

lirndImage*
lirnd_resources_insert_image (lirndResources* self,
                              const char*     name);

int
lirnd_resources_get_image_count (lirndResources* self);

int
lirnd_resources_get_shader_count (lirndResources* self);

#endif

/** @} */
/** @} */
