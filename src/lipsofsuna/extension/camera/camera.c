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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtCamera Camera
 * @{
 */

#include "lipsofsuna/system.h"
#include "lipsofsuna/extension/physics/physics.h"
#include "camera.h"

#define LIALG_CAMERA_DEFAULT_FOV (M_PI / 5.0f)
#define LIALG_CAMERA_DEFAULT_NEAR 1.0f
#define LIALG_CAMERA_DEFAULT_FAR 75.0f
#define LIALG_CAMERA_DEFAULT_ZOOM 14.0f
#define LIALG_CAMERA_INTERPOLATION_WARP 50.0f
#define LIALG_CAMERA_MINIMUM_ZOOM 1.5f
#define LIALG_CAMERA_MAXIMUM_ZOOM 100.0f
#define LIALG_CAMERA_SENSITIVITY_ZOOM 1.0f
#define LIALG_CAMERA_SMOOTHING_TIMESTEP (1.0f / 60.0f)

static void private_update_1st_person (
	LIExtCamera* self);

static void private_update_3rd_person (
	LIExtCamera* self,
	float        dist);

static void private_update_modelview (
	LIExtCamera* self);

static void private_update_orientation (
	LIExtCamera* self,
	float        secs);

static void private_update_projection (
	LIExtCamera* self);

/*****************************************************************************/

/**
 * \brief Creates a new camera.
 * \param module Camera module.
 * \return New camera or NULL.
 */
LIExtCamera* liext_camera_new (
	LIExtModule* module)
{
	LIExtCamera* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtCamera));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->config.collision_group = 0xFFFF;
	self->config.collision_mask = 0xFFFF;
	self->config.driver = LIALG_CAMERA_FIRSTPERSON;
	self->config.distance = LIALG_CAMERA_DEFAULT_ZOOM;
	self->smoothing.pos = 0.5f;
	self->smoothing.rot = 0.5f;
	self->transform.center = limat_transform_identity ();
	self->transform.current = limat_transform_identity ();
	self->transform.local = limat_transform_identity ();
	self->transform.target = limat_transform_identity ();
	self->transform.inverse = limat_transform_identity ();
	self->view.fov = LIALG_CAMERA_DEFAULT_FOV;
	self->view.nearplane = LIALG_CAMERA_DEFAULT_NEAR;
	self->view.farplane = LIALG_CAMERA_DEFAULT_FAR;
	self->view.aspect = 1.0f;

	private_update_modelview (self);
	private_update_projection (self);

	return self;
}

/**
 * \brief Frees the camera.
 * \param self Camera.
 */
void liext_camera_free (
	LIExtCamera* self)
{
	lisys_free (self);
}

/**
 * \brief Calculates the world space transformation for the first person driver.
 * \param self Camera.
 * \param result Return location for the transformation.
 */
void liext_camera_calculate_1st_person_transform (
	LIExtCamera*    self,
	LIMatTransform* result)
{
	LIMatTransform target;

	target = limat_transform_multiply (self->transform.center, self->transform.local);
	target.rotation = limat_quaternion_normalize (target.rotation);
	*result = target;
}

/**
 * \brief Calculates the 3rd person camera distance after collisions.
 * \param self Camera.
 * \param center Center position and rotation.
 * \param radius Camera object radius.
 * \param distance Distance from the center.
 * \param collision_group Collision group.
 * \param collision_mask Collision mask.
 * \return Clipped distance.
 */
float liext_camera_calculate_3rd_person_clipped_distance (
	LIExtCamera*          self,
	const LIMatTransform* center,
	float                 distance,
	float                 radius,
	int                   collision_group,
	int                   collision_mask)
{
	int hit;
	float frac;
	LIMatTransform center1;
	LIMatTransform target;
	LIPhyContact tmp;
	LIPhyPhysics* physics;

	/* Find the physics manager. */
	physics = limai_program_find_component (self->module->program, "physics");
	if (physics == NULL)
		return distance;

	/* Apply the local transformation. */
	center1 = limat_transform_multiply (*center, self->transform.local);
	center1.rotation = limat_quaternion_normalize (center->rotation);

	/* Calculate the unclipped target transformation. */
	liext_camera_calculate_3rd_person_transform (self, center, distance, &target);

	/* Find the clip distance with a ray/sphere cast. */
	if (radius >= 0.001f)
	{
		hit = liphy_physics_cast_sphere (physics, &center1.position, &target.position,
			radius, collision_group, collision_mask, NULL, 0, &tmp);
	}
	else
	{
		hit = liphy_physics_cast_ray (physics, &center1.position, &target.position,
			collision_group, collision_mask, NULL, 0, &tmp);
	}

	/* Return the clip distance. */
	if (hit)
		return distance * tmp.fraction;
	else
		return distance;

	return distance * frac;
}

