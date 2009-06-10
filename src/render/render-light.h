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
 * \addtogroup lirndLight Light
 * @{
 */

#ifndef __RENDER_LIGHT_H__
#define __RENDER_LIGHT_H__

#include <math/lips-math.h>
#include <model/lips-model.h>
#include "render.h"
#include "render-scene.h"
#include "render-types.h"

/* FIXME: Should be configurable. */
#define SHADOWMAPSIZE 512

struct _lirndLight
{
	int directional;
	int enabled;
	float rating;
	float cutoff;
	float exponent;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float equation[3];
	limatMatrix projection;
	limatMatrix modelview;
	limatMatrix modelview_inverse;
	limatTransform transform;
	const limdlNode* node;
	lirndRender* render;
	struct
	{
		GLuint fbo;
		GLuint map;
	} shadow;
};

lirndLight*
lirnd_light_new (lirndRender* render,
                 const float* color,
                 const float* equation,
                 float        cutoff,
                 float        exponent,
                 int          shadow);

lirndLight*
lirnd_light_new_directional (lirndRender* self,
                             const float* color);

lirndLight*
lirnd_light_new_from_model (lirndRender*     render,
                            const limdlNode* light);

void
lirnd_light_free (lirndLight* self);

void
lirnd_light_bind (lirndLight*  self,
                  lirndCamera* camera,
                  int          number,
                  int          texture,
                  int          shadow);

int
lirnd_light_compare (const lirndLight* self,
                     const lirndLight* light);

void
lirnd_light_update (lirndLight* self,
                    lirndScene* scene);

void
lirnd_light_set_ambient (lirndLight*  self,
                         const float* value);

void
lirnd_light_get_direction (const lirndLight* self,
                           limatVector*      value);

void
lirnd_light_set_direction (lirndLight*        self,
                           const limatVector* value);

void
lirnd_light_set_directional (lirndLight* self,
                             int         value);

int
lirnd_light_get_enabled (const lirndLight* self);

void
lirnd_light_get_modelview (const lirndLight* self,
                           limatMatrix*      value);

void
lirnd_light_get_transform (lirndLight*     self,
                           limatTransform* value);

void
lirnd_light_set_transform (lirndLight*           self,
                           const limatTransform* transform);

void
lirnd_light_get_projection (const lirndLight* self,
                            limatMatrix*      value);

void
lirnd_light_set_projection (lirndLight*     self,
                            const limatMatrix* value);

#endif

/** @} */
/** @} */
