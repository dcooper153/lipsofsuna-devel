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
 * \addtogroup LIExtBinding Binding
 * @{
 */

#include "ext-module.h"

/*****************************************************************************/

/* @luadoc
 * module "Extension.Binding"
 * --- Bind controls to actions.
 * -- @name Binding
 * -- @class table
 */

/* @luadoc
 * --- Creates a new binding.
 * --
 * -- @param self Binding class.
 * -- @param args Arguments.<ul>
 * --   <li>action: Action or valid action name. (required)</li>
 * --   <li>joystickaxis: Joystick axis number.</li>
 * --   <li>joystickbutton: Joystick button number.</li>
 * --   <li>key: Key code.</li>
 * --   <li>mods: Key modifier mask.</li>
 * --   <li>mouseaxis: Mouse axis number.</li>
 * --   <li>mousebutton: Mouse button number.</li>
 * --   <li>mousedelta: Mouse axis number.</li>
 * --   <li>mult: Value multiplier.</li>
 * --   <li>params: Free form string passed to the action handler.</li></ul>
 * -- @return New Binding.
 * function Binding.new(self, args)
 */
static void Binding_new (LIScrArgs* args)
{
	int type;
	int code;
	int mods = 0;
	float mult = 1.0f;
	const char* name;
	const char* params = "";
	LIBndAction* action;
	LIBndBinding* self;
	LIExtModule* module;
	LIScrData* data;

	/* Arguments. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_BINDING);
	if (liscr_args_gets_string (args, "action", &name))
	{
		action = libnd_manager_find_action (module->bindings, name);
		if (action == NULL)
			return;
	}
	else if (liscr_args_gets_data (args, "action", LIEXT_SCRIPT_ACTION, &data)) 
		action = data->data;
	else
		return;
	if (liscr_args_gets_int (args, "joystickaxis", &code)) type = LIBND_TYPE_JOYSTICK_AXIS;
	else if (liscr_args_gets_int (args, "joysticbutton", &code)) type = LIBND_TYPE_JOYSTICK;
	else if (liscr_args_gets_int (args, "key", &code)) type = LIBND_TYPE_KEYBOARD;
	else if (liscr_args_gets_int (args, "mouseaxis", &code)) type = LIBND_TYPE_MOUSE_AXIS;
	else if (liscr_args_gets_int (args, "mousebutton", &code)) type = LIBND_TYPE_MOUSE;
	else if (liscr_args_gets_int (args, "mousedelta", &code)) type = LIBND_TYPE_MOUSE_DELTA;
	else
		return;
	liscr_args_gets_int (args, "mods", &mods);
	liscr_args_gets_float (args, "mult", &mult);
	liscr_args_gets_string (args, "params", &params);

	/* Allocate userdata. */
	self = libnd_binding_new (module->bindings, type, action, params, code, mods, mult);
	if (self == NULL)
		return;
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_BINDING, libnd_binding_free);
	if (data == NULL)
	{
		libnd_binding_free (self);
		return;
	}
	libnd_binding_set_userdata (self, data);
	liscr_args_seti_data (args, data);
}

/* @luadoc
 * --- Removes the binding.
 * --
 * -- @param self Binding.
 * function Binding.free(self)
 */
static void Binding_free (LIScrArgs* args)
{
#warning Bindings break if freed multiple times.
	liscr_data_unref (args->self, NULL);
}

/*****************************************************************************/

void liext_script_binding (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_BINDING, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Binding_new);
	liscr_class_insert_mfunc (self, "free", Binding_free);
}

/** @} */
/** @} */
