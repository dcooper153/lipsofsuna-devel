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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 * \addtogroup licliscrPlayer Player
 * @{
 */

#include <stdlib.h>
#include <client/lips-client.h>
#include "lips-client-script.h"

/* @luadoc
 * module "Core.Client.Player"
 * ---
 * -- Define how the player moves and interacts with the world.
 * -- @name Player
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Sets the tilting rate of the player.
 * --
 * -- @param self Module class.
 * -- @param value Tilting rate.
 * -- @param [keep] True if should keep tilting, false if instantaneous.
 * function Module.tilt(self, value, keep)
 */
static int
Player_tilt (lua_State* lua)
{
	int keep;
	float value;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);
	value = luaL_checknumber (lua, 2);
	keep = lua_toboolean (lua, 3);

	if (module->network != NULL)
		licli_network_tilt (module->network, value, keep);
	return 0;
}

/* @luadoc
 * ---
 * -- Sets the turning rate of the player.
 * --
 * -- @param self Module class.
 * -- @param value Turning rate.
 * -- @param [keep] True if should keep turning, false if instantaneous.
 * function Module.turn(self, value, keep)
 */
static int
Player_turn (lua_State* lua)
{
	int keep;
	float value;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);
	value = luaL_checknumber (lua, 2);
	keep = lua_toboolean (lua, 3);

	if (module->network != NULL)
		licli_network_turn (module->network, value, keep);
	return 0;
}

/* @luadoc
 * ---
 * -- FIXME
 * -- @name Player.analog
 * -- @class table
 */
static int
Player_getter_analog (lua_State* lua)
{
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);

	if (module->network == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	lua_pushboolean (lua, module->network->analog);
	return 1;
}
static int
Player_setter_analog (lua_State* lua)
{
	int value;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);
	luaL_checkany (lua, 2);
	value = lua_toboolean (lua, 2);

	if (module->network == NULL)
		return 0;
	module->network->analog = value;
	return 0;
}

/* @luadoc
 * ---
 * -- FIXME
 * -- @name Player.move
 * -- @class table
 */
static int
Player_getter_move (lua_State* lua)
{
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);

	if (module->network == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	lua_pushnumber (lua, module->network->curr.controls.move);
	return 1;
}
static int
Player_setter_move (lua_State* lua)
{
	float value;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);
	value = luaL_checknumber (lua, 3);

	if (module->network == NULL)
		return 0;
	module->network->curr.controls.move = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Current rotation quaternion of the player.
 * -- @name Player.rotation
 * -- @class table
 */
static int
Player_getter_rotation (lua_State* lua)
{
	licliModule* module;
	limatQuaternion tmp;
	liscrData* quat;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);

	if (module->network == NULL)
		return 0;
	licli_network_get_rotation (module->network, &tmp);
	quat = liscr_quaternion_new (script, &tmp);
	if (quat == NULL)
		return 0;
	liscr_pushdata (lua, quat);
	liscr_data_unref (quat, NULL);

	return 1;
}
static int
Player_setter_rotation (lua_State* lua)
{
	licliModule* module;
	liscrData* quat;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);
	quat = liscr_checkdata (lua, 3, LICOM_SCRIPT_QUATERNION);

	if (module->network == NULL)
		return 0;
	licli_network_set_rotation (module->network, quat->data);

	return 0;
}

/* @luadoc
 * ---
 * -- Gets or sets the tilting rate of the player.
 * -- @name Player.tilt_rate
 * -- @class table
 */
static int
Player_getter_tilt_rate (lua_State* lua)
{
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);

	if (module->network == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	lua_pushnumber (lua, module->network->curr.controls.tilt);
	return 1;
}
static int
Player_setter_tilt_rate (lua_State* lua)
{
	float value;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);
	value = luaL_checknumber (lua, 3);

	if (module->network == NULL)
		return 0;
	module->network->curr.controls.tilt = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Gets or sets the turning rate of the player.
 * -- @name Player.turn_rate
 * -- @class table
 */
static int
Player_getter_turn_rate (lua_State* lua)
{
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);

	if (module->network == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	lua_pushnumber (lua, module->network->curr.controls.turn);
	return 1;
}
static int
Player_setter_turn_rate (lua_State* lua)
{
	float value;
	licliModule* module;

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_PLAYER);
	value = luaL_checknumber (lua, 3);

	if (module->network == NULL)
		return 0;
	module->network->curr.controls.turn = value;
	return 0;
}

/*****************************************************************************/

void
licliPlayerScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_PLAYER, data);
	liscr_class_insert_func (self, "tilt", Player_tilt);
	liscr_class_insert_func (self, "turn", Player_turn);
	liscr_class_insert_getter (self, "analog", Player_getter_analog);
	liscr_class_insert_getter (self, "move", Player_getter_move);
	liscr_class_insert_getter (self, "rotation", Player_getter_rotation);
	liscr_class_insert_getter (self, "tilt_rate", Player_getter_tilt_rate);
	liscr_class_insert_getter (self, "turn_rate", Player_getter_turn_rate);
	liscr_class_insert_setter (self, "analog", Player_setter_analog);
	liscr_class_insert_setter (self, "move", Player_setter_move);
	liscr_class_insert_setter (self, "rotation", Player_setter_rotation);
	liscr_class_insert_setter (self, "tilt_rate", Player_setter_tilt_rate);
	liscr_class_insert_setter (self, "turn_rate", Player_setter_turn_rate);
}

/** @} */
/** @} */
/** @} */
