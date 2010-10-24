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

#ifndef __ENGINE_OBJECT_H__
#define __ENGINE_OBJECT_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/system.h>
#include "engine-model.h"
#include "engine-types.h"

#ifndef LIENG_DISABLE_SCRIPT
#include <lipsofsuna/script.h>
#endif

#define LIENG_OBJECT(o) ((LIEngObject*)(o))

struct _LIEngObject
{
	int refs;
	void* userdata;
	uint32_t id;
	uint16_t flags;
	LIEngEngine* engine;
	LIEngModel* model;
	LIEngSector* sector;
	LIMatTransform transform;
	LIMatTransform transform_event;
	LIMdlPose* pose;
#ifndef LIENG_DISABLE_SCRIPTS
	LIScrData* script;
#endif
	struct
	{
		float pos;
		float rot;
		LIMatTransform target;
	} smoothing;
};

LIAPICALL (LIEngObject*, lieng_object_new, (
	LIEngEngine* engine,
	uint32_t     id));

LIAPICALL (void, lieng_object_free, (
	LIEngObject* self));

LIAPICALL (void, lieng_object_ref, (
	LIEngObject* self,
	int          count));

LIAPICALL (int, lieng_object_animate, (
	LIEngObject* self,
	int          channel,
	const char*  animation,
	int          permanent,
	float        priority,
	float        time,
	float        fade_in,
	float        fade_out));

LIAPICALL (LIMdlNode*, lieng_object_find_node, (
	LIEngObject* self,
	const char*  name));

LIAPICALL (int, lieng_object_merge_model, (
	LIEngObject* self,
	LIEngModel*  model));

LIAPICALL (int, lieng_object_moved, (
	LIEngObject* self));

LIAPICALL (void, lieng_object_refresh, (
	LIEngObject* self,
	float        radius));

LIAPICALL (void, lieng_object_update, (
	LIEngObject* self,
	float        secs));

LIAPICALL (void, lieng_object_get_bounds, (
	const LIEngObject* self,
	LIMatAabb*         bounds));

LIAPICALL (void, lieng_object_get_bounds_transform, (
	const LIEngObject* self,
	LIMatAabb*         bounds));

LIAPICALL (int, lieng_object_get_dirty, (
	const LIEngObject* self));

LIAPICALL (void, lieng_object_set_dirty, (
	LIEngObject* self,
	int          value));

LIAPICALL (float, lieng_object_get_distance, (
	const LIEngObject* self,
	const LIEngObject* object));

LIAPICALL (int, lieng_object_get_flags, (
	const LIEngObject* self));

LIAPICALL (void, lieng_object_set_flags, (
	LIEngObject* self,
	int          flags));

LIAPICALL (int, lieng_object_set_model, (
	LIEngObject* self,
	LIEngModel*  model));

LIAPICALL (int, lieng_object_get_realized, (
	const LIEngObject* self));

LIAPICALL (int, lieng_object_set_realized, (
	LIEngObject* self,
	int          value));

LIAPICALL (LIEngSector*, lieng_object_get_sector, (
	LIEngObject* self));

LIAPICALL (int, lieng_object_get_selected, (
	const LIEngObject* self));

LIAPICALL (int, lieng_object_set_selected, (
	LIEngObject* self,
	int          select));

LIAPICALL (void, lieng_object_get_smoothing, (
	LIEngObject* self,
	float*       pos,
	float*       rot));

LIAPICALL (void, lieng_object_set_smoothing, (
	LIEngObject* self,
	float        pos,
	float        rot));

LIAPICALL (void, lieng_object_get_target, (
	const LIEngObject* self,
	LIMatTransform*    value));

LIAPICALL (void, lieng_object_get_transform, (
	const LIEngObject* self,
	LIMatTransform*    value));

LIAPICALL (int, lieng_object_set_transform, (
	LIEngObject*          self,
	const LIMatTransform* value));

LIAPICALL (void*, lieng_object_get_userdata, (
	LIEngObject* self));

LIAPICALL (void, lieng_object_set_userdata, (
	LIEngObject* self,
	void*        data));

#endif
