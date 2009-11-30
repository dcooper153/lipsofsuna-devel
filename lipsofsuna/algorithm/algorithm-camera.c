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
 * \addtogroup lialgCamera Camera
 * @{
 */

#include <system/lips-system.h>
#include "algorithm-camera.h"

#define LIALG_CAMERA_DEFAULT_FOV (M_PI / 5.0f)
#define LIALG_CAMERA_DEFAULT_NEAR 1.0f
#define LIALG_CAMERA_DEFAULT_FAR 75.0f
#define LIALG_CAMERA_DEFAULT_ROTATION 8.0f
#define LIALG_CAMERA_DEFAULT_ZOOM 14.0f
#define LIALG_CAMERA_INTERPOLATION_C 1.0f
#define LIALG_CAMERA_INTERPOLATION_N -8.0f
#define LIALG_CAMERA_INTERPOLATION_WARP 50.0f
#define LIALG_CAMERA_MINIMUM_ZOOM 1.5f
#define LIALG_CAMERA_MAXIMUM_ZOOM 100.0f
#define LIALG_CAMERA_ROTATION_EPSILON 0.001f
#define LIALG_CAMERA_SENSITIVITY_ZOOM 1.0f

static void
private_update_1st_person (lialgCamera* self,
                           float        secs);

static void
private_update_3rd_person (lialgCamera* self,
                           float        secs);

static void
private_update_modelview (lialgCamera* self);

static void
private_update_orientation (lialgCamera* self,
                            float        secs);

static void
private_update_projection (lialgCamera* self);

/*****************************************************************************/

/**
 * \brief Creates a new camera.
 *
 * \return New camera or NULL.
 */
lialgCamera*
lialg_camera_new ()
{
	lialgCamera* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lialgCamera));
	if (self == NULL)
		return NULL;
	self->config.driver = LIALG_CAMERA_FIRSTPERSON;
	self->config.distance = LIALG_CAMERA_DEFAULT_ZOOM;
	self->transform.center = limat_transform_identity ();
	self->transform.current = limat_transform_identity ();
	self->transform.local = limat_transform_identity ();
	self->transform.target = limat_transform_identity ();
	self->transform.inverse = limat_transform_identity ();
	self->view.fov = LIALG_CAMERA_DEFAULT_FOV;
	self->view.near = LIALG_CAMERA_DEFAULT_NEAR;
	self->view.far = LIALG_CAMERA_DEFAULT_FAR;
	self->view.aspect = 1.0f;

	private_update_modelview (self);
	private_update_projection (self);

	return self;
}

/**
 * \brief Frees the camera.
 *
 * \param self Camera.
 */
void
lialg_camera_free (lialgCamera* self)
{
	lisys_free (self);
}

/**
 * \brief Moves the camera by the specified amount.
 *
 * \param self Camera.
 * \param value Movement amount.
 */
void
lialg_camera_move (lialgCamera* self,
                   float        value)
{
	limatVector dir;
	limatVector src;
	limatVector dst;
	limatQuaternion rot;

	/* Calculate eye position. */
	rot = limat_quaternion_conjugate (self->transform.current.rotation);
	dir = limat_quaternion_get_basis (rot, 2);
	src = self->transform.current.position;
	dst = limat_vector_add (src, limat_vector_multiply (dir, -value));
	self->transform.target.position = dst;
	private_update_modelview (self);
}

/**
 * \brief Projects a point to the viewport plane of the camera.
 *
 * \param self Camera.
 * \param object Point in object space.
 * \param window Return location for a point in window space.
 * \return Nonzero on success.
 */
int
lialg_camera_project (lialgCamera*       self,
                      const limatVector* object,
                      limatVector*       window)
{
	return limat_matrix_project (
		self->view.projection, self->view.modelview,
		self->view.viewport, object, window);
}

/**
 * \brief Tilts the camera by the specified amount.
 *
 * \param self Camera.
 * \param value Rotation in radians.
 */
