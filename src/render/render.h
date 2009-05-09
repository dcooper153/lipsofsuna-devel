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
 * \addtogroup lirndRender Render
 * @{
 */

#ifndef __RENDER_H__
#define __RENDER_H__

#include <algorithm/lips-algorithm.h>
#include <config/lips-config.h>
#include <image/lips-image.h>
#include <system/lips-system.h>
#include "render-constraint.h"
#include "render-light.h"
#include "render-lighting.h"
#include "render-material.h"
#include "render-object.h"
#include "render-particle.h"
#include "render-resources.h"
#include "render-scene.h"
#include "render-shader.h"
#include "render-types.h"

struct _lirndRender
{
	lirndLighting* lighting;
	lirndResources* resources;
	struct
	{
		char* dir;
	} config;
	struct
	{
		float time;
		GLuint noise;
	} helpers;
	struct
	{
		int count;
		int count_used;
		int count_free;
		lirndParticle* particles;
		lirndParticle* particles_used;
		lirndParticle* particles_free;
	} particle;
	struct
	{
		lirndObject* model;
	} sky;
	struct
	{
		int enabled;
		lirndShader* shader;
		lirndShader* shadowmap;
		lirndShader* fixed;
	} shader;
	struct
	{
		limatFrustum* frustum;
		limatMatrix* modelview;
		limatMatrix* projection;
		lirndScene* scene;
	} temporary;
	struct
	{
		lirndConstraint* constraints;
	} world;
};

lirndRender*
lirnd_render_new (const char* dir);

void
lirnd_render_free (lirndRender* self);

lirndShader*
lirnd_render_find_shader (lirndRender* self,
                          const char*  name);

lirndImage*
lirnd_render_find_image (lirndRender* self,
                         const char*  name);

int
lirnd_render_load_model (lirndRender* self,
                         const char*  name);

int
lirnd_render_load_image (lirndRender* self,
                         const char*  name);

void
lirnd_render_insert_constraint (lirndRender*     self,
                                lirndConstraint* constraint);

lirndParticle*
lirnd_render_insert_particle (lirndRender*    self,
                              const limatVector* position,
                              const limatVector* velocity);

int
lirnd_render_pick (lirndRender*    self,
                   lirndScene*     scene,
                   limatMatrix*    modelview,
                   limatMatrix*    projection,
                   limatFrustum*   frustum,
                   int             x,
                   int             y,
                   int             size,
                   lirndSelection* result);

void
lirnd_render_remove_constraint (lirndRender*     self,
                                lirndConstraint* constraint);

void
lirnd_render_render (lirndRender*  self,
                     lirndScene*   scene,
                     limatMatrix*  modelview,
                     limatMatrix*  projection,
                     limatFrustum* frustum);

void
lirnd_render_render_custom (lirndRender*  self,
                            lirndScene*   scene,
                            limatMatrix*  modelview,
                            limatMatrix*  projection,
                            limatFrustum* frustum,
                            lirndCallback call,
                            void*         data);


void
lirnd_render_update (lirndRender* self,
                     float        secs);

void
lirnd_render_set_global_shadows (lirndRender* self,
                                 int          value);

int
lirnd_render_get_light_count (const lirndRender* self);

void
lirnd_render_set_light_count (lirndRender* self,
                              int          count);
void
lirnd_render_set_light_focus (lirndRender*    self,
                              const limatVector* point);

void
lirnd_render_set_local_shadows (lirndRender* self,
                                int          value);

int
lirnd_render_get_shaders_enabled (const lirndRender* self);

void
lirnd_render_set_shaders_enabled (lirndRender* self,
                                  int          value);

int
lirnd_render_set_sky (lirndRender* self,
                      lirndModel*  model);

void
lirnd_render_set_sun (lirndRender*    self,
                      const limatVector* direction);

#endif

/** @} */
/** @} */
