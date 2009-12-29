/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lialgCamera Camerai
 * @{
 */

#ifndef __ALGORITHM_CAMERA_H__
#define __ALGORITHM_CAMERA_H__

#include <math/lips-math.h>

typedef int lialgCameraDriver;
enum 
{
	LIALG_CAMERA_FIRSTPERSON,
	LIALG_CAMERA_THIRDPERSON,
	LIALG_CAMERA_MANUAL,
	LIALG_CAMERA_MAX
};

typedef struct _lialgCamera lialgCamera;
struct _lialgCamera
{
	struct
	{
		float distance;
		lialgCameraDriver driver;
	} config;
	struct
	{
		limatTransform center;
		limatTransform current;
		limatTransform local;
		limatTransform target;
		limatTransform inverse;
	} transform;
	struct
	{
		float aspect;
		float fov;
		float near;
		float far;
		int viewport[4];
		limatMatrix modelview;
		limatMatrix projection;
	} view;
};

lialgCamera*
lialg_camera_new ();

void
lialg_camera_free (lialgCamera* self);

void
lialg_camera_clip (lialgCamera* self,
                   float        dist);

void
lialg_camera_move (lialgCamera* self,
                   float        value);

int
lialg_camera_project (lialgCamera*       self,
                      const limatVector* object,
                      limatVector*       window);

void
lialg_camera_tilt (lialgCamera* self,
                   float        value);

void
lialg_camera_turn (lialgCamera* self,
                   float        value);

int
lialg_camera_unproject (lialgCamera*       self,
                        const limatVector* window,
                        limatVector*       object);

void
lialg_camera_update (lialgCamera* self,
                     float        secs);

void
lialg_camera_warp (lialgCamera* self);

void
lialg_camera_zoom (lialgCamera* self,
                   float        value);

void
lialg_camera_get_bounds (const lialgCamera* self,
                         limatAabb*         aabb);

void
lialg_camera_get_center (lialgCamera*    self,
                         limatTransform* result);

void
lialg_camera_set_center (lialgCamera*          self,
                         const limatTransform* value);

lialgCameraDriver
lialg_camera_get_driver (lialgCamera* self);

void
lialg_camera_set_driver (lialgCamera*      self,
                         lialgCameraDriver value);

void
lialg_camera_set_far (lialgCamera* self,
                      float        value);

void
lialg_camera_get_frustum (const lialgCamera* self,
                          limatFrustum*      result);

void
lialg_camera_get_modelview (const lialgCamera* self,
                            limatMatrix*       value);

void
lialg_camera_set_near (lialgCamera* self,
                       float        value);

void
lialg_camera_get_projection (const lialgCamera* self,
                             limatMatrix*       value);

void
lialg_camera_set_projection (lialgCamera* self,
                             float        fov,
                             float        aspect,
                             float        near,
                             float        far);

void
lialg_camera_get_transform (const lialgCamera* self,
                            limatTransform*    value);

void
lialg_camera_set_transform (lialgCamera*          self,
                            const limatTransform* value);

void
lialg_camera_get_up (const lialgCamera* self,
                     limatVector*       result);

void
lialg_camera_set_viewport (lialgCamera* self,
                           int          x,
                           int          y,
                           int          width,
                           int          height);

#endif

/** @} */
/** @} */