void
lialg_camera_tilt (lialgCamera* self,
                   float        value)
{
	limatQuaternion rot;
	limatTransform transform;
	limatVector axis;

	axis = limat_vector_init (1.0f, 0.0f, 0.0f);
	rot = limat_quaternion_rotation (value, axis);
	if (self->config.driver == LIALG_CAMERA_MANUAL)
	{
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.target, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.target = transform;
	}
	else
	{
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.local, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.local = transform;
	}
	private_update_modelview (self);
}

/**
 * \brief Turns the camera by the specified amount.
 *
 * \param self Camera.
 * \param value Rotation in radians.
 */
void
lialg_camera_turn (lialgCamera* self,
                   float        value)
{
	limatQuaternion rot;
	limatTransform transform;
	limatVector axis;

	if (self->config.driver == LIALG_CAMERA_MANUAL)
	{
		rot = limat_quaternion_conjugate (self->transform.target.rotation);
		axis = limat_vector_init (0.0f, 1.0f, 0.0f);
		axis = limat_quaternion_transform (rot, axis);
		rot = limat_quaternion_rotation (value, axis);
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.target, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.target = transform;
	}
	else
	{
		rot = limat_quaternion_conjugate (self->transform.local.rotation);
		axis = limat_vector_init (0.0f, 1.0f, 0.0f);
		axis = limat_quaternion_transform (rot, axis);
		rot = limat_quaternion_rotation (value, axis);
		transform = limat_convert_quaternion_to_transform (rot);
		transform = limat_transform_multiply (self->transform.local, transform);
		transform.rotation = limat_quaternion_normalize (transform.rotation);
		self->transform.local = transform;
	}
	private_update_modelview (self);
}

/**
 * \brief Projects a point on the viewport plane to the scene.
 *
 * \param self Camera.
 * \param object Point in viewport.
 * \param window Return location for a point in world space.
 * \return Nonzero on success.
 */
int
lialg_camera_unproject (lialgCamera*       self,
                        const limatVector* window,
                        limatVector*       object)
{
	return limat_matrix_unproject (
		self->view.projection, self->view.modelview,
		self->view.viewport, window, object);
}

/**
 * \brief Updates the position of the camera.
 *
 * \param self Camera.
 * \param secs Number of seconds since the last update.
 */
void
lialg_camera_update (lialgCamera* self,
                     float        secs)
{
	switch (self->config.driver)
	{
		case LIALG_CAMERA_FIRSTPERSON:
			private_update_1st_person (self, secs);
			break;
		case LIALG_CAMERA_THIRDPERSON:
			private_update_3rd_person (self, secs);
			break;
		default:
			private_update_orientation (self, secs);
			private_update_modelview (self);
			break;
	}
}

/**
 * \brief Warps the camera to the target position.
 *
 * \param self Camera.
 */
void
lialg_camera_warp (lialgCamera* self)
{
	switch (self->config.driver)
	{
		case LIALG_CAMERA_FIRSTPERSON:
			private_update_1st_person (self, 1.0f);
			break;
		case LIALG_CAMERA_THIRDPERSON:
			private_update_3rd_person (self, 1.0f);
			break;
		default:
			break;
	}
	self->transform.current = self->transform.target;
	private_update_orientation (self, 1.0f);
	private_update_modelview (self);
}

/**
 * \brief Zooms in or out.
 *
 * \param self Camera.
 * \param value Amount and direction of zoom.
 */
void
lialg_camera_zoom (lialgCamera* self,
                   float        value)
{
	self->config.distance += value * LIALG_CAMERA_SENSITIVITY_ZOOM;
	if (self->config.distance < LIALG_CAMERA_MINIMUM_ZOOM)
		self->config.distance = LIALG_CAMERA_MINIMUM_ZOOM;
	if (self->config.distance > LIALG_CAMERA_MAXIMUM_ZOOM)
		self->config.distance = LIALG_CAMERA_MAXIMUM_ZOOM;
	private_update_modelview (self);
}

/**
 * \brief Gets the size of the camera.
 *
 * \param self Camera.
 * \param aabb Return location for the bounding box.
 */
