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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtCamera Camera
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/camera"
 * --- Camera control.
 * -- @name Camera
 * -- @class table
 */

/* @luadoc
 * --- Creates a new camera.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.
 * function Camera.new(self, args)
 */
static void Camera_new (LIScrArgs* args)
{
	LIAlgCamera* self;
	LIExtModule* module;
	LIScrData* data;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	self = lialg_camera_new ();
	if (self == NULL)
		return;
	lialg_camera_set_driver (self, LIALG_CAMERA_THIRDPERSON);
	lialg_camera_set_clipping (self, liext_cameras_clip_camera, module);
	lialg_camera_set_viewport (self, 0, 0,
		module->client->window->mode.width,
		module->client->window->mode.height);

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, args->clss, lialg_camera_free);
	if (data == NULL)
	{
		lialg_camera_free (self);
		return;
	}
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data);
}

/* @luadoc
 * --- Moves the camera forward or backward.
 * --
 * -- @param self Camera.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Movement rate.</li>
 * --   <li>keep: True if should keep moving.</li></ul>
 * function Camera.move(self, args)
 */
static void Camera_move (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIAlgCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.move_rate = value;
		else
			lialg_camera_move (camera, -value);
	}
}

/* @luadoc
 * --- Creates a picking ray for the current camera configuration.
 * -- @param self Camera.
 * -- @param args Arguments.<ul>
 * --   <li>cursor: Cursor position, in pixels.</li>
 * --   <li>far: Ray end distance, in world coordinate units.</li>
 * --   <li>near: Ray start distance, in world coordinate units.</li></ul>
 * function Camera.picking_ray(self, args)
 */
static void Camera_picking_ray (LIScrArgs* args)
{
	float fardist = 50.0f;
	float neardist = 0.0f;
	LIMatVector cursor;
	LIMatVector dir;
	LIMatVector ray0;
	LIMatVector ray1;
	LIAlgCamera* self = args->self;

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
	lialg_camera_unproject (self, &cursor, &ray0);
	cursor.z = 1.0f;
	lialg_camera_unproject (self, &cursor, &ray1);
	dir = limat_vector_subtract (ray1, ray0);
	dir = limat_vector_normalize (dir);

	/* Apply near and far distances specified by the user. */
	ray1 = limat_vector_add (ray0, limat_vector_multiply (dir, fardist));
	ray0 = limat_vector_add (ray0, limat_vector_multiply (dir, neardist));
	liscr_args_seti_vector (args, &ray0);
	liscr_args_seti_vector (args, &ray1);
}

/* @luadoc
 * ---
 * -- Resets the look spring transformation of the camera.
 * --
 * -- @param self Camera.
 * function Camera.reset(self)
 */
static void Camera_reset (LIScrArgs* args)
{
	LIAlgCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	transform = limat_transform_identity ();
	camera->transform.local = transform;
	lialg_camera_warp (camera);
}

/* @luadoc
 * --- Sets the tilting rate of the camera.
 * --
 * -- @param self Camera.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Tilting rate.</li>
 * --   <li>keep: True if should keep tilting.</li></ul>
 * function Camera.tilt(self, args)
 */
static void Camera_tilt (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIAlgCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		value *= M_PI / 180.0f;
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.tilt_rate = value;
		else
			lialg_camera_tilt (camera, -value);
	}
}

/* @luadoc
 * --- Sets the turning rate of the camera.
 * --
 * -- @param self Camera.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Turning rate.</li>
 * --   <li>keep: True if should keep turning.</li></ul>
 * function Camera.turn(self, args)
 */
static void Camera_turn (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIAlgCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		value *= M_PI / 180.0f;
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.turn_rate = value;
		else
			lialg_camera_turn (camera, value);
	}
}

/* @luadoc
 * --- Warps the camera to the target point.
 * -- @param self Camera.
 * function Camera.warp(self)
 */
static void Camera_warp (LIScrArgs* args)
{
	lialg_camera_warp (args->self);
}

/* @luadoc
 * --- Animates the camera.
 * --
 * -- @param self Camera.
 * -- @param args Arguments.<ul>
 * --   <li>1,secs: Tick length. (required)</li></ul>
 * function Camera.update(self, args)
 */
static void Camera_update (LIScrArgs* args)
{
	float secs;
	LIAlgCamera* camera;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &secs) ||
	    liscr_args_gets_float (args, "secs", &secs))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		camera = args->self;
		liext_cameras_update (module, camera, secs);
	}
}

