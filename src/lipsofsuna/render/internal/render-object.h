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

#ifndef __RENDER_INTERNAL_OBJECT_H__
#define __RENDER_INTERNAL_OBJECT_H__

#include "render-types.h"

LIAPICALL (LIRenObject*, liren_object_new, (
	LIRenRender* render,
	int          id));

LIAPICALL (void, liren_object_free, (
	LIRenObject* self));

LIAPICALL (void, liren_object_add_model, (
	LIRenObject* self,
	LIRenModel*  model));

LIAPICALL (int, liren_object_channel_animate, (
	LIRenObject*            self,
	int                     channel,
	int                     keep,
	const LIMdlPoseChannel* info));

LIAPICALL (void, liren_object_channel_edit, (
	LIRenObject*          self,
	int                   channel,
	int                   frame,
	const char*           node,
	const LIMatTransform* transform,
	float                 scale));

LIAPICALL (void, liren_object_channel_fade, (
	LIRenObject* self,
	int          channel,
	float        time));

LIAPICALL (LIMdlPoseChannel*, liren_object_channel_get_state, (
	LIRenObject* self,
	int          channel));

LIAPICALL (void, liren_object_clear_animations, (
	LIRenObject* self));

LIAPICALL (void, liren_object_clear_models, (
	LIRenObject* self));

LIAPICALL (int, liren_object_find_node, (
	LIRenObject*    self,
	const char*     name,
	int             world,
	LIMatTransform* result));

LIAPICALL (void, liren_object_particle_animation, (
	LIRenObject* self,
	float        start,
	int          loop));

LIAPICALL (void, liren_object_model_changed, (
	LIRenObject* self,
	LIRenModel*  model));

LIAPICALL (void, liren_object_remove_model, (
	LIRenObject* self,
	LIRenModel*  model));

LIAPICALL (void, liren_object_replace_model, (
	LIRenObject* self,
	LIRenModel*  model_old,
	LIRenModel*  model_new));

LIAPICALL (void, liren_object_update, (
	LIRenObject* self,
	float        secs));

LIAPICALL (int, liren_object_set_effect, (
	LIRenObject* self,
	const char*  shader,
	const float* params));

LIAPICALL (int, liren_object_get_id, (
	LIRenObject* self));

LIAPICALL (int, liren_object_get_loaded, (
	LIRenObject* self));

LIAPICALL (int, liren_object_set_model, (
	LIRenObject* self,
	LIRenModel*  model));

LIAPICALL (int, liren_object_set_particle, (
	LIRenObject* self,
	const char*  name));

LIAPICALL (void, liren_object_set_particle_emitting, (
	LIRenObject* self,
	int          value));

LIAPICALL (int, liren_object_set_realized, (
	LIRenObject* self,
	int          value));

LIAPICALL (void, liren_object_remove_model, (
	LIRenObject* self,
	LIRenModel*  model));

LIAPICALL (void, liren_object_set_render_distance, (
	LIRenObject* self,
	float        value));

LIAPICALL (void, liren_object_set_shadow, (
	LIRenObject* self,
	int          value));

LIAPICALL (void, liren_object_set_transform, (
	LIRenObject*          self,
	const LIMatTransform* value));

#endif
