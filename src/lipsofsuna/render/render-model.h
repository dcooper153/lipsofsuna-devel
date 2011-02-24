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

#ifndef __RENDER_MODEL_H__
#define __RENDER_MODEL_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-buffer-texture.h"
#include "render-material.h"
#include "render-mesh.h"
#include "render-particles.h"
#include "render-types.h"

typedef struct _LIRenModelGroup LIRenModelGroup;
struct _LIRenModelGroup
{
	int start;
	int count;
	LIMatVector center;
};

struct _LIRenModel
{
	int id;
	LIMatAabb bounds;
	LIRenMesh mesh;
	LIRenParticles particles;
	LIRenRender* render;
	struct
	{
		int count;
		LIRenModelGroup* array;
	} groups;
	struct
	{
		int count;
		LIRenMaterial** array;
	} materials;
};

LIAPICALL (LIRenModel*, liren_model_new, (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id));

LIAPICALL (void, liren_model_free, (
	LIRenModel* self));

LIAPICALL (int, liren_model_deform, (
	LIRenModel*      self,
	const char*      shader,
	const LIMdlPose* pose));

LIAPICALL (int, liren_model_intersect_ray, (
	const LIRenModel*  self,
	const LIMatVector* ray0,
	const LIMatVector* ray1,
	LIMatVector*       result));

LIAPICALL (void, liren_model_replace_image, (
	LIRenModel* self,
	LIRenImage* image));

LIAPICALL (void, liren_model_update_transparency, (
	LIRenModel* self));

LIAPICALL (void, liren_model_get_bounds, (
	LIRenModel* self,
	LIMatAabb*  aabb));

LIAPICALL (int, liren_model_set_model, (
	LIRenModel* self,
	LIMdlModel* model));

#endif
