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

#include "ext-module.h"

static void Camera_new (LIScrArgs* args)
{
	LIExtCamera* self;
	LIExtModule* module;
	LIScrData* data;
	LIRenRender* render;
	LIRenVideomode mode;

	/* Allocate self. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_CAMERA);
	self = liext_camera_new ();
	if (self == NULL)
		return;
	liext_camera_set_driver (self, LIALG_CAMERA_THIRDPERSON);
	liext_camera_set_clipping (self, (LIExtCameraClip) liext_cameras_clip_camera, module);

	/* Initialize the viewport. */
	render = limai_program_find_component (module->program, "render");
	if (render)
	{
		liren_render_get_videomode (render, &mode);
		liext_camera_set_viewport (self, 0, 0, mode.width, mode.height);
	}

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_CAMERA, liext_camera_free);
	if (data == NULL)
	{
		liext_camera_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Camera_move (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.move_rate = value;
		else
			liext_camera_move (camera, -value);
	}
}

static void Camera_picking_ray (LIScrArgs* args)
{
	float fardist = 50.0f;
	float neardist = 0.0f;
	LIMatVector cursor;
	LIMatVector dir;
	LIMatVector ray0;
	LIMatVector ray1;
	LIExtCamera* self = args->self;

	/* Handle arguments. */
	liscr_args_gets_float (args, "far", &fardist);
	liscr_args_gets_float (args, "near", &neardist);
	if (!liscr_args_gets_vector (args, "cursor", &cursor))
	{
		cursor.x = self->view.viewport[0] + self->view.viewport[2] / 2.0f;
		cursor.y = self->view.viewport[1] + self->view.viewport[3] / 2.0f;
	}
	else
		cursor.y = self->view.viewport[3] - cursor.y - 1;

	/* Calculate ray vector. */
	cursor.z = 0.0f;
	if (!liext_camera_unproject (self, &cursor, &ray0))
		return;
	cursor.z = 1.0f;
	if (!liext_camera_unproject (self, &cursor, &ray1))
		return;
	dir = limat_vector_subtract (ray1, ray0);
	dir = limat_vector_normalize (dir);

	/* Apply near and far distances specified by the user. */
	ray1 = limat_vector_add (ray0, limat_vector_multiply (dir, fardist));
	ray0 = limat_vector_add (ray0, limat_vector_multiply (dir, neardist));
	liscr_args_seti_vector (args, &ray0);
	liscr_args_seti_vector (args, &ray1);
}

static void Camera_reset (LIScrArgs* args)
{
	LIExtCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	transform = limat_transform_identity ();
	camera->transform.local = transform;
	liext_camera_warp (camera);
}

static void Camera_tilt (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		value *= M_PI / 180.0f;
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.tilt_rate = value;
		else
			liext_camera_tilt (camera, -value);
	}
}

static void Camera_turn (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		value *= M_PI / 180.0f;
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.turn_rate = value;
		else
			liext_camera_turn (camera, value);
	}
}

static void Camera_warp (LIScrArgs* args)
{
	liext_camera_warp (args->self);
}

static void Camera_update (LIScrArgs* args)
{
	float secs;

	if (liscr_args_geti_float (args, 0, &secs))
		liext_camera_update (args->self, secs);
}

static void Camera_zoom (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.zoom_rate = value;
		else
			liext_camera_zoom (camera, value);
	}
}

static void Camera_get_collision_group (LIScrArgs* args)
{
	LIExtCamera* camera;

	camera = args->self;
	liscr_args_seti_int (args, camera->config.collision_group);
}
static void Camera_set_collision_group (LIScrArgs* args)
{
	int value;
	LIExtCamera* camera;

	camera = args->self;
	if (liscr_args_geti_int (args, 0, &value))
		camera->config.collision_group = value;
}

static void Camera_get_collision_mask (LIScrArgs* args)
{
	LIExtCamera* camera;

	camera = args->self;
	liscr_args_seti_int (args, camera->config.collision_mask);
}
static void Camera_set_collision_mask (LIScrArgs* args)
{
	int value;
	LIExtCamera* camera;

	camera = args->self;
	if (liscr_args_geti_int (args, 0, &value))
		camera->config.collision_mask = value;
}

static void Camera_set_far (LIScrArgs* args)
{
	float value;
	LIExtCamera* camera;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		camera = args->self;
		liext_camera_set_far (camera, value);
	}
}

static void Camera_get_fov (LIScrArgs* args)
{
	liscr_args_seti_float (args, liext_camera_get_fov (args->self));
}
static void Camera_set_fov (LIScrArgs* args)
{
	float fov = M_PI / 5.0f;

	liscr_args_geti_float (args, 0, &fov);
	liext_camera_set_fov (args->self, fov);
}

static void Camera_get_mode (LIScrArgs* args)
{
	LIExtCamera* camera;

	camera = args->self;
	switch (liext_camera_get_driver (camera))
	{
		case LIALG_CAMERA_FIRSTPERSON:
			liscr_args_seti_string (args, "first-person");
			break;
		case LIALG_CAMERA_MANUAL:
			liscr_args_seti_string (args, "manual");
			break;
		case LIALG_CAMERA_THIRDPERSON:
			liscr_args_seti_string (args, "third-person");
			break;
	}
}
static void Camera_set_mode (LIScrArgs* args)
{
	const char* value;
	LIExtCamera* camera;

	if (liscr_args_geti_string (args, 0, &value))
	{
		camera = args->self;
		if (!strcmp (value, "first-person"))
			liext_camera_set_driver (camera, LIALG_CAMERA_FIRSTPERSON);
		else if (!strcmp (value, "manual"))
			liext_camera_set_driver (camera, LIALG_CAMERA_MANUAL);
		else if (!strcmp (value, "third-person"))
			liext_camera_set_driver (camera, LIALG_CAMERA_THIRDPERSON);
	}
}