void
lialg_camera_get_bounds (const lialgCamera* self,
                         limatAabb*         aabb)
{
	float max;
	float top;
	float right;
	float near;
	limatVector size;
	limatVector zero;

	near = self->view.near;
	top = tan (self->view.fov * M_PI / 360.0f) * near;
	right = top * self->view.aspect;

	max = 1.7f * LI_MAX (LI_MAX (top, right), near);
	size = limat_vector_init (max, max, max);
	zero = limat_vector_init (0.0f, 0.0f, 0.0f);
	limat_aabb_init_from_center (aabb, &zero, &size);
}

/**
 * \brief Sets the point of interest for automatic camera modes.
 *
 * \param self Camera.
 * \param value Center transformation.
 */
void
lialg_camera_set_center (lialgCamera*          self,
                         const limatTransform* value)
{
	self->transform.center = *value;
}

/**
 * \brief Gets the driver type of the camera.
 *
 * \param self Camera.
 * \return Camera driver type.
 */
lialgCameraDriver
lialg_camera_get_driver (lialgCamera* self)
{
	return self->config.driver;
}

/**
 * \brief Sets the driver type of the camera.
 *
 * \param self Camera.
 * \param value Camera driver type.
 */
void
lialg_camera_set_driver (lialgCamera*      self,
                         lialgCameraDriver value)
{
	self->config.driver = value;
}

/**
 * \brief Sets the far plane of the camera.
 *
 * \param self Camera.
 * \param value Far plane distance.
 */
void
lialg_camera_set_far (lialgCamera* self,
                      float        value)
{
	self->view.far = value;
	private_update_projection (self);
}

/**
 * \brief Gets the frustum of the camera.
 *
 * \param self Camera.
 * \param result Return location for frustum.
 */
void
lialg_camera_get_frustum (const lialgCamera* self,
                          limatFrustum*      result)
{
	limat_frustum_init (result,
		&self->view.modelview,
		&self->view.projection);
}

/**
 * \brief Gets the modelview matrix of the camera.
 *
 * \param self Camera.
 * \param value Return location for the matrix.
 */
void
lialg_camera_get_modelview (const lialgCamera* self,
                            limatMatrix*       value)
{
	*value = self->view.modelview;
}

/**
 * \brief Sets the near plane of the camera.
 *
 * \param self Camera.
 * \param value Near plane distance.
 */
void
lialg_camera_set_near (lialgCamera* self,
                       float        value)
{
	self->view.near = value;
	private_update_projection (self);
}

/**
 * \brief Gets the projection matrix of the camera.
 *
 * \param self Camera.
 * \param value Return location for the matrix.
 */
void
lialg_camera_get_projection (const lialgCamera* self,
                             limatMatrix*          value)
{
	*value = self->view.projection;
}

/**
 * \brief Sets the projection settings of the camera.
 *
 * \param self Camera.
 * \param fov Field of view.
 * \param aspect Ascpect ratio of the viewport.
 * \param near Near plane distance.
 * \param far Far plane distance.
 */
void
lialg_camera_set_projection (lialgCamera* self,
                             float        fov,
                             float        aspect,
                             float        near,
                             float        far)
{
	self->view.fov = fov;
	self->view.aspect = aspect;
	self->view.near = near;
	self->view.far = far;
	private_update_projection (self);
}

/**
 * \brief Gets the current transformation of the camera.
 *
 * \param self Camera.
 * \param value Return location for the transformation.
 */
void
lialg_camera_get_transform (const lialgCamera* self,
                            limatTransform*    value)
{
	*value = limat_transform_init (
		self->transform.current.position,
		self->transform.current.rotation);
}

/**
 * \brief Sets the target transformation of the camera.
 *
 * \param self Camera.
 * \param value Transformation.
 */
void
lialg_camera_set_transform (lialgCamera*          self,
                            const limatTransform* value)
{
	self->transform.target = *value;
	private_update_modelview (self);
}

/**
 * \brief Gets the up vector of the camera.
 *
 * \param self Camera.
 * \param result Return location for the up vector.
 */
