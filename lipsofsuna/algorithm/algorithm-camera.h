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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup LIAlgCamera Camera
 * @{
 */

#ifndef __ALGORITHM_CAMERA_H__
#define __ALGORITHM_CAMERA_H__

#include <lipsofsuna/math.h>

typedef int LIAlgCameraDriver;
enum 
{
	LIALG_CAMERA_FIRSTPERSON,
	LIALG_CAMERA_THIRDPERSON,
	LIALG_CAMERA_MANUAL,
	LIALG_CAMERA_MAX
};

typedef struct _LIAlgCamera LIAlgCamera;
struct _LIAlgCamera
{
	struct
	{
		float distance;
		LIAlgCameraDriver driver;
	} config;
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

LIAlgCamera*
lialg_camera_new ();

void
lialg_camera_free (LIAlgCamera* self);

void
lialg_camera_clip (LIAlgCamera* self,
                   float        dist);

void
lialg_camera_move (LIAlgCamera* self,
                   float        value);

int
lialg_camera_project (LIAlgCamera*       self,
                      const LIMatVector* object,
                      LIMatVector*       window);

void
lialg_camera_tilt (LIAlgCamera* self,
                   float        value);

void
lialg_camera_turn (LIAlgCamera* self,
                   float        value);

int
lialg_camera_unproject (LIAlgCamera*       self,
                        const LIMatVector* window,
                        LIMatVector*       object);

void
lialg_camera_update (LIAlgCamera* self,
                     float        secs);

void
lialg_camera_warp (LIAlgCamera* self);

void
lialg_camera_zoom (LIAlgCamera* self,
                   float        value);

void
lialg_camera_get_bounds (const LIAlgCamera* self,
                         LIMatAabb*         aabb);

void
lialg_camera_get_center (LIAlgCamera*    self,
                         LIMatTransform* result);

void
lialg_camera_set_center (LIAlgCamera*          self,
                         const LIMatTransform* value);

LIAlgCameraDriver
lialg_camera_get_driver (LIAlgCamera* self);

void
lialg_camera_set_driver (LIAlgCamera*      self,
                         LIAlgCameraDriver value);

void
lialg_camera_set_far (LIAlgCamera* self,
                      float        value);

void
lialg_camera_get_frustum (const LIAlgCamera* self,
                          LIMatFrustum*      result);

void
lialg_camera_get_modelview (const LIAlgCamera* self,
                            LIMatMatrix*       value);

void
lialg_camera_set_near (LIAlgCamera* self,
                       float        value);

void
lialg_camera_get_projection (const LIAlgCamera* self,
                             LIMatMatrix*       value);

void
lialg_camera_set_projection (LIAlgCamera* self,
                             float        fov,
                             float        aspect,
                             float        nearplane,
                             float        farplane);

void
lialg_camera_get_transform (const LIAlgCamera* self,
                            LIMatTransform*    value);

void
lialg_camera_set_transform (LIAlgCamera*          self,
                            const LIMatTransform* value);

void
lialg_camera_get_up (const LIAlgCamera* self,
                     LIMatVector*       result);

void
lialg_camera_set_viewport (LIAlgCamera* self,
                           int          x,
                           int          y,
                           int          width,
                           int          height);

#endif

/** @} */
/** @} */
