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
 * -- Moves the zoom of the camera.
 * --
 * -- @param self Camera class.
 * -- @param value Movement value.
 * -- @param [keep] True if should keep moving, false if instantaneous.
 * function Camera.move(self, value, keep)
 */
static int
Camera_move (lua_State* lua)
{
	int keep;
	float value;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);
	value = luaL_checknumber (lua, 2);
	keep = lua_toboolean (lua, 3);

	if (keep)
		self->move = value;
	else
		lieng_camera_move (self->module->camera, value);

	return 0;
}

/* @luadoc
 * ---
 * -- Sets the tilting rate of the camera.
 * --
 * -- @param self Camera class.
 * -- @param value Tilting rate.
 * -- @param [keep] True if should keep tilting, false if instantaneous.
 * function Camera.tilt(self, value, keep)
 */
static int
Camera_tilt (lua_State* lua)
{
	int keep;
	float value;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);
	value = luaL_checknumber (lua, 2);
	keep = lua_toboolean (lua, 3);

	if (keep)
		self->tilt = value;
	else
		lieng_camera_tilt (self->module->camera, -value);

	return 0;
}

/* @luadoc
 * ---
 * -- Sets the turning rate of the camera.
 * --
 * -- @param self Camera class.
 * -- @param value Turning rate.
 * -- @param [keep] True if should keep turning, false if instantaneous.
 * function Camera.turn(self, value, keep)
 */
static int
Camera_turn (lua_State* lua)
{
	int keep;
	float value;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);
	value = luaL_checknumber (lua, 2);
	keep = lua_toboolean (lua, 3);

	if (keep)
		self->turn = value;
	else
		lieng_camera_turn (self->module->camera, value);

	return 0;
}

/* @luadoc
 * ---
 * -- Adjusts the zoom of the camera.
 * --
 * -- @param self Camera class.
 * -- @param value Zooming rate.
 * -- @param [keep] True if should keep zooming, false if instantaneous.
 * function Camera.zoom(self, value, keep)
 */
static int
Camera_zoom (lua_State* lua)
{
	int keep;
	float value;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);
	value = luaL_checknumber (lua, 2);
	keep = lua_toboolean (lua, 3);

	if (keep)
		self->zoom = value;
	else
		lieng_camera_zoom (self->module->camera, value);

	return 0;
}

/* @luadoc
 * ---
 * -- Far plane of the camera.
 * -- @name Camera.far
 * -- @class table
 */
static int
Camera_setter_far (lua_State* lua)
{
	int value;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);
	value = luaL_checkint (lua, 3);

	lieng_camera_set_far (module->module->camera, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Camera mode.
 * -- @name Camera.mode
 * -- @class table
 */
static int
Camera_getter_mode (lua_State* lua)
{
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);

	lua_pushnumber (lua, lieng_camera_get_driver (module->module->camera));
	return 1;
}
static int
Camera_setter_mode (lua_State* lua)
{
	int value;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);
	value = luaL_checkint (lua, 3);

	lieng_camera_set_driver (module->module->camera, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Near plane of the camera.
 * -- @name Camera.far
 * -- @class table
 */
static int
Camera_setter_near (lua_State* lua)
{
	int value;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);
	value = luaL_checkint (lua, 3);

	lieng_camera_set_near (module->module->camera, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Camera position.
 * -- @name Camera.position
 * -- @class table
 */
static int
Camera_getter_position (lua_State* lua)
{
	liextModule* module;
	limatTransform transform;
	liscrData* vector;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);

	lieng_camera_get_transform (module->module->camera, &transform);
	vector = liscr_vector_new (script, &transform.position);
	if (vector != NULL)
	{
		liscr_pushdata (lua, vector);
		liscr_data_unref (vector, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}

/* @luadoc
 * ---
 * -- Camera rotation.
 * -- @name Camera.rotation
 * -- @class table
 */
static int
Camera_getter_rotation (lua_State* lua)
{
	liextModule* module;
	limatTransform transform;
	liscrData* quat;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CAMERA);

	lieng_camera_get_transform (module->module->camera, &transform);
	quat = liscr_quaternion_new (script, &transform.rotation);
	if (quat != NULL)
	{
		liscr_pushdata (lua, quat);
		liscr_data_unref (quat, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}

/*****************************************************************************/

void
liextCameraScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CAMERA, data);
	liscr_class_insert_enum (self, "FIRSTPERSON", LIENG_CAMERA_DRIVER_FIRSTPERSON);
	liscr_class_insert_enum (self, "THIRDPERSON", LIENG_CAMERA_DRIVER_THIRDPERSON);
	liscr_class_insert_enum (self, "MANUAL", LIENG_CAMERA_DRIVER_MANUAL);
	liscr_class_insert_func (self, "move", Camera_move);
	liscr_class_insert_func (self, "tilt", Camera_tilt);
	liscr_class_insert_func (self, "turn", Camera_turn);
	liscr_class_insert_func (self, "zoom", Camera_zoom);
	liscr_class_insert_getter (self, "mode", Camera_getter_mode);
	liscr_class_insert_getter (self, "position", Camera_getter_position);
	liscr_class_insert_getter (self, "rotation", Camera_getter_rotation);
	liscr_class_insert_setter (self, "far", Camera_setter_far);
	liscr_class_insert_setter (self, "mode", Camera_setter_mode);
	liscr_class_insert_setter (self, "near", Camera_setter_near);
}

/** @} */
/** @} */
/** @} */