void
lialg_camera_get_up (const lialgCamera* self,
                     limatVector*       result)
{
	result->x = self->view.modelview.m[1];
	result->y = self->view.modelview.m[4];
	result->z = self->view.modelview.m[9];
}

/**
 * \brief Sets the viewport of the camera.
 *
 * \param self Camera.
 * \param x Left border of the viewport.
 * \param y Bottom border of the viewport.
 * \param width Width of the viewport.
 * \param height Height of the viewport.
 */
void
lialg_camera_set_viewport (lialgCamera* self,
                           int          x,
                           int          y,
                           int          width,
                           int          height)
{
	self->view.viewport[0] = x;
	self->view.viewport[1] = y;
	self->view.viewport[2] = width;
	self->view.viewport[3] = height;
	self->view.aspect = (float) width / height;
	private_update_projection (self);
}

/*****************************************************************************/

static void
private_update_1st_person (lialgCamera* self,
                           float        secs)
{
	/* Copy center position and rotation. */
	self->transform.target = self->transform.center;
	self->transform.current.position = self->transform.target.position;

	/* Apply local rotation. */
	self->transform.target = limat_transform_multiply (self->transform.target, self->transform.local);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);

	/* Standard updates. */
	private_update_orientation (self, secs);
	private_update_modelview (self);
}

static void
private_update_3rd_person (lialgCamera* self,
                           float        secs)
{
	limatTransform transform;

	/* Copy center position and rotation. */
	self->transform.target = self->transform.center;

	/* Apply local rotation. */
	self->transform.target = limat_transform_multiply (self->transform.target, self->transform.local);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);

	/* Project backwards. */
	transform = limat_transform_init (
		limat_vector_init (0.0f, 0.0f, self->config.distance),
		limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f));
	self->transform.target = limat_transform_multiply (self->transform.target, transform);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);

	/* Standard updates. */
	private_update_orientation (self, secs);
	private_update_modelview (self);
}

static void
private_update_modelview (lialgCamera* self)
{
	limatTransform t;

	self->transform.current.rotation = limat_quaternion_normalize (self->transform.current.rotation);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);
	t = limat_transform_invert (self->transform.current);
	self->transform.inverse = t;
	self->view.modelview = limat_convert_transform_to_matrix (t);
}

static void
private_update_orientation (lialgCamera* self,
                            float        secs)
{
	float dist;
	limatQuaternion q;
	limatVector dst;
	limatVector src;
	limatVector disp;

	/* Interpolate position. */
	/* d' = Nd  ->  d = Cde^(Nt) */
	dst = self->transform.target.position;
	src = self->transform.current.position;
	disp = limat_vector_subtract (dst, src);
	dist = limat_vector_get_length (disp);
	if (dist < LIALG_CAMERA_INTERPOLATION_WARP)
	{
		disp = limat_vector_normalize (disp);
		dist = dist * LIALG_CAMERA_INTERPOLATION_C * exp (LIALG_CAMERA_INTERPOLATION_N * secs);
		disp = limat_vector_multiply (disp, dist);
		self->transform.current.position = limat_vector_subtract (dst, disp);
	}
	else
		self->transform.current.position = dst;

	/* Interpolate direction. */
	self->transform.target.rotation = limat_quaternion_get_nearest (
		self->transform.target.rotation,
		self->transform.current.rotation);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);
	self->transform.current.rotation = limat_quaternion_nlerp (
		self->transform.target.rotation,
		self->transform.current.rotation,
		LI_MIN (1.0f, LIALG_CAMERA_DEFAULT_ROTATION * secs));
	self->transform.current.rotation = limat_quaternion_normalize (self->transform.current.rotation);

	/* Eliminate fluctuation. */
	q = limat_quaternion_subtract (self->transform.current.rotation, self->transform.target.rotation);
	if (limat_quaternion_get_length (q) < LIALG_CAMERA_ROTATION_EPSILON)
		self->transform.current.rotation = self->transform.target.rotation;
}

static void
private_update_projection (lialgCamera* self)
{
	self->view.projection = limat_matrix_perspective (
		self->view.fov, self->view.aspect,
		self->view.near, self->view.far);
}

/** @} */
/** @} */
