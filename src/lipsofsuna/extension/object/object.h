/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __EXT_OBJECT_OBJECT_H__
#define __EXT_OBJECT_OBJECT_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/archive.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/script.h"
#include "object-manager.h"
#include "object-sector.h"
#include "object-types.h"

enum
{
	LIENG_OBJECT_FLAG_STATIC = 0x01,
	LIENG_OBJECT_FLAG_REALIZED = 0x02
};

struct _LIObjObject
{
	uint32_t id;
	int external_id;
	int flags;
	LIObjManager* manager;
	LIObjSector* sector;
	LIMatTransform transform;
	LIMatTransform transform_event;
	LIScrData* script;
};

LIAPICALL (LIObjObject*, liobj_object_new, (
	LIObjManager* manager));

LIAPICALL (void, liobj_object_free, (
	LIObjObject* self));

LIAPICALL (void, liobj_object_get_bounds, (
	const LIObjObject* self,
	LIMatAabb*         bounds));

LIAPICALL (int, liobj_object_get_external_id, (
	const LIObjObject* self));

LIAPICALL (void, liobj_object_set_external_id, (
	LIObjObject* self,
	int          value));

LIAPICALL (int, liobj_object_get_dirty, (
	const LIObjObject* self));

LIAPICALL (void, liobj_object_set_dirty, (
	LIObjObject* self,
	int          value));

LIAPICALL (float, liobj_object_get_distance, (
	const LIObjObject* self,
	const LIObjObject* object));

LIAPICALL (int, liobj_object_set_model, (
	LIObjObject* self,
	LIMdlModel*  model));

LIAPICALL (int, liobj_object_get_realized, (
	const LIObjObject* self));

LIAPICALL (int, liobj_object_set_realized, (
	LIObjObject* self,
	int          value));

LIAPICALL (LIObjSector*, liobj_object_get_sector, (
	LIObjObject* self));

LIAPICALL (int, liobj_object_get_static, (
	const LIObjObject* self));

LIAPICALL (void, liobj_object_set_static, (
	LIObjObject* self,
	int          value));

LIAPICALL (void, liobj_object_get_transform, (
	const LIObjObject* self,
	LIMatTransform*    value));

LIAPICALL (int, liobj_object_set_transform, (
	LIObjObject*          self,
	const LIMatTransform* value));

#endif
