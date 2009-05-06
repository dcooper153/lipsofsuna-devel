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
 * \addtogroup lirndLighting Lighting
 * @{
 */

#ifndef __RENDER_LIGHTING_H__
#define __RENDER_LIGHTING_H__

#include <algorithm/lips-algorithm.h>
#include <video/lips-video.h>
#include "render.h"
#include "render-light.h"
#include "render-scene.h"
#include "render-types.h"

struct _lirndLighting
{
	lirndRender* render;
	struct
	{
		int global_shadows;
		int local_shadows;
	} config;
	struct
	{
		int capacity;
		int count;
		lirndLight** active;
		lialgPtrdic* dict;
		GLuint depth_texture_max;
	} lights;
	struct
	{
		lirndLight* light;
		limatVector direction;
		float ambient[4];
	} sun;
};

lirndLighting*
lirnd_lighting_new (lirndRender* render);

void
lirnd_lighting_free (lirndLighting* self);

int
lirnd_lighting_insert_light (lirndLighting* self,
                             lirndLight*    light);

void
lirnd_lighting_remove_light (lirndLighting* self,
                             lirndLight*    light);

int
lirnd_lighting_create_light (lirndLighting*   self,
                             const limdlNode* node);

void
lirnd_lighting_update (lirndLighting* self,
                       lirndScene*    scene);

void
lirnd_lighting_set_ambient (lirndLighting* self,
                            const float*   value);

void
lirnd_lighting_set_center (lirndLighting*     self,
                           const limatVector* point);

void
lirnd_lighting_set_sun_color (lirndLighting* self,
                              const float*   value);

void
lirnd_lighting_set_sun_direction (lirndLighting*     self,
                                  const limatVector* value);

#endif

/** @} */
/** @} */