/* @luadoc
 * --- Adjusts the zoom of the camera.
 * --
 * -- @param self Camera.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Zooming rate.</li>
 * --   <li>keep: True if should keep zooming.</li></ul>
 * function Camera.zoom(self, args)
 */
static void Camera_zoom (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIAlgCamera* camera;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		camera = args->self;
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			camera->controls.zoom_rate = value;
		else
			lialg_camera_zoom (camera, value);
	}
}

/* @luadoc
 * --- Far plane of the camera.
 * -- @name Camera.far
 * -- @class table
 */
static void Camera_setter_far (LIScrArgs* args)
{
	float value;
	LIAlgCamera* camera;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		camera = args->self;
		lialg_camera_set_far (camera, value);
	}
}

/* @luadoc
 * --- The field of view of the camera.
 * -- @name Camera.fov
 * -- @class table
 */
static void Camera_getter_fov (LIScrArgs* args)
{
	liscr_args_seti_float (args, lialg_camera_get_fov (args->self));
}
static void Camera_setter_fov (LIScrArgs* args)
{
	float fov = M_PI / 5.0f;

	liscr_args_geti_float (args, 0, &fov);
	lialg_camera_set_fov (args->self, fov);
}

/* @luadoc
 * --- Camera mode.
 * --
 * -- Recognized values: "first-person"/"manual"/"third-person".
 * --
 * -- @name Camera.mode
 * -- @class table
 */
static void Camera_getter_mode (LIScrArgs* args)
{
	LIAlgCamera* camera;

	camera = args->self;
	switch (lialg_camera_get_driver (camera))
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
static void Camera_setter_mode (LIScrArgs* args)
{
	const char* value;
	LIAlgCamera* camera;

	if (liscr_args_geti_string (args, 0, &value))
	{
		camera = args->self;
		if (!strcmp (value, "first-person"))
			lialg_camera_set_driver (camera, LIALG_CAMERA_FIRSTPERSON);
		else if (!strcmp (value, "manual"))
			lialg_camera_set_driver (camera, LIALG_CAMERA_MANUAL);
		else if (!strcmp (value, "third-person"))
			lialg_camera_set_driver (camera, LIALG_CAMERA_THIRDPERSON);
	}
}

/* @luadoc
 * --- Modelview matrix.
 * -- @name Camera.modelview
 * -- @class table
 */
static void Camera_getter_modelview (LIScrArgs* args)
{
	int i;
	LIAlgCamera* camera;
	LIMatMatrix matrix;

	camera = args->self;
	lialg_camera_get_modelview (camera, &matrix);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	for (i = 0 ; i < 16 ; i++)
		liscr_args_seti_float (args, matrix.m[i]);
}

/* @luadoc
 * --- Near plane of the camera.
 * -- @name Camera.far
 * -- @class table
 */
static void Camera_setter_near (LIScrArgs* args)
{
	float value;
	LIAlgCamera* camera;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		camera = args->self;
		lialg_camera_set_near (camera, value);
	}
}

/* @luadoc
 * --- Camera position.
 * -- @name Camera.position
 * -- @class table
 */
static void Camera_getter_position (LIScrArgs* args)
{
	LIAlgCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	lialg_camera_get_transform (camera, &transform);
	liscr_args_seti_vector (args, &transform.position);
}

/* @luadoc
 * --- The position smoothing factor of the camera.
 * -- @name Camera.position_smoothing
 * -- @class table
 */
static void Camera_getter_position_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lialg_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_seti_float (args, pos);
}
static void Camera_setter_position_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lialg_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_geti_float (args, 0, &pos);
	lialg_camera_set_smoothing (args->self, pos, rot);
}

/* @luadoc
 * --- Projection matrix.
 * -- @name Camera.projection
 * -- @class table
 */
static void Camera_getter_projection (LIScrArgs* args)
{
	int i;
	LIAlgCamera* camera;
	LIMatMatrix matrix;

	camera = args->self;
	lialg_camera_get_projection (camera, &matrix);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	for (i = 0 ; i < 16 ; i++)
		liscr_args_seti_float (args, matrix.m[i]);
}

/* @luadoc
 * --- Camera rotation.
 * -- @name Camera.rotation
 * -- @class table
 */
static void Camera_getter_rotation (LIScrArgs* args)
{
	LIAlgCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	lialg_camera_get_transform (camera, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/* @luadoc
 * --- The rotation smoothing factor of the camera.
 * -- @name Camera.rotation_smoothing
 * -- @class table
 */
static void Camera_getter_rotation_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lialg_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_seti_float (args, rot);
}
static void Camera_setter_rotation_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lialg_camera_get_smoothing (args->self, &pos, &rot);
	liscr_args_geti_float (args, 0, &rot);
	lialg_camera_set_smoothing (args->self, pos, rot);
}

