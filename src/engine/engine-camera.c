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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine-camera.h"

#define LIENG_CAMERA_DEFAULT_FOV (M_PI / 5.0f)
#define LIENG_CAMERA_DEFAULT_NEAR 0.5f
#define LIENG_CAMERA_DEFAULT_FAR 500.0f
#define LIENG_CAMERA_DEFAULT_ROTATION 8.0f
#define LIENG_CAMERA_DEFAULT_ZOOM 14.0f
#define LIENG_CAMERA_INTERPOLATION_C 1.0f
#define LIENG_CAMERA_INTERPOLATION_N -8.0f
#define LIENG_CAMERA_INTERPOLATION_WARP 50.0f
#define LIENG_CAMERA_MINIMUM_ZOOM 1.5f
#define LIENG_CAMERA_MAXIMUM_ZOOM 100.0f
#define LIENG_CAMERA_ROTATION_EPSILON 0.001f
#define LIENG_CAMERA_SENSITIVITY_ZOOM 1.0f

static void
private_update_1st_person (liengCamera* self,
                           float        secs);

static void
private_update_3rd_person (liengCamera* self,
                           float        secs);

static void
private_update_geometry (liengCamera* self);

static void
private_update_modelview (liengCamera* self);

static void
private_update_orientation (liengCamera* self,
                            float        secs);

static void
private_update_projection (liengCamera* self);

static void
private_sweep (liengCamera*    self,
               const limatVector* src,
               const limatVector* dst,
               limatVector*       result);

/*****************************************************************************/

/**
 * \brief Creates a new camera.
 *
 * \param engine Engine.
 * \return New camera or NULL.
 */
liengCamera*
lieng_camera_new (liengEngine* engine)
{
	liengCamera* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liengCamera));
	if (self == NULL)
		return NULL;
	self->engine = engine;
	self->config.driver = LIENG_CAMERA_DRIVER_FIRSTPERSON;
	self->config.clip = 1;
	self->config.distance = LIENG_CAMERA_DEFAULT_ZOOM;
	self->transform.center = limat_transform_identity ();
	self->transform.current = limat_transform_identity ();
	self->transform.local = limat_transform_identity ();
	self->transform.target = limat_transform_identity ();
	self->transform.inverse = limat_transform_identity ();
	self->view.fov = LIENG_CAMERA_DEFAULT_FOV;
	self->view.near = LIENG_CAMERA_DEFAULT_NEAR;
	self->view.far = LIENG_CAMERA_DEFAULT_FAR;
	self->view.aspect = 1.0f;

	/* Create object. */
	self->object = lieng_object_new (engine, NULL,
		LIPHY_SHAPE_MODE_CONVEX, LIPHY_CONTROL_MODE_RIGID, 0, NULL);
	if (self->object == NULL)
		goto error;
	lieng_object_ref (self->object, 1);

	private_update_geometry (self);
	private_update_modelview (self);
	private_update_projection (self);

	return self;

error:
	lieng_camera_free (self);
	return NULL;
}

/**
 * \brief Frees the camera.
 *
 * \param self Camera.
 */
void
lieng_camera_free (liengCamera* self)
{
	if (self->object != NULL)
		lieng_object_free (self->object);
	if (self->shape != NULL)
		liphy_shape_free (self->shape);
	free (self);
}

/**
 * \brief Moves the camera by the specified amount.
 *
 * \param self Camera.
 * \param value Movement amount.
 */
