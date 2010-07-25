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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliCamera Camera
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Camera"
 * --- Advanced camera control.
 * -- @name Camera
 * -- @class table
 */

/* @luadoc
 * --- Moves the camera forward or backward.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Movement rate.</li>
 * --   <li>keep: True if should keep moving.</li></ul>
 * function Camera.move(self, args)
 */
static void Camera_move (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtModule* module;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			module->move = value;
		else
			lialg_camera_move (module->client->camera, -value);
	}
}

/* @luadoc
 * ---
 * -- Resets the look spring transformation of the camera.
 * --
 * -- @param self Camera class.
 * function Camera.reset(self)
 */
static void Camera_reset (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	transform = limat_transform_identity ();
	module->client->camera->transform.local = transform;
	lialg_camera_warp (module->client->camera);
}

/* @luadoc
 * --- Sets the tilting rate of the camera.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Tilting rate.</li>
 * --   <li>keep: True if should keep tilting.</li></ul>
 * function Camera.tilt(self, args)
 */
static void Camera_tilt (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtModule* module;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		value *= M_PI / 180.0f;
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			module->tilt = value;
		else
			lialg_camera_tilt (module->client->camera, -value);
	}
}

/* @luadoc
 * --- Sets the turning rate of the camera.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Turning rate.</li>
 * --   <li>keep: True if should keep turning.</li></ul>
 * function Camera.turn(self, args)
 */
static void Camera_turn (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtModule* module;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		value *= M_PI / 180.0f;
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			module->turn = value;
		else
			lialg_camera_turn (module->client->camera, value);
	}
}

/* @luadoc
 * --- Adjusts the zoom of the camera.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.<ul>
 * --   <li>rate: Zooming rate.</li>
 * --   <li>keep: True if should keep zooming.</li></ul>
 * function Camera.zoom(self, args)
 */
static void Camera_zoom (LIScrArgs* args)
{
	int keep = 0;
	float value;
	LIExtModule* module;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			module->zoom = value;
		else
			lialg_camera_zoom (module->client->camera, value);
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
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		lialg_camera_set_far (module->client->camera, value);
	}
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
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	switch (lialg_camera_get_driver (module->client->camera))
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
	LIExtModule* module;

	if (liscr_args_geti_string (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		if (!strcmp (value, "first-person"))
			lialg_camera_set_driver (module->client->camera, LIALG_CAMERA_FIRSTPERSON);
		else if (!strcmp (value, "manual"))
			lialg_camera_set_driver (module->client->camera, LIALG_CAMERA_MANUAL);
		else if (!strcmp (value, "third-person"))
			lialg_camera_set_driver (module->client->camera, LIALG_CAMERA_THIRDPERSON);
	}
}

/* @luadoc
 * --- Near plane of the camera.
 * -- @name Camera.far
 * -- @class table
 */
static void Camera_setter_near (LIScrArgs* args)
{
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		lialg_camera_set_near (module->client->camera, value);
	}
}

/* @luadoc
 * --- Camera position.
 * -- @name Camera.position
 * -- @class table
 */
static void Camera_getter_position (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	lialg_camera_get_transform (module->client->camera, &transform);
	liscr_args_seti_vector (args, &transform.position);
}

/* @luadoc
 * --- Camera rotation.
 * -- @name Camera.rotation
 * -- @class table
 */
static void Camera_getter_rotation (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	lialg_camera_get_transform (module->client->camera, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/* @luadoc
 * --- The position of the target of third person camera.
 * -- @name Camera.center
 * -- @class table
 */
static void Camera_setter_target_position (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	transform = limat_transform_identity ();
	lialg_camera_get_center (module->client->camera, &transform);
	liscr_args_geti_vector (args, 0, &transform.position);
	lialg_camera_set_center (module->client->camera, &transform);
}
static void Camera_getter_target_position (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	lialg_camera_get_center (module->client->camera, &transform);
	liscr_args_seti_vector (args, &transform.position);
}

/* @luadoc
 * --- The rotation of the target of third person camera.
 * -- @name Camera.center
 * -- @class table
 */
static void Camera_setter_target_rotation (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	transform = limat_transform_identity ();
	lialg_camera_get_center (module->client->camera, &transform);
	liscr_args_geti_quaternion (args, 0, &transform.rotation);
	lialg_camera_set_center (module->client->camera, &transform);
}
static void Camera_getter_target_rotation (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	lialg_camera_get_center (module->client->camera, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/*****************************************************************************/

void
liext_script_camera (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CAMERA, data);
	liscr_class_insert_cfunc (self, "move", Camera_move);
	liscr_class_insert_cfunc (self, "reset", Camera_reset);
	liscr_class_insert_cfunc (self, "tilt", Camera_tilt);
	liscr_class_insert_cfunc (self, "turn", Camera_turn);
	liscr_class_insert_cfunc (self, "zoom", Camera_zoom);
	liscr_class_insert_cvar (self, "far", NULL, Camera_setter_far);
	liscr_class_insert_cvar (self, "mode", Camera_getter_mode, Camera_setter_mode);
	liscr_class_insert_cvar (self, "near", NULL, Camera_setter_near);
	liscr_class_insert_cvar (self, "position", Camera_getter_position, NULL);
	liscr_class_insert_cvar (self, "rotation", Camera_getter_rotation, NULL);
	liscr_class_insert_cvar (self, "target_position", Camera_getter_target_position, Camera_setter_target_position);
	liscr_class_insert_cvar (self, "target_rotation", Camera_getter_target_rotation, Camera_setter_target_rotation);
}

/** @} */
/** @} */
/** @} */
