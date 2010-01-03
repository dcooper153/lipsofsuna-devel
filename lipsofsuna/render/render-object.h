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

/**
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenObject Object
 * @{
 */

#ifndef __RENDER_OBJECT_H__
#define __RENDER_OBJECT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include "render.h"
#include "render-buffer.h"
#include "render-context.h"
#include "render-material.h"
#include "render-model.h"
#include "render-types.h"

struct _LIRenObject
{
	int id;
	int realized;
	void* userdata;
	LIMatAabb aabb;
	LIMatTransform transform;
	LIMdlPose* pose;
	LIRenScene* scene;
	LIRenModel* model;
	LIRenModel* instance;
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
		LIRenLight** array;
	} lights;
	struct
	{
		LIMatVector center;
		LIMatMatrix matrix;
	} orientation;
};

LIRenObject*
liren_object_new (LIRenScene* scene,
                  int         id);

void
liren_object_free (LIRenObject* self);

void
liren_object_deform (LIRenObject* self);

void
liren_object_emit_particles (LIRenObject* self);

void
liren_object_update (LIRenObject* self,
                     float        secs);

void
liren_object_get_bounds (const LIRenObject* self,
                         LIMatAabb*         result);

void
liren_object_get_center (const LIRenObject* self,
                         LIMatVector*       center);

int
liren_object_set_model (LIRenObject* self,
                        LIRenModel*  model);

int
liren_object_set_pose (LIRenObject* self,
                       LIMdlPose*   pose);

int
liren_object_get_realized (const LIRenObject* self);

int
liren_object_set_realized (LIRenObject* self,
                           int          value);

void
liren_object_get_transform (LIRenObject*    self,
                            LIMatTransform* value);

void
liren_object_set_transform (LIRenObject*          self,
                            const LIMatTransform* value);

void*
liren_object_get_userdata (const LIRenObject* self);

void
liren_object_set_userdata (LIRenObject* self,
                           void*        value);

#endif

/** @} */
/** @} */