void
lieng_camera_move (liengCamera* self,
                   float        value)
{
	limatVector dir;
	limatVector src;
	limatVector dst;
	limatVector ret;
	limatQuaternion rot;

	/* Calculate eye position. */
	rot = limat_quaternion_conjugate (self->transform.current.rotation);
	dir = limat_quaternion_get_basis (rot, 2);
	src = self->transform.current.position;
	dst = limat_vector_add (src, limat_vector_multiply (dir, -value));

	/* Check for obstacles and set position. */
	private_sweep (self, &src, &dst, &ret);
	self->transform.target.position = ret;
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
lieng_camera_project (liengCamera*       self,
                      const limatVector* object,
                      limatVector*       window)
{
	float w;
	limatMatrix m;
	limatVector v;
	limatVector result;

	/* Multiply by modelview matrix. */
	w = 1.0f;
	v = *object;
	m = self->view.modelview;
	result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * w;
	result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * w;
	result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * w;
	w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * w;

	/* Multiply by projection matrix. */
	v = result;
	m = self->view.projection;
	result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * w;
	result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * w;
	result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * w;
	w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * w;

	/* Convert to window space. */
	if (LI_ABS (w) < LI_MATH_EPSILON)
		return 0;
	result.x /= w;
	result.y /= w;
	result.z /= w;
	window->x = self->view.viewport[0] + self->view.viewport[2] * (result.x + 1.0f) / 2.0f;
	window->y = self->view.viewport[1] + self->view.viewport[3] * (result.y + 1.0f) / 2.0f;
	window->z = (result.z + 1.0f) / 2.0f;
	return 1;
}

/**
 * \brief Tilts the camera by the specified amount.
 *
 * \param self Camera.
 * \param value Rotation in radians.
 */
void
lieng_camera_tilt (liengCamera* self,
                   float        value)
{
	limatQuaternion rot;
	limatTransform transform;
	limatVector axis;

	axis = limat_vector_init (1.0f, 0.0f, 0.0f);
	rot = limat_quaternion_rotation (value, axis);
	transform = limat_convert_quaternion_to_transform (rot);
	transform = limat_transform_multiply (self->transform.local, transform);
	transform.rotation = limat_quaternion_normalize (transform.rotation);
	self->transform.local = transform;
	private_update_modelview (self);
}

/**
 * \brief Turns the camera by the specified amount.
 *
 * \param self Camera.
 * \param value Rotation in radians.
 */
void
lieng_camera_turn (liengCamera* self,
                   float        value)
{
	limatQuaternion rot;
	limatTransform transform;
	limatVector axis;

	rot = limat_quaternion_conjugate (self->transform.local.rotation);
	axis = limat_vector_init (0.0f, 1.0f, 0.0f);
	axis = limat_quaternion_transform (rot, axis);
	rot = limat_quaternion_rotation (value, axis);
	transform = limat_convert_quaternion_to_transform (rot);
	transform = limat_transform_multiply (self->transform.local, transform);
	transform.rotation = limat_quaternion_normalize (transform.rotation);
	self->transform.local = transform;
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
lieng_camera_unproject (liengCamera*       self,
                        const limatVector* window,
                        limatVector*       object)
{
	limatMatrix m;
	limatVector tmp;

	/* Get inverse matrix. */
	m = limat_matrix_multiply (self->view.projection, self->view.modelview);
	if (limat_matrix_get_singular (m))
		return 0;
	m = limat_matrix_invert (m);

	/* Vector to [-1,1] range. */
	tmp.x = 2.0f * (window->x - self->view.viewport[0]) / self->view.viewport[2] - 1.0f;
	tmp.y = 2.0f * (window->y - self->view.viewport[1]) / self->view.viewport[3] - 1.0f;
	tmp.z = 2.0f * window->z - 1.0f;

	/* Multiply by the inverse matrix. */
	*object = limat_matrix_transform (m, tmp);
	return 1;
}

/**
 * \brief Updates the position of the camera.
 *
 * \param self Camera.
 * \param secs Number of seconds since the last update.
 */
void
lieng_camera_update (liengCamera* self,
                     float        secs)
{
	switch (self->config.driver)
	{
		case LIENG_CAMERA_DRIVER_FIRSTPERSON:
			private_update_1st_person (self, secs);
			break;
		case LIENG_CAMERA_DRIVER_THIRDPERSON:
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
lieng_camera_warp (liengCamera* self)
{
	switch (self->config.driver)
	{
		case LIENG_CAMERA_DRIVER_FIRSTPERSON:
			private_update_1st_person (self, 1.0f);
			break;
		case LIENG_CAMERA_DRIVER_THIRDPERSON:
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
lieng_camera_zoom (liengCamera* self,
                   float        value)
{
	self->config.distance += value * LIENG_CAMERA_SENSITIVITY_ZOOM;
	if (self->config.distance < LIENG_CAMERA_MINIMUM_ZOOM)
		self->config.distance = LIENG_CAMERA_MINIMUM_ZOOM;
	if (self->config.distance > LIENG_CAMERA_MAXIMUM_ZOOM)
		self->config.distance = LIENG_CAMERA_MAXIMUM_ZOOM;
	private_update_modelview (self);
}

/**
 * \brief Gets the size of the camera.
 *
 * \param self Camera.
 * \param aabb Return location for the bounding box.
 */
void
lieng_camera_get_bounds (const liengCamera* self,
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

	max = 3.0f * LI_MAX (LI_MAX (top, right), near);
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
lieng_camera_set_center (liengCamera*          self,
                         const limatTransform* value)
{
	self->transform.center = *value;
}

/**
 * \brief Gets the clip mode of the camera.
 *
 * \param self Camera.
 * \return Nonzero if clipping is enabled.
 */
int
lieng_camera_get_clip (const liengCamera* self)
{
	return self->config.clip;
}

/**
 * \brief Sets the clip mode of the camera.
 *
 * \param self Camera.
 * \param clip Nonzero if should clip.
 */
void
lieng_camera_set_clip (liengCamera* self,
                       int          clip)
{
	self->config.clip = clip;
}

/**
 * \brief Gets the driver type of the camera.
 *
 * \param self Camera.
 * \return Camera driver type.
 */
liengCameraDriver
lieng_camera_get_driver (liengCamera* self)
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
lieng_camera_set_driver (liengCamera*      self,
                         liengCameraDriver value)
{
	self->config.driver = value;
}

/**
 * \brief Gets the frustum of the camera.
 *
 * \param self Camera.
 * \param result Return location for frustum.
 */
void
lieng_camera_get_frustum (const liengCamera* self,
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
lieng_camera_get_modelview (const liengCamera* self,
                            limatMatrix*       value)
{
	*value = self->view.modelview;
}

/**
 * \brief Gets the projection matrix of the camera.
 *
 * \param self Camera.
 * \param value Return location for the matrix.
 */
void
lieng_camera_get_projection (const liengCamera* self,
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
lieng_camera_set_projection (liengCamera* self,
                             float        fov,
                             float        aspect,
                             float        near,
                             float        far)
{
	self->view.fov = fov;
	self->view.aspect = aspect;
	self->view.near = near;
	self->view.far = far;
	private_update_geometry (self);
	private_update_projection (self);
}

/**
 * \brief Gets the current transformation of the camera.
 *
 * \param self Camera.
 * \param value Return location for the transformation.
 */
void
lieng_camera_get_transform (const liengCamera* self,
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
lieng_camera_set_transform (liengCamera*          self,
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
lieng_camera_get_up (const liengCamera* self,
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
lieng_camera_set_viewport (liengCamera* self,
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
private_update_1st_person (liengCamera* self,
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
private_update_3rd_person (liengCamera* self,
                           float        secs)
{
	float dist;
	limatVector src;
	limatVector dst;
	limatVector ret;
	limatTransform transform;

	/* Copy center position and rotation. */
	self->transform.target = self->transform.center;

	/* Apply local rotation. */
	self->transform.target = limat_transform_multiply (self->transform.target, self->transform.local);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);

	/* Check for walls. */
	dist = self->config.distance;
	if (self->config.clip)
	{
		src = limat_vector_init (0.0f, 0.0f, 0.1f);
		src = limat_transform_transform (self->transform.target, src);
		dst = limat_vector_init (0.0f, 0.0f, dist);
		dst = limat_transform_transform (self->transform.target, dst);
		private_sweep (self, &src, &dst, &ret);
		dist = limat_vector_get_length (limat_vector_subtract (src, ret));
	}

	/* Project backwards. */
	transform = limat_transform_init (
		limat_vector_init (0.0f, 0.0f, dist),
		limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f));
	self->transform.target = limat_transform_multiply (self->transform.target, transform);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);

	/* Standard updates. */
	private_update_orientation (self, secs);
	private_update_modelview (self);
}

static void
private_update_geometry (liengCamera* self)
{
	limatAabb aabb;
	liphyShape* shape;

	/* FIXME: Could use more accurate shape. */
	lieng_camera_get_bounds (self, &aabb);

	/* FIXME: Should rather modify the shape? */
	shape = liphy_shape_new_aabb (self->engine->physics, &aabb);
	if (shape == NULL)
		return;
	lieng_object_set_shape (self->object, NULL);
	if (self->shape != NULL)
		liphy_shape_free (self->shape);
	lieng_object_set_shape (self->object, shape);
	self->shape = shape;
}

static void
private_update_modelview (liengCamera* self)
{
	limatTransform t;

	self->transform.current.rotation = limat_quaternion_normalize (self->transform.current.rotation);
	self->transform.target.rotation = limat_quaternion_normalize (self->transform.target.rotation);
	t = limat_transform_invert (self->transform.current);
	self->transform.inverse = t;
	self->view.modelview = limat_convert_transform_to_matrix (t);
}

static void
private_update_orientation (liengCamera* self,
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
	if (dist < LIENG_CAMERA_INTERPOLATION_WARP)
	{
		disp = limat_vector_normalize (disp);
		dist = dist * LIENG_CAMERA_INTERPOLATION_C * exp (LIENG_CAMERA_INTERPOLATION_N * secs);
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
		LI_MIN (1.0f, LIENG_CAMERA_DEFAULT_ROTATION * secs));
	self->transform.current.rotation = limat_quaternion_normalize (self->transform.current.rotation);

	/* Eliminate fluctuation. */
	q = limat_quaternion_subtract (self->transform.current.rotation, self->transform.target.rotation);
	if (limat_quaternion_get_length (q) < LIENG_CAMERA_ROTATION_EPSILON)
		self->transform.current.rotation = self->transform.target.rotation;
}

static void
private_update_projection (liengCamera* self)
{
	self->view.projection = limat_matrix_perspective (
		self->view.fov, self->view.aspect,
		self->view.near, self->view.far);
}

static void
private_sweep (liengCamera*       self,
               const limatVector* src,
               const limatVector* dst,
               limatVector*       result)
{
	int x;
	int y;
	int count = 1;
	float frac = 0.0f;
	float frac1;
	limatVector dir;
	limatVector dir1;
	limatVector filter;
	limatVector filter0;
	limatVector filter1;
	limatTransform transform;

	/* Initialize filter. */
	dir = limat_vector_subtract (*dst, *src);
	dir = limat_vector_normalize (dir);
	filter0 = limat_vector_cross (dir, limat_vector_init (1.0f, 0.0f, 0.0f));
	filter1 = limat_vector_cross (dir, limat_vector_init (0.0f, 1.0f, 0.0f));
	if (limat_vector_dot (filter0, filter0) >
	    limat_vector_dot (filter1, filter1))
	{
		filter0 = limat_vector_normalize (filter0);
		filter1 = limat_vector_cross (filter0, dir);
	}
	else
	{
		filter1 = limat_vector_normalize (filter1);
		filter0 = limat_vector_cross (filter1, dir);
	}

	/* Position collision shape. */
	dir = limat_vector_subtract (*dst, *src);
	transform = limat_transform_init (*src, self->transform.current.rotation);
	lieng_object_set_transform (self->object, &transform);

	/* Smoothing sweeps. */
	for (y = -1 ; y <= 1 ; y++)
	{
		for (x = -1 ; x <= 1 ; x++)
		{
			if (!x && !y)
				continue;
			filter = limat_vector_add (
				limat_vector_multiply (filter0, 0.6f * x),
				limat_vector_multiply (filter1, 0.6f * y));
			dir1 = limat_vector_add (dir, filter);
			frac1 = limat_vector_get_length (dir) / limat_vector_get_length (dir1);
			frac1 *= liphy_object_sweep (self->object->physics, &dir1);
			frac += frac1;
			count++;
		}
	}

	/* Main sweep. */
	frac += liphy_object_sweep (self->object->physics, &dir);
	dir = limat_vector_multiply (dir, frac / count);
	*result = limat_vector_add (*src, dir);
}

/** @} */
/** @} */