static void Camera_get_modelview (LIScrArgs* args)
{
	int i;
	LIExtCamera* camera;
	LIMatMatrix matrix;

	camera = args->self;
	liext_camera_get_modelview (camera, &matrix);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	for (i = 0 ; i < 16 ; i++)
		liscr_args_seti_float (args, matrix.m[i]);
}

static void Camera_set_near (LIScrArgs* args)
{
	float value;
	LIExtCamera* camera;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		camera = args->self;
		liext_camera_set_near (camera, value);
	}
}

static void Camera_get_position (LIScrArgs* args)
{
	LIExtCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	liext_camera_get_transform (camera, &transform);
	liscr_args_seti_vector (args, &transform.position);
}

static void Camera_get_position_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	liext_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_seti_float (args, pos);
}
static void Camera_set_position_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	liext_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_geti_float (args, 0, &pos);
	liext_camera_set_smoothing (args->self, pos, rot);
}

static void Camera_get_projection (LIScrArgs* args)
{
	int i;
	LIExtCamera* camera;
	LIMatMatrix matrix;

	camera = args->self;
	liext_camera_get_projection (camera, &matrix);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	for (i = 0 ; i < 16 ; i++)
		liscr_args_seti_float (args, matrix.m[i]);
}

static void Camera_get_rotation (LIScrArgs* args)
{
	LIExtCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	liext_camera_get_transform (camera, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

static void Camera_get_rotation_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	liext_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_seti_float (args, rot);
}
static void Camera_set_rotation_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	liext_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_geti_float (args, 0, &rot);
	liext_camera_set_smoothing (args->self, pos, rot);
}

static void Camera_set_target_position (LIScrArgs* args)
{
	LIExtCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	transform = limat_transform_identity ();
	liext_camera_get_center (camera, &transform);
	liscr_args_geti_vector (args, 0, &transform.position);
	liext_camera_set_center (camera, &transform);
}
static void Camera_get_target_position (LIScrArgs* args)
{
	LIExtCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	liext_camera_get_center (camera, &transform);
	liscr_args_seti_vector (args, &transform.position);
}

static void Camera_set_target_rotation (LIScrArgs* args)
{
	LIExtCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	transform = limat_transform_identity ();
	liext_camera_get_center (camera, &transform);
	liscr_args_geti_quaternion (args, 0, &transform.rotation);
	liext_camera_set_center (camera, &transform);
}
static void Camera_get_target_rotation (LIScrArgs* args)
{
	LIExtCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	liext_camera_get_center (camera, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

static void Camera_get_viewport (LIScrArgs* args)
{
	LIExtCamera* camera;

	camera = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, camera->view.viewport[0]);
	liscr_args_seti_float (args, camera->view.viewport[1]);
	liscr_args_seti_float (args, camera->view.viewport[2]);
	liscr_args_seti_float (args, camera->view.viewport[3]);
}
static void Camera_set_viewport (LIScrArgs* args)
{
	int viewport[4];
	LIExtCamera* camera;

	camera = args->self;
	memcpy (viewport, camera->view.viewport, 4 * sizeof (int));
	liscr_args_geti_int (args, 0, viewport + 0);
	liscr_args_geti_int (args, 1, viewport + 1);
	liscr_args_geti_int (args, 2, viewport + 2);
	liscr_args_geti_int (args, 3, viewport + 3);
	liext_camera_set_viewport (camera, viewport[0], viewport[1], viewport[2], viewport[3]);
}

/*****************************************************************************/

void liext_script_camera (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_CAMERA, "camera_new", Camera_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_move", Camera_move);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_picking_ray", Camera_picking_ray);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_reset", Camera_reset);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_tilt", Camera_tilt);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_turn", Camera_turn);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_update", Camera_update);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_warp", Camera_warp);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_zoom", Camera_zoom);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_collision_group", Camera_get_collision_group);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_collision_group", Camera_set_collision_group);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_collision_mask", Camera_get_collision_mask);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_collision_mask", Camera_set_collision_mask);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_far", Camera_set_far);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_fov", Camera_get_fov);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_fov", Camera_set_fov);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_mode", Camera_get_mode);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_mode", Camera_set_mode);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_near", Camera_set_near);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_modelview", Camera_get_modelview);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_position", Camera_get_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_position_smoothing", Camera_get_position_smoothing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_position_smoothing", Camera_set_position_smoothing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_projection", Camera_get_projection);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_rotation", Camera_get_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_rotation_smoothing", Camera_get_rotation_smoothing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_rotation_smoothing", Camera_set_rotation_smoothing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_target_position", Camera_get_target_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_target_position", Camera_set_target_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_target_rotation", Camera_get_target_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_target_rotation", Camera_set_target_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_get_viewport", Camera_get_viewport);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_CAMERA, "camera_set_viewport", Camera_set_viewport);
}

/** @} */
/** @} */