/* @luadoc
 * --- The position of the target of third person camera.
 * -- @name Camera.center
 * -- @class table
 */
static void Camera_setter_target_position (LIScrArgs* args)
{
	LIAlgCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	transform = limat_transform_identity ();
	lialg_camera_get_center (camera, &transform);
	liscr_args_geti_vector (args, 0, &transform.position);
	lialg_camera_set_center (camera, &transform);
}
static void Camera_getter_target_position (LIScrArgs* args)
{
	LIAlgCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	lialg_camera_get_center (camera, &transform);
	liscr_args_seti_vector (args, &transform.position);
}

/* @luadoc
 * --- The rotation of the target of third person camera.
 * -- @name Camera.center
 * -- @class table
 */
static void Camera_setter_target_rotation (LIScrArgs* args)
{
	LIAlgCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	transform = limat_transform_identity ();
	lialg_camera_get_center (camera, &transform);
	liscr_args_geti_quaternion (args, 0, &transform.rotation);
	lialg_camera_set_center (camera, &transform);
}
static void Camera_getter_target_rotation (LIScrArgs* args)
{
	LIAlgCamera* camera;
	LIMatTransform transform;

	camera = args->self;
	lialg_camera_get_center (camera, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/* @luadoc
 * --- The viewport of the camera.
 * -- @name Camera.viewport
 * -- @class table
 */
static void Camera_getter_viewport (LIScrArgs* args)
{
	LIAlgCamera* camera;

	camera = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, camera->view.viewport[0]);
	liscr_args_seti_float (args, camera->view.viewport[1]);
	liscr_args_seti_float (args, camera->view.viewport[2]);
	liscr_args_seti_float (args, camera->view.viewport[3]);
}
static void Camera_setter_viewport (LIScrArgs* args)
{
	int viewport[4];
	LIAlgCamera* camera;

	camera = args->self;
	memcpy (viewport, camera->view.viewport, 4 * sizeof (int));
	liscr_args_geti_int (args, 0, viewport + 0);
	liscr_args_geti_int (args, 1, viewport + 1);
	liscr_args_geti_int (args, 2, viewport + 2);
	liscr_args_geti_int (args, 3, viewport + 3);
	lialg_camera_set_viewport (camera, viewport[0], viewport[1], viewport[2], viewport[3]);
}

/*****************************************************************************/

void liext_script_camera (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CAMERA, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Camera_new);
	liscr_class_insert_mfunc (self, "move", Camera_move);
	liscr_class_insert_mfunc (self, "picking_ray", Camera_picking_ray);
	liscr_class_insert_mfunc (self, "reset", Camera_reset);
	liscr_class_insert_mfunc (self, "tilt", Camera_tilt);
	liscr_class_insert_mfunc (self, "turn", Camera_turn);
	liscr_class_insert_mfunc (self, "update", Camera_update);
	liscr_class_insert_mfunc (self, "warp", Camera_warp);
	liscr_class_insert_mfunc (self, "zoom", Camera_zoom);
	liscr_class_insert_mvar (self, "far", NULL, Camera_setter_far);
	liscr_class_insert_mvar (self, "fov", Camera_getter_fov, Camera_setter_fov);
	liscr_class_insert_mvar (self, "mode", Camera_getter_mode, Camera_setter_mode);
	liscr_class_insert_mvar (self, "near", NULL, Camera_setter_near);
	liscr_class_insert_mvar (self, "modelview", Camera_getter_modelview, NULL);
	liscr_class_insert_mvar (self, "position", Camera_getter_position, NULL);
	liscr_class_insert_mvar (self, "position_smoothing", Camera_getter_position_smoothing, Camera_setter_position_smoothing);
	liscr_class_insert_mvar (self, "projection", Camera_getter_projection, NULL);
	liscr_class_insert_mvar (self, "rotation", Camera_getter_rotation, NULL);
	liscr_class_insert_mvar (self, "rotation_smoothing", Camera_getter_rotation_smoothing, Camera_setter_rotation_smoothing);
	liscr_class_insert_mvar (self, "target_position", Camera_getter_target_position, Camera_setter_target_position);
	liscr_class_insert_mvar (self, "target_rotation", Camera_getter_target_rotation, Camera_setter_target_rotation);
	liscr_class_insert_mvar (self, "viewport", Camera_getter_viewport, Camera_setter_viewport);
}

/** @} */
/** @} */
