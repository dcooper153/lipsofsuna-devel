/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __RENDER_OBJECT_H__
#define __RENDER_OBJECT_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "render.h"

LIAPICALL (int, liren_render_object_new, (
	LIRenRender* render,
	int          id));

LIAPICALL (void, liren_render_object_free, (
	LIRenRender* self,
	int          id));

LIAPICALL (void, liren_render_object_channel_animate, (
	LIRenRender* self,
	int          id,
	int          channel,
	const char*  name,
	int          additive,
	int          repeat,
	int          repeat_start,
	int          keep,
	float        fade_in,
	float        fade_out,
	float        weight,
	float        weight_scale,
	float        time,
	float        time_scale,
	const char** node_names,
	float*       node_weights,
	int          node_count));

LIAPICALL (void, liren_render_object_channel_edit, (
	LIRenRender*          self,
	int                   id,
	int                   channel,
	int                   frame,
	const char*           node,
	const LIMatTransform* transform,
	float                 scale));

LIAPICALL (void, liren_render_object_channel_fade, (
	LIRenRender* self,
	int          id,
	int          channel,
	float        time));

LIAPICALL (LIMdlPoseChannel*, liren_render_object_channel_get_state, (
	LIRenRender* self,
	int          id,
	int          channel));

LIAPICALL (void, liren_render_object_deform, (
	LIRenRender* self,
	int          id));

LIAPICALL (int, liren_render_object_find_node, (
	LIRenRender*    self,
	int             id,
	const char*     name,
	int             world,
	LIMatTransform* result));

LIAPICALL (void, liren_render_object_particle_animation, (
	LIRenRender* self,
	int          id,
	float        start,
	int          loop));

LIAPICALL (void, liren_render_object_set_effect, (
	LIRenRender* self,
	int          id,
	const char*  shader,
	const float* params));

LIAPICALL (void, liren_render_object_set_model, (
	LIRenRender* self,
	int          id,
	int          model));

LIAPICALL (void, liren_render_object_set_realized, (
	LIRenRender* self,
	int          id,
	int          value));

LIAPICALL (void, liren_render_object_set_shadow, (
	LIRenRender* self,
	int          id,
	int          value));

LIAPICALL (void, liren_render_object_set_transform, (
	LIRenRender*          self,
	int                   id,
	const LIMatTransform* value));

#endif
