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
 * \addtogroup lirndScene Scene
 * @{
 */

#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include <algorithm/lips-algorithm.h>
#include <particle/lips-particle.h>
#include <system/lips-system.h>
#include "render-light.h"
#include "render-lighting.h"
#include "render-object.h"
#include "render-types.h"

struct _lirndScene
{
	lialgPtrdic* objects;
	liparManager* particles;
	lirndRender* render;
	lirndLighting* lighting;
	struct
	{
		lirndObject* model;
	} sky;
};

lirndScene*
lirnd_scene_new (lirndRender* render);

void
lirnd_scene_free (lirndScene* self);

liparPoint*
lirnd_scene_insert_particle (lirndScene*        self,
                             const limatVector* position,
                             const limatVector* velocity);

int
lirnd_scene_pick (lirndScene*     self,
                  limatMatrix*    modelview,
                  limatMatrix*    projection,
                  limatFrustum*   frustum,
                  const int*      viewport,
                  int             x,
                  int             y,
                  int             size,
                  lirndSelection* result);

void
lirnd_scene_render (lirndScene*   self,
                    limatMatrix*  modelview,
                    limatMatrix*  projection,
                    limatFrustum* frustum);

void
lirnd_scene_update (lirndScene* self,
                    float       secs);

void
lirnd_scene_set_light_focus (lirndScene*        self,
                             const limatVector* point);

int
lirnd_scene_set_sky (lirndScene* self,
                     lirndModel* model);

#endif

/** @} */
/** @} */
