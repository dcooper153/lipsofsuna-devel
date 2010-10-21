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

#ifndef __RENDER_GROUP_H__
#define __RENDER_GROUP_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-object.h"
#include "render-types.h"

typedef struct _LIRenGroupObject LIRenGroupObject;
struct _LIRenGroupObject
{
	LIMatAabb aabb;
	LIMatTransform transform;
	LIRenModel* model;
	LIRenGroupObject* next;
};

struct _LIRenGroup
{
	int realized;
	LIMatAabb aabb;
	LIRenScene* scene;
	LIRenGroupObject* objects;
	struct
	{
		int count;
		LIRenLight** array;
	} lights;
};

LIAPICALL (LIRenGroup*, liren_group_new, (
	LIRenScene* scene));

LIAPICALL (void, liren_group_free, (
	LIRenGroup* self));

LIAPICALL (void, liren_group_clear, (
	LIRenGroup* self));

LIAPICALL (int, liren_group_insert_model, (
	LIRenGroup*     self,
	LIRenModel*     model,
	LIMatTransform* transform));

LIAPICALL (void, liren_group_reload_model, (
	LIRenGroup* self,
	LIRenModel* model_old,
	LIRenModel* model_new));

LIAPICALL (void, liren_group_update, (
	LIRenGroup* self,
	float       secs));

LIAPICALL (void, liren_group_get_bounds, (
	const LIRenGroup* self,
	LIMatAabb*        result));

LIAPICALL (int, liren_group_get_realized, (
	const LIRenGroup* self));

LIAPICALL (int, liren_group_set_realized, (
	LIRenGroup* self,
	int         value));

#endif

/** @} */
/** @} */

