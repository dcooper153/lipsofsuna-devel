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
 * \addtogroup LIRenSort Sort
 * @{
 */

#ifndef __RENDER_SORT_H__
#define __RENDER_SORT_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-buffer.h"
#include "render-material.h"
#include "render-types.h"

struct _LIRenSortgroup
{
	int transparent;
	LIMatAabb bounds;
	LIMatMatrix matrix;
	LIRenBuffer* indices;
	LIRenBuffer* vertices;
	LIRenMaterial* material;
};

struct _LIRenSortface
{
	int index;
	LIMatAabb bounds;
	LIMatMatrix matrix;
	LIRenBuffer* indices;
	LIRenBuffer* vertices;
	LIRenSortface* next;
	LIRenMaterial* material;
};

struct _LIRenSort
{
	LIMatMatrix modelview;
	LIMatMatrix projection;
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
	LIRenBuffer*   indices,
	LIRenBuffer*   vertices,
	LIRenMaterial* material,
	int            transparent));

LIAPICALL (int, liren_sort_add_faces, (
	LIRenSort*     self,
	LIMatAabb*     bounds,
	LIMatMatrix*   matrix,
	LIRenBuffer*   indices,
	LIRenBuffer*   vertices,
	LIRenMaterial* material));

LIAPICALL (int, liren_sort_add_model, (
	LIRenSort*   self,
	LIMatAabb*   bounds,
	LIMatMatrix* matrix,
	LIRenModel*  model));

LIAPICALL (int, liren_sort_add_object, (
	LIRenSort*   self,
	LIRenObject* object));

LIAPICALL (void, liren_sort_clear, (
	LIRenSort* self));

#endif

/** @} */
/** @} */