/**
 * \brief Calculates the world space transformation for the third person driver.
 * \param self Camera.
 * \param center Center position and rotation.
 * \param distance Distance from the center.
 * \param result Return location for the transformation.
 */
void liext_camera_calculate_3rd_person_transform (
	LIExtCamera*          self,
	const LIMatTransform* center,
	float                 distance,
	LIMatTransform*       result)
{
	LIMatTransform center1;
	LIMatTransform project;
	LIMatTransform target;

	/* Apply the local transformation. */
	center1 = limat_transform_multiply (*center, self->transform.local);
	center1.rotation = limat_quaternion_normalize (center->rotation);

	/* Project the camera backwards from the target. */
	project = limat_transform_init (
		limat_vector_init (0.0f, 0.0f, distance),
		limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f));
	target = limat_transform_multiply (center1, project);
	target.rotation = limat_quaternion_normalize (target.rotation);

	/* Set the target position. */
	*result = target;
}

/**
 * \brief Calculates the transformation after smoothing.
 * \param self Camera.
 * \param target Target transformation.
 * \param position_smoothing Position smoothing factor.
 * \param rotation_smoothing Rotation smoothing factor.
 * \param result Return location for the transformation.
 */
void liext_camera_calculate_smoothed_transform (
	LIExtCamera*          self,
	const LIMatTransform* target,
	float                 position_smoothing,
	float                 rotation_smoothing,
	LIMatTransform*       result)
{
	LIMatTransform src;
	LIMatTransform dst;

	/* Get the source and destination transformations. */
	dst = *target;
	src = self->transform.current;
	src.rotation = limat_quaternion_get_nearest (src.rotation, dst.rotation);

	/* Calculate the interpolated transformation. */
	result->position = limat_vector_lerp (dst.position, src.position, position_smoothing);
	result->rotation = limat_quaternion_nlerp (dst.rotation, src.rotation, rotation_smoothing);
}

/**
 * \brief Moves the camera by the specified amount.
 * \param self Camera.
 * \param value Movement amount.
 */
