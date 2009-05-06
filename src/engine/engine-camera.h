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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengCamera Camera
 * @{
 */

#ifndef __ENGINE_CAMERA_H__
#define __ENGINE_CAMERA_H__

#include <math/lips-math.h>
#include <physics/lips-physics.h>
#include "engine.h"
#include "engine-types.h"

typedef enum _liengCameraDriver liengCameraDriver;
enum _liengCameraDriver
{
	LIENG_CAMERA_DRIVER_FIRSTPERSON,
	LIENG_CAMERA_DRIVER_THIRDPERSON,
	LIENG_CAMERA_DRIVER_MANUAL
};

struct _liengCamera
{
	liengEngine* engine;
	liengObject* object;
	liphyShape* shape;
	struct
	{
		int clip;
		float distance;
		liengCameraDriver driver;
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
		int viewport[4];
		limatMatrix modelview;
		limatMatrix projection;
		float aspect;
		float fov;
		float near;
		float far;
	} view;
};

liengCamera*
lieng_camera_new (liengEngine* engine);

void
lieng_camera_free (liengCamera* self);

void
lieng_camera_move (liengCamera* self,
                   float        value);

int
lieng_camera_project (liengCamera*       self,
                      const limatVector* object,
                      limatVector*       window);

void
lieng_camera_tilt (liengCamera* self,
                   float        value);

void
lieng_camera_turn (liengCamera* self,
                   float        value);

int
lieng_camera_unproject (liengCamera*       self,
                        const limatVector* window,
                        limatVector*       object);

void
lieng_camera_update (liengCamera* self,
                     float        secs);

void
lieng_camera_warp (liengCamera* self);

void
lieng_camera_zoom (liengCamera* self,
                   float        value);

void
lieng_camera_get_bounds (const liengCamera* self,
                         limatAabb*         aabb);

void
lieng_camera_set_center (liengCamera*          self,
                         const limatTransform* value);

int
lieng_camera_get_clip (const liengCamera* self);

void
lieng_camera_set_clip (liengCamera* self,
                       int          clip);

liengCameraDriver
lieng_camera_get_driver (liengCamera* self);

void
lieng_camera_set_driver (liengCamera*      self,
                         liengCameraDriver value);

void
lieng_camera_get_frustum (const liengCamera* self,
                          limatFrustum*      result);

void
lieng_camera_get_modelview (const liengCamera* self,
                            limatMatrix*       value);

void
lieng_camera_get_projection (const liengCamera* self,
                             limatMatrix*       value);

void
lieng_camera_set_projection (liengCamera* self,
                             float        fov,
                             float        aspect,
                             float        near,
                             float        far);

void
lieng_camera_get_transform (const liengCamera* self,
                            limatTransform*    value);

void
lieng_camera_set_transform (liengCamera*          self,
                            const limatTransform* value);

void
lieng_camera_get_up (const liengCamera* self,
                     limatVector*       result);

void
lieng_camera_set_viewport (liengCamera* self,
                           int          x,
                           int          y,
                           int          width,
                           int          height);

#endif

/** @} */
/** @} */
