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
 * \addtogroup lirndObject Object
 * @{
 */

#ifndef __RENDER_OBJECT_H__
#define __RENDER_OBJECT_H__

#include <math/lips-math.h>
#include <model/lips-model.h>
#include "render.h"
#include "render-buffer.h"
#include "render-context.h"
#include "render-material.h"
#include "render-model.h"
#include "render-types.h"

struct _lirndObject
{
	int id;
	int realized;
	void* userdata;
	limatAabb aabb;
	limatTransform transform;
	limdlPose* pose;
	lirndScene* scene;
	lirndModel* model;
	lirndModel* instance;
	struct
	{
		int width;
		int height;
		GLuint depth;
		GLuint map;
		GLuint fbo[6];
	} cubemap;
	struct
	{
		int count;
		lirndLight** array;
	} lights;
	struct
	{
		limatVector center;
		limatMatrix matrix;
	} orientation;
};

lirndObject*
lirnd_object_new (lirndScene* scene,
                  int         id);

void
lirnd_object_free (lirndObject* self);

void
lirnd_object_deform (lirndObject* self);

void
lirnd_object_emit_particles (lirndObject* self);

void
lirnd_object_update (lirndObject* self,
                     float        secs);

void
lirnd_object_get_bounds (const lirndObject* self,
                         limatAabb*         result);

void
lirnd_object_get_center (const lirndObject* self,
                         limatVector*       center);

int
lirnd_object_set_model (lirndObject* self,
                        lirndModel*  model);

int
lirnd_object_set_pose (lirndObject* self,
                       limdlPose*   pose);

int
lirnd_object_get_realized (const lirndObject* self);

int
lirnd_object_set_realized (lirndObject* self,
                           int          value);

void
lirnd_object_get_transform (lirndObject*    self,
                            limatTransform* value);

void
lirnd_object_set_transform (lirndObject*          self,
                            const limatTransform* value);

void*
lirnd_object_get_userdata (const lirndObject* self);

void
lirnd_object_set_userdata (lirndObject* self,
                           void*        value);

#endif

/** @} */
/** @} */

