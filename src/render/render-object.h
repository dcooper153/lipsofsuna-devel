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
#include "render-context.h"
#include "render-material.h"
#include "render-model.h"
#include "render-types.h"

struct _lirndObject
{
	int id;
	int realized;
	GLuint buffer;
	void* userdata;
	limatTransform transform;
	limdlVertex* vertices;
	lirndScene* scene;
	lirndModel* model;
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
		int count;
		lirndMaterial** array;
	} materials;
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
lirnd_object_deform (lirndObject* self,
                     limdlPose*   pose);

void
lirnd_object_emit_particles (lirndObject* self);

void
lirnd_object_render (lirndObject*  self,
                     lirndContext* context);

void
lirnd_object_render_debug (lirndObject* self,
                           lirndRender* render);

void
lirnd_object_render_group (lirndObject*  self,
                           lirndContext* context,
                           int           group);

void
lirnd_object_replace_image (lirndObject* self,
                            lirndImage*  replace,
                            lirndImage*  replacement);

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
                        lirndModel*  model,
                        limdlPose*   pose);

int
lirnd_object_get_realized (const lirndObject* self);

int
lirnd_object_set_realized (lirndObject* self,
                           int          value);

void
lirnd_object_set_transform (lirndObject*          self,
                            const limatTransform* transform);

void*
lirnd_object_get_userdata (const lirndObject* self);

void
lirnd_object_set_userdata (lirndObject* self,
                           void*        value);

#endif

/** @} */
/** @} */

