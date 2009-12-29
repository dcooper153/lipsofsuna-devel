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
 */

#include <system/lips-system.h>
#include "lips-client.h"

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
 * -- Arguments:
 * -- rate: Tilting rate. (required)
 * -- keep: True if should keep tilting.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * function Module.tilt(self, args)
 */
static void Player_tilt (liscrArgs* args)
{
	int keep;
	float value;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (liscr_args_gets_float (args, "rate", &value) && module->network == NULL)
	{
		liscr_args_gets_bool (args, "keep", &keep);
		licli_network_tilt (module->network, value, keep);
	}
}

/* @luadoc
 * ---
 * -- Sets the turning rate of the player.
 * --
 * -- Arguments:
 * -- rate: Turning rate. (required)
 * -- keep: True if should keep turning.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * function Module.turn(self, args)
 */
static void Player_turn (liscrArgs* args)
{
	int keep;
	float value;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (liscr_args_gets_float (args, "rate", &value) && module->network == NULL)
	{
		liscr_args_gets_bool (args, "keep", &keep);
		licli_network_turn (module->network, value, keep);
	}
}

/* @luadoc
 * ---
 * -- FIXME
 * -- @name Player.analog
 * -- @class table
 */
static void Player_getter_analog (liscrArgs* args)
{
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (module->network != NULL)
		liscr_args_seti_float (args, module->network->analog);
}
static void Player_setter_analog (liscrArgs* args)
{
	int value;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (liscr_args_geti_bool (args, 0, &value) && module->network != NULL)
		module->network->analog = value;
}

/* @luadoc
 * ---
 * -- FIXME
 * -- @name Player.move
 * -- @class table
 */
static void Player_getter_move (liscrArgs* args)
{
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (module->network != NULL)
		liscr_args_seti_float (args, module->network->curr.controls.move);
}
static void Player_setter_move (liscrArgs* args)
{
	float value;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (liscr_args_geti_float (args, 0, &value) && module->network != NULL)
		module->network->curr.controls.move = value;
}

/* @luadoc
 * ---
 * -- Player object or nil if no object is present.
 * -- @name Player.object
 * -- @class table
 */
static void Player_getter_object (liscrArgs* args)
{
	licliModule* module;
	liengObject* object;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (module->network != NULL)
	{
		object = lieng_engine_find_object (module->engine, module->network->id);
		if (object != NULL)
			liscr_args_seti_data (args, object->script);
	}
}

/* @luadoc
 * ---
 * -- Current rotation quaternion of the player.
 * -- @name Player.rotation
 * -- @class table
 */
static void Player_getter_rotation (liscrArgs* args)
{
	licliModule* module;
	limatQuaternion tmp;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (module->network != NULL)
	{
		licli_network_get_rotation (module->network, &tmp);
		liscr_args_seti_quaternion (args, &tmp);
	}
}
static void Player_setter_rotation (liscrArgs* args)
{
	licliModule* module;
	liscrData* data;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (liscr_args_geti_data (args, 0, LICOM_SCRIPT_QUATERNION, &data) && module->network != NULL)
		licli_network_set_rotation (module->network, data->data);
}

/* @luadoc
 * ---
 * -- Gets or sets the tilting rate of the player.
 * -- @name Player.tilt_rate
 * -- @class table
 */
static void Player_getter_tilt_rate (liscrArgs* args)
{
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (module->network != NULL)
		liscr_args_seti_float (args, module->network->curr.controls.tilt);
}
static void Player_setter_tilt_rate (liscrArgs* args)
{
	float value;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (liscr_args_geti_float (args, 0, &value))
		module->network->curr.controls.tilt = value;
}

/* @luadoc
 * ---
 * -- Gets or sets the turning rate of the player.
 * -- @name Player.turn_rate
 * -- @class table
 */
static void Player_getter_turn_rate (liscrArgs* args)
{
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (module->network != NULL)
		liscr_args_seti_float (args, module->network->curr.controls.turn);
}
static void Player_setter_turn_rate (liscrArgs* args)
{
	float value;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_PLAYER);
	if (liscr_args_geti_float (args, 0, &value) && module->network != NULL)
		module->network->curr.controls.turn = value;
}

/*****************************************************************************/

void
licliPlayerScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_PLAYER, data);
	liscr_class_insert_cfunc (self, "tilt", Player_tilt);
	liscr_class_insert_cfunc (self, "turn", Player_turn);
	liscr_class_insert_cvar (self, "analog", Player_getter_analog, Player_setter_analog);
	liscr_class_insert_cvar (self, "move", Player_getter_move, Player_setter_move);
	liscr_class_insert_cvar (self, "object", Player_getter_object, NULL);
	liscr_class_insert_cvar (self, "rotation", Player_getter_rotation, Player_setter_rotation);
	liscr_class_insert_cvar (self, "tilt_rate", Player_getter_tilt_rate, Player_setter_tilt_rate);
	liscr_class_insert_cvar (self, "turn_rate", Player_getter_turn_rate, Player_setter_turn_rate);
}

/** @} */
/** @} */