void liext_camera_move (
	LIExtCamera* self,
	float        value)
{
	LIMatVector dir;
	LIMatVector src;
	LIMatVector dst;
	LIMatQuaternion rot;

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
 * \param self Camera.
 * \param object Point in object space.
 * \param window Return location for a point in window space.
 * \return Nonzero on success.
 */
int liext_camera_project (
	LIExtCamera*       self,
	const LIMatVector* object,
	LIMatVector*       window)
{
	return limat_matrix_project (
		self->view.projection, self->view.modelview,
		self->view.viewport, object, window);
}

/**
 * \brief Tilts the camera by the specified amount.
 * \param self Camera.
 * \param value Rotation in radians.
 */
void liext_camera_tilt (
	LIExtCamera* self,
	float        value)
{
	LIMatQuaternion rot;
	LIMatTransform transform;
	LIMatVector axis;

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
 * \param self Camera.
 * \param value Rotation in radians.
 */
void liext_camera_turn (
	LIExtCamera* self,
	float        value)
{
	LIMatQuaternion rot;
	LIMatTransform transform;
	LIMatVector axis;

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
 * \param self Camera.
 * \param object Point in viewport.
 * \param window Return location for a point in world space.
 * \return Nonzero on success.
 */
int liext_camera_unproject (
	LIExtCamera*       self,
	const LIMatVector* window,
	LIMatVector*       object)
{
	return limat_matrix_unproject (
		self->view.projection, self->view.modelview,
		self->view.viewport, window, object);
}

/**
 * \brief Updates the position of the camera.
 * \param self Camera.
 * \param secs Number of seconds since the last update.
 */
void liext_camera_update (
	LIExtCamera* self,
	float        secs)
{
	switch (self->config.driver)
	{
		case LIALG_CAMERA_FIRSTPERSON:
			private_update_1st_person (self);
			private_update_orientation (self, secs);
			private_update_modelview (self);
			break;
		case LIALG_CAMERA_THIRDPERSON:
			private_update_3rd_person (self, self->config.distance);
			private_update_orientation (self, secs);
			private_update_modelview (self);
			break;
		default:
			private_update_orientation (self, secs);
			private_update_modelview (self);
			break;
	}
}

/**
 * \brief Warps the camera to the target position.
 * \param self Camera.
 */
void liext_camera_warp (
	LIExtCamera* self)
{
	switch (self->config.driver)
	{
		case LIALG_CAMERA_FIRSTPERSON:
			private_update_1st_person (self);
			break;
		case LIALG_CAMERA_THIRDPERSON:
			private_update_3rd_person (self, self->config.distance);
			break;
		default:
			break;
	}
	self->transform.current = self->transform.target;
	private_update_modelview (self);
}

/**
 * \brief Zooms in or out.
 * \param self Camera.
 * \param value Amount and direction of zoom.
 */
void liext_camera_zoom (
	LIExtCamera* self,
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
 * \param self Camera.
 * \param aabb Return location for the bounding box.
 */
void liext_camera_get_bounds (
	const LIExtCamera* self,
	LIMatAabb*         aabb)
{
	float max;
	float top;
	float right;
	float nearplane;
	LIMatVector size;
	LIMatVector zero;

	nearplane = self->view.nearplane;
	top = tan (self->view.fov * M_PI / 360.0f) * nearplane;
	right = top * self->view.aspect;

	max = 1.7f * LIMAT_MAX (LIMAT_MAX (top, right), nearplane);
	size = limat_vector_init (max, max, max);
	zero = limat_vector_init (0.0f, 0.0f, 0.0f);
	limat_aabb_init_from_center (aabb, &zero, &size);
}

/**
 * \brief Gets the point of interest for automatic camera modes.
 * \param self Camera.
 * \param result Return location for transformation.
 */
void liext_camera_get_center (
	LIExtCamera*    self,
	LIMatTransform* result)
{
	*result = self->transform.center;
}

/**
 * \brief Sets the point of interest for automatic camera modes.
 * \param self Camera.
 * \param value Center transformation.
 */
void liext_camera_set_center (
	LIExtCamera*          self,
	const LIMatTransform* value)
{
	self->transform.center = *value;
}

/**
 * \brief Gets the driver type of the camera.
 * \param self Camera.
 * \return Camera driver type.
 */
LIExtCameraDriver liext_camera_get_driver (
	LIExtCamera* self)
{
	return self->config.driver;
}

/**
 * \brief Sets the driver type of the camera.
 * \param self Camera.
 * \param value Camera driver type.
 */
void liext_camera_set_driver (
	LIExtCamera*      self,
	LIExtCameraDriver value)
{
	self->config.driver = value;
}

/**
 * \brief Sets the far plane of the camera.
 * \param self Camera.
 * \param value Far plane distance.
 */
void liext_camera_set_far (
	LIExtCamera* self,
	float        value)
{
	self->view.farplane = value;
	private_update_projection (self);
}

/**
 * \brief Gets the field of view of the camera.
 * \param self Camera.
 * \return Field of view in radians.
 */
float liext_camera_get_fov (
	const LIExtCamera* self)
{
	return self->view.fov;
}

/**
 * \brief Sets the field of view of the camera.
 * \param self Camera.
 * \param value Field of view in radians.
 */
void liext_camera_set_fov (
	LIExtCamera* self,
	float        value)
{
	self->view.fov = value;
	private_update_projection (self);
}

/**
 * \brief Gets the frustum of the camera.
 * \param self Camera.
 * \param result Return location for frustum.
 */
void liext_camera_get_frustum (
	const LIExtCamera* self,
	LIMatFrustum*      result)
{
	limat_frustum_init (result,
		&self->view.modelview,
		&self->view.projection);
}

/**
 * \brief Gets the modelview matrix of the camera.
 * \param self Camera.
 * \param value Return location for the matrix.
 */
void liext_camera_get_modelview (
	const LIExtCamera* self,
	LIMatMatrix*       value)
{
	*value = self->view.modelview;
}

/**
 * \brief Sets the near plane of the camera.
 * \param self Camera.
 * \param value Near plane distance.
 */
void liext_camera_set_near (
	LIExtCamera* self,
	float        value)
{
	self->view.nearplane = value;
	private_update_projection (self);
}

/**
 * \brief Gets the projection matrix of the camera.
 * \param self Camera.
 * \param value Return location for the matrix.
 */
void liext_camera_get_projection (
	const LIExtCamera* self,
	LIMatMatrix*          value)
{
	*value = self->view.projection;
}

/**
 * \brief Sets the projection settings of the camera.
 * \param self Camera.
 * \param fov Field of view.
 * \param aspect Ascpect ratio of the viewport.
 * \param nearplane Near plane distance.
 * \param farplane Far plane distance.
 */
void liext_camera_set_projection (
	LIExtCamera* self,
	float        fov,
	float        aspect,
	float        nearplane,
	float        farplane)
{
	self->view.fov = fov;
	self->view.aspect = aspect;
	self->view.nearplane = nearplane;
	self->view.farplane = farplane;
	private_update_projection (self);
}

/**
 * \brief Gets the smoothing rates of the camera.
 * \param self Camera.
 * \param pos Return location for the position smoothing rate.
 * \param rot Return location for the rotation smoothing rate.
 */
void liext_camera_get_smoothing (
	const LIExtCamera* self,
	float*             pos,
	float*             rot)
{
	*pos = self->smoothing.pos;
	*rot = self->smoothing.rot;
}

/**
 * \brief Sets the smoothing rates of the camera.
 * \param self Camera.
 * \param pos Position smoothing rate.
 * \param rot Rotation smoothing rate.
 */
void liext_camera_set_smoothing (
	LIExtCamera* self,
	float        pos,
	float        rot)
{
	self->smoothing.pos = pos;
	self->smoothing.rot = rot;
}

/**
 * \brief Gets the current transformation of the camera.
 *
 * \param self Camera.
 * \param value Return location for the transformation.
 */
void liext_camera_get_transform (
	const LIExtCamera* self,
	LIMatTransform*    value)
{
	*value = limat_transform_init (
		self->transform.current.position,
		self->transform.current.rotation);
}

/**
 * \brief Sets the current transformation of the camera.
 * \param self Camera.
 * \param value Transformation.
 */
void liext_camera_set_transform (
	LIExtCamera*          self,
	const LIMatTransform* value)
{
	self->transform.target = *value;
	self->transform.current = *value;
	private_update_modelview (self);
}

/**
 * \brief Sets the target transformation of the camera.
 * \param self Camera.
 * \param value Transformation.
 */
void liext_camera_set_target_transform (
	LIExtCamera*          self,
	const LIMatTransform* value)
{
	self->transform.target = *value;
}

/**
 * \brief Gets the up vector of the camera.
 * \param self Camera.
 * \param result Return location for the up vector.
 */
void liext_camera_get_up (
	const LIExtCamera* self,
	LIMatVector*       result)
{
	result->x = self->view.modelview.m[1];
	result->y = self->view.modelview.m[4];
	result->z = self->view.modelview.m[9];
}

/**
 * \brief Sets the viewport of the camera.
 * \param self Camera.
 * \param x Left border of the viewport.
 * \param y Bottom border of the viewport.
 * \param width Width of the viewport.
 * \param height Height of the viewport.
 */
void liext_camera_set_viewport (
	LIExtCamera* self,
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

static void private_update_1st_person (
	LIExtCamera* self)
{
	self->transform.current.position = self->transform.target.position;
	liext_camera_calculate_1st_person_transform (self, &self->transform.target);
}

static void private_update_3rd_person (
	LIExtCamera* self,
	float        dist)
{
	dist = liext_camera_calculate_3rd_person_clipped_distance (
		self, &self->transform.center, dist, 0.0f,
		self->config.collision_group, self->config.collision_mask);
	liext_camera_calculate_3rd_person_transform (
		self, &self->transform.center,
		dist, &self->transform.target);
}

static void private_update_modelview (
	LIExtCamera* self)
{
	LIMatTransform t;

	t = limat_transform_invert (self->transform.current);
	self->transform.inverse = t;
	self->view.modelview = limat_convert_transform_to_matrix (t);
}

static void private_update_orientation (
	LIExtCamera* self,
	float        secs)
{
	float dist;
	LIMatVector disp;

	/* Update timer. */
	self->smoothing.timer += secs;
	if (self->smoothing.timer < LIALG_CAMERA_SMOOTHING_TIMESTEP)
		return;

	/* Check if it'd be better to warp than interpolate. */
	disp = limat_vector_subtract (self->transform.target.position, self->transform.current.position);
	dist = limat_vector_get_length (disp);
	if (self->smoothing.timer >= 1.0f || dist >= LIALG_CAMERA_INTERPOLATION_WARP)
	{
		self->transform.current = self->transform.target;
		self->smoothing.timer = 0.0f;
		return;
	}

	/* Interpolate the transformation. */
	while (self->smoothing.timer >= LIALG_CAMERA_SMOOTHING_TIMESTEP)
	{
		liext_camera_calculate_smoothed_transform (
			self,
			&self->transform.target,
			self->smoothing.pos,
			self->smoothing.rot,
			&self->transform.current);
		self->smoothing.timer -= LIALG_CAMERA_SMOOTHING_TIMESTEP;
	}
}

static void private_update_projection (
	LIExtCamera* self)
{
	self->view.projection = limat_matrix_perspective (
		self->view.fov, self->view.aspect,
		self->view.nearplane, self->view.farplane);
}

/** @} */
/** @} */
