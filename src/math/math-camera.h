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
 * \addtogroup limat Math
 * @{
 * \addtogroup limatCamera Camera
 * @{
 */

#ifndef __MATH_CAMERA_H__
#define __MATH_CAMERA_H__

#include "math-frustum.h"
#include "math-matrix.h"
#include "math-transform.h"
#include "math-vector.h"

typedef int limatCameraDriver;
enum 
{
	LIMAT_CAMERA_FIRSTPERSON,
	LIMAT_CAMERA_THIRDPERSON,
	LIMAT_CAMERA_MANUAL
};

typedef struct _limatCamera limatCamera;
struct _limatCamera
{
	struct
	{
		float distance;
		limatCameraDriver driver;
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

limatCamera*
limat_camera_new ();

void
limat_camera_free (limatCamera* self);

void
limat_camera_move (limatCamera* self,
                   float        value);

int
limat_camera_project (limatCamera*       self,
                      const limatVector* object,
                      limatVector*       window);

void
limat_camera_tilt (limatCamera* self,
                   float        value);

void
limat_camera_turn (limatCamera* self,
                   float        value);

int
limat_camera_unproject (limatCamera*       self,
                        const limatVector* window,
                        limatVector*       object);

void
limat_camera_update (limatCamera* self,
                     float        secs);

void
limat_camera_warp (limatCamera* self);

void
limat_camera_zoom (limatCamera* self,
                   float        value);

void
limat_camera_get_bounds (const limatCamera* self,
                         limatAabb*         aabb);

void
limat_camera_set_center (limatCamera*          self,
                         const limatTransform* value);

limatCameraDriver
limat_camera_get_driver (limatCamera* self);

void
limat_camera_set_driver (limatCamera*      self,
                         limatCameraDriver value);

void
limat_camera_get_frustum (const limatCamera* self,
                          limatFrustum*      result);

void
limat_camera_get_modelview (const limatCamera* self,
                            limatMatrix*       value);

void
limat_camera_get_projection (const limatCamera* self,
                             limatMatrix*       value);

void
limat_camera_set_projection (limatCamera* self,
                             float        fov,
                             float        aspect,
                             float        near,
                             float        far);

void
limat_camera_get_transform (const limatCamera* self,
                            limatTransform*    value);

void
limat_camera_set_transform (limatCamera*          self,
                            const limatTransform* value);

void
limat_camera_get_up (const limatCamera* self,
                     limatVector*       result);

void
limat_camera_set_viewport (limatCamera* self,
                           int          x,
                           int          y,
                           int          width,
                           int          height);

#endif

/** @} */
/** @} */
