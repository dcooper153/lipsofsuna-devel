/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __ALGORITHM_CAMERA_H__
#define __ALGORITHM_CAMERA_H__

#include "lipsofsuna/math.h"

typedef struct _LIExtCamera LIExtCamera;
typedef int LIExtCameraDriver;
typedef float (*LIExtCameraClip)(void*, LIExtCamera*, LIMatTransform*, LIMatTransform*);

enum 
{
	LIALG_CAMERA_FIRSTPERSON,
	LIALG_CAMERA_THIRDPERSON,
	LIALG_CAMERA_MANUAL,
	LIALG_CAMERA_MAX
};

struct _LIExtCamera
{
	struct
	{
		int collision_mask;
		int collision_group;
		float distance;
		LIExtCameraDriver driver;
		LIExtCameraClip clip_func;
		void* clip_data;
	} config;
	struct
	{
		float move_rate;
		float tilt_rate;
		float turn_rate;
		float zoom_rate;
	} controls;
	struct
	{
		float pos;
		float rot;
		float timer;
	} smoothing;
	struct
	{
		LIMatTransform center;
		LIMatTransform current;
		LIMatTransform local;
		LIMatTransform target;
		LIMatTransform inverse;
	} transform;
	struct
	{
		float aspect;
		float fov;
		float nearplane;
		float farplane;
		int viewport[4];
		LIMatMatrix modelview;
		LIMatMatrix projection;
	} view;
};

LIAPICALL (LIExtCamera*, liext_camera_new, ());

LIAPICALL (void, liext_camera_free, (
	LIExtCamera* self));

LIAPICALL (void, liext_camera_move, (
	LIExtCamera* self,
	float        value));

LIAPICALL (int, liext_camera_project, (
	LIExtCamera*       self,
	const LIMatVector* object,
	LIMatVector*       window));

LIAPICALL (void, liext_camera_tilt, (
	LIExtCamera* self,
	float        value));

LIAPICALL (void, liext_camera_turn, (
	LIExtCamera* self,
	float        value));

LIAPICALL (int, liext_camera_unproject, (
	LIExtCamera*       self,
	const LIMatVector* window,
	LIMatVector*       object));

LIAPICALL (void, liext_camera_update, (
	LIExtCamera* self,
	float        secs));

LIAPICALL (void, liext_camera_warp, (
	LIExtCamera* self));

LIAPICALL (void, liext_camera_zoom, (
	LIExtCamera* self,
	float        value));

LIAPICALL (void, liext_camera_get_bounds, (
	const LIExtCamera* self,
	LIMatAabb*         aabb));

LIAPICALL (void, liext_camera_get_center, (
	LIExtCamera*    self,
	LIMatTransform* result));

LIAPICALL (void, liext_camera_set_center, (
	LIExtCamera*          self,
	const LIMatTransform* value));

LIAPICALL (void, liext_camera_set_clipping, (
	LIExtCamera*    self,
	LIExtCameraClip func,
	void*           data));

LIAPICALL (LIExtCameraDriver, liext_camera_get_driver, (
	LIExtCamera* self));

LIAPICALL (void, liext_camera_set_driver, (
	LIExtCamera*      self,
	LIExtCameraDriver value));

LIAPICALL (void, liext_camera_set_far, (
	LIExtCamera* self,
	float        value));

LIAPICALL (float, liext_camera_get_fov, (
	const LIExtCamera* self));

LIAPICALL (void, liext_camera_set_fov, (
	LIExtCamera* self,
	float        value));

LIAPICALL (void, liext_camera_get_frustum, (
	const LIExtCamera* self,
	LIMatFrustum*      result));

LIAPICALL (void, liext_camera_get_modelview, (
	const LIExtCamera* self,
	LIMatMatrix*       value));

LIAPICALL (void, liext_camera_set_near, (
	LIExtCamera* self,
	float        value));

LIAPICALL (void, liext_camera_get_projection, (
	const LIExtCamera* self,
	LIMatMatrix*       value));

LIAPICALL (void, liext_camera_set_projection, (
	LIExtCamera* self,
	float        fov,
	float        aspect,
	float        nearplane,
	float        farplane));

LIAPICALL (void, liext_camera_get_smoothing, (
	const LIExtCamera* self,
	float*             pos,
	float*             rot));

LIAPICALL (void, liext_camera_set_smoothing, (
	LIExtCamera* self,
	float        pos,
	float        rot));

LIAPICALL (void, liext_camera_get_transform, (
	const LIExtCamera* self,
	LIMatTransform*    value));

LIAPICALL (void, liext_camera_set_transform, (
	LIExtCamera*          self,
	const LIMatTransform* value));

LIAPICALL (void, liext_camera_get_up, (
	const LIExtCamera* self,
	LIMatVector*       result));

LIAPICALL (void, liext_camera_set_viewport, (
	LIExtCamera* self,
	int          x,
	int          y,
	int          width,
	int          height));

#endif
