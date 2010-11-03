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

#ifndef __RENDER_LIGHT_H__
#define __RENDER_LIGHT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "render.h"
#include "render-types.h"

/* FIXME: Should be configurable. */
#define SHADOWMAPSIZE 512

struct _LIRenLight
{
	int directional;
	int enabled;
	float rating;
	float cutoff;
	float exponent;
	float shadow_far;
	float shadow_near;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float equation[3];
	LIMatAabb bounds;
	LIMatMatrix projection;
	LIMatMatrix modelview;
	LIMatMatrix modelview_inverse;
	LIMatTransform transform;
	const LIMdlNode* node;
	LIRenScene* scene;
	struct
	{
		GLuint fbo;
		GLuint map;
	} shadow;
};

LIAPICALL (LIRenLight*, liren_light_new, (
	LIRenScene*  scene,
	const float* color,
	const float* equation,
	float        cutoff,
	float        exponent,
	int          shadow));

LIAPICALL (LIRenLight*, liren_light_new_directional, (
	LIRenScene*  scene,
	const float* color));

LIAPICALL (LIRenLight*, liren_light_new_from_model, (
	LIRenScene*      scene,
	const LIMdlNode* light));

LIAPICALL (void, liren_light_free, (
	LIRenLight* self));

LIAPICALL (int, liren_light_compare, (
	const LIRenLight* self,
	const LIRenLight* light));

LIAPICALL (void, liren_light_update, (
	LIRenLight* self));

LIAPICALL (void, liren_light_update_projection, (
	LIRenLight* self));

LIAPICALL (void, liren_light_set_ambient, (
	LIRenLight*  self,
	const float* value));

LIAPICALL (int, liren_light_get_bounds, (
	const LIRenLight* self,
	LIMatAabb*        result));

LIAPICALL (void, liren_light_get_direction, (
	const LIRenLight* self,
	LIMatVector*      value));

LIAPICALL (void, liren_light_set_direction, (
	LIRenLight*        self,
	const LIMatVector* value));

LIAPICALL (void, liren_light_set_directional, (
	LIRenLight* self,
	int         value));

LIAPICALL (int, liren_light_get_enabled, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_get_modelview, (
	const LIRenLight* self,
	LIMatMatrix*      value));

LIAPICALL (void, liren_light_get_position, (
	const LIRenLight* self,
	GLfloat*          value));

LIAPICALL (void, liren_light_get_transform, (
	LIRenLight*     self,
	LIMatTransform* value));

LIAPICALL (void, liren_light_set_transform, (
	LIRenLight*           self,
	const LIMatTransform* transform));

LIAPICALL (void, liren_light_get_projection, (
	const LIRenLight* self,
	LIMatMatrix*      value));

LIAPICALL (void, liren_light_set_projection, (
	LIRenLight*        self,
	const LIMatMatrix* value));

LIAPICALL (int, liren_light_get_shadow, (
	const LIRenLight* self));

LIAPICALL (void, liren_light_set_shadow, (
	LIRenLight* self,
	int         value));

LIAPICALL (int, liren_light_get_type, (
	const LIRenLight* self));

#endif
