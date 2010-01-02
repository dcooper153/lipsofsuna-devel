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

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Camera"
 * ---
 * -- Advanced camera control.
 * -- @name Camera
 * -- @class table
 */

/* @luadoc
 * ---
 * -- First person camera driver.
 * --
 * -- @name Camera.FIRSTPERSON
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Third person camera driver.
 * --
 * -- @name Camera.THIRDPERSON
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Manual camera driver.
 * --
 * -- @name Camera.MANUAL
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Moves the camera forward or backward.
 * --
 * -- Arguments:
 * -- rate: Movement rate.
 * -- keep: True if should keep moving.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.
 * function Camera.move(self, args)
 */
static void Camera_move (liscrArgs* args)
{
	int keep = 0;
	float value;
	liextModule* module;

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
static void Camera_reset (liscrArgs* args)
{
	liextModule* module;
	limatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	transform = limat_transform_identity ();
	module->client->camera->transform.local = transform;
	lialg_camera_warp (module->client->camera);
}

/* @luadoc
 * ---
 * -- Sets the tilting rate of the camera.
 * --
 * -- Arguments:
 * -- rate: Tilting rate.
 * -- keep: True if should keep tilting.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.
 * function Camera.tilt(self, args)
 */
static void Camera_tilt (liscrArgs* args)
{
	int keep = 0;
	float value;
	liextModule* module;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			module->tilt = value;
		else
			lialg_camera_tilt (module->client->camera, -value);
	}
}

/* @luadoc
 * ---
 * -- Sets the turning rate of the camera.
 * --
 * -- Arguments:
 * -- rate: Turning rate.
 * -- keep: True if should keep turning.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.
 * function Camera.turn(self, args)
 */
static void Camera_turn (liscrArgs* args)
{
	int keep = 0;
	float value;
	liextModule* module;

	if (liscr_args_gets_float (args, "rate", &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		liscr_args_gets_bool (args, "keep", &keep);
		if (keep)
			module->turn = value;
		else
			lialg_camera_tilt (module->client->camera, value);
	}
}

/* @luadoc
 * ---
 * -- Adjusts the zoom of the camera.
 * --
 * -- Arguments:
 * -- rate: Zooming rate.
 * -- keep: True if should keep zooming.
 * --
 * -- @param self Camera class.
 * -- @param args Arguments.
 * function Camera.zoom(self, args)
 */
static void Camera_zoom (liscrArgs* args)
{
	int keep = 0;
	float value;
	liextModule* module;

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
 * ---
 * -- Far plane of the camera.
 * -- @name Camera.far
 * -- @class table
 */
static void Camera_setter_far (liscrArgs* args)
{
	float value;
	liextModule* module;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		lialg_camera_set_far (module->client->camera, value);
	}
}

/* @luadoc
 * ---
 * -- Camera mode.
 * -- @name Camera.mode
 * -- @class table
 */
static void Camera_getter_mode (liscrArgs* args)
{
	liextModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	liscr_args_seti_int (args, lialg_camera_get_driver (module->client->camera));
}
static void Camera_setter_mode (liscrArgs* args)
{
	int value;
	liextModule* module;

	if (liscr_args_geti_int (args, 0, &value) && value >= 0 && value < LIALG_CAMERA_MAX)
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		lialg_camera_set_driver (module->client->camera, value);
	}
}

/* @luadoc
 * ---
 * -- Near plane of the camera.
 * -- @name Camera.far
 * -- @class table
 */
static void Camera_setter_near (liscrArgs* args)
{
	float value;
	liextModule* module;

	if (liscr_args_geti_float (args, 0, &value) && value > 0.0f)
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
		lialg_camera_set_near (module->client->camera, value);
	}
}

/* @luadoc
 * ---
 * -- Camera position.
 * -- @name Camera.position
 * -- @class table
 */
static void Camera_getter_position (liscrArgs* args)
{
	liextModule* module;
	limatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	lialg_camera_get_transform (module->client->camera, &transform);
	liscr_args_seti_vector (args, &transform.position);
}

/* @luadoc
 * ---
 * -- Camera rotation.
 * -- @name Camera.rotation
 * -- @class table
 */
static void Camera_getter_rotation (liscrArgs* args)
{
	liextModule* module;
	limatTransform transform;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_CAMERA);
	lialg_camera_get_transform (module->client->camera, &transform);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/*****************************************************************************/

void
liextCameraScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CAMERA, data);
	liscr_class_insert_enum (self, "FIRSTPERSON", LIALG_CAMERA_FIRSTPERSON);
	liscr_class_insert_enum (self, "THIRDPERSON", LIALG_CAMERA_THIRDPERSON);
	liscr_class_insert_enum (self, "MANUAL", LIALG_CAMERA_MANUAL);
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
}

/** @} */
/** @} */
/** @} */
