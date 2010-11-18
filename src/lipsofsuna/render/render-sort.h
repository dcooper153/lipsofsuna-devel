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

#ifndef __RENDER_SORT_H__
#define __RENDER_SORT_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-buffer.h"
#include "render-material.h"
#include "render-mesh.h"
#include "render-types.h"

enum
{
	LIREN_SORT_TYPE_FACE,
	LIREN_SORT_TYPE_PARTICLE
};

struct _LIRenSortgroup
{
	int index;
	int count;
	int transparent;
	LIMatAabb bounds;
	LIMatMatrix matrix;
	LIRenMesh* mesh;
	LIRenMaterial* material;
};

struct _LIRenSortface
{
	int type;
	LIRenSortface* next;
	union
	{
		struct
		{
			int index;
			LIMatAabb bounds;
			LIMatMatrix matrix;
			LIRenMesh* mesh;
			LIRenMaterial* material;
		} face;
		struct
		{
			float size;
			float diffuse[4];
			LIMatVector position;
			LIRenImage* image;
			LIRenShader* shader;
		} particle;
	};
};

struct _LIRenSort
{
	LIMatFrustum frustum;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenRender* render;
	struct
	{
		int count;
		LIRenSortface** array;
	} buckets;
	struct
	{
		int count;
		int capacity;
		LIRenSortface* array;
	} faces;
	struct
	{
		int count;
		int capacity;
		LIRenSortgroup* array;
	} groups;
};

LIAPICALL (LIRenSort*, liren_sort_new, (
	LIRenRender* render));

LIAPICALL (void, liren_sort_free, (
	LIRenSort* self));

LIAPICALL (int, liren_sort_add_group, (
	LIRenSort*     self,
	LIMatAabb*     bounds,
	LIMatMatrix*   matrix,
	int            index,
	int            count,
	LIRenMesh*     mesh,
	LIRenMaterial* material,
	LIMatVector*   face_sort_coords));

LIAPICALL (int, liren_sort_add_faces, (
	LIRenSort*     self,
	LIMatAabb*     bounds,
	LIMatMatrix*   matrix,
	int            index,
	int            count,
	LIRenMesh*     mesh,
	LIRenMaterial* material,
	LIMatVector*   face_sort_coords));

LIAPICALL (int, liren_sort_add_model, (
	LIRenSort*   self,
	LIMatAabb*   bounds,
	LIMatMatrix* matrix,
	LIRenModel*  model));

LIAPICALL (int, liren_sort_add_object, (
	LIRenSort*   self,
	LIRenObject* object));

int liren_sort_add_particle (
	LIRenSort*         self,
	const LIMatVector* position,
	float              size,
	const float*       diffuse,
	LIRenImage*        image,
	LIRenShader*       shader);

LIAPICALL (void, liren_sort_clear, (
	LIRenSort*         self,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection));

#endif
