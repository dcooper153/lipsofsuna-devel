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

#ifndef __EXT_INPUT_INPUT_H__
#define __EXT_INPUT_INPUT_H__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/render.h"

#define LIINP_SCRIPT_INPUT "Input"

typedef struct _LIInpInput LIInpInput;

LIAPICALL (LIInpInput*, liinp_input_new, (
	LIMaiProgram* program));

LIAPICALL (void, liinp_input_free, (
	LIInpInput* self));

LIAPICALL (void, liinp_input_get_pointer, (
	LIInpInput* self,
	int*        x,
	int*        y));

LIAPICALL (int, liinp_input_get_mouse_button_state, (
	LIInpInput* self));

LIAPICALL (int, liinp_input_get_pointer_grab, (
	LIInpInput* self));

LIAPICALL (void, liinp_input_set_pointer_grab, (
	LIInpInput* self,
	int         value));

/*****************************************************************************/

LIAPICALL (void, liinp_script_input, (
	LIScrScript* self));

#endif
