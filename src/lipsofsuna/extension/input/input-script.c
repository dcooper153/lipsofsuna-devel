/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIInp Input
 * @{
 * \addtogroup LIInpInput Input
 * @{
 */

#include "input.h"

static void Input_get_cursor_pos (LIScrArgs* args)
{
	int x;
	int y;
	LIInpInput* self;
	LIMatVector tmp;

	self = liscr_script_get_userdata (args->script, LIINP_SCRIPT_INPUT);
	liinp_input_get_pointer (self, &x, &y);
	tmp = limat_vector_init (x, y, 0.0f);
	liscr_args_seti_vector (args, &tmp);
}

static void Input_get_mouse_button_state (LIScrArgs* args)
{
	int s;
	LIInpInput* self;

	self = liscr_script_get_userdata (args->script, LIINP_SCRIPT_INPUT);
	s = liinp_input_get_mouse_button_state (self);
	liscr_args_seti_int (args, s);
}

static void Input_get_pointer_grab (LIScrArgs* args)
{
	LIInpInput* self;

	self = liscr_script_get_userdata (args->script, LIINP_SCRIPT_INPUT);
	liscr_args_seti_bool (args, liinp_input_get_pointer_grab (self));
}
static void Input_set_pointer_grab (LIScrArgs* args)
{
	int value;
	LIInpInput* self;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		self = liscr_script_get_userdata (args->script, LIINP_SCRIPT_INPUT);
		liinp_input_set_pointer_grab (self, value);
	}
}

/*****************************************************************************/

void liinp_script_input (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIINP_SCRIPT_INPUT, "input_get_cursor_pos", Input_get_cursor_pos);
	liscr_script_insert_cfunc (self, LIINP_SCRIPT_INPUT, "input_get_mouse_button_state", Input_get_mouse_button_state);
	liscr_script_insert_cfunc (self, LIINP_SCRIPT_INPUT, "input_get_pointer_grab", Input_get_pointer_grab);
	liscr_script_insert_cfunc (self, LIINP_SCRIPT_INPUT, "input_set_pointer_grab", Input_set_pointer_grab);
}

/** @} */
/** @} */
/** @} */
