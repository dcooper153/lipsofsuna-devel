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

#include "lips-client.h"

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Binding"
 * ---
 * -- Bind controls to actions.
 * -- @name Binding
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Trigger when a key is pressed.
 * -- @name Binding.KEYBOARD
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Trigger when a mouse button is pressed.
 * -- @name Binding.MOUSE
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Trigger when mouse is moved.
 * -- @name Binding.MOUSE_AXIS
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Trigger when mouse is moved.
 * -- @name Binding.MOUSE_DELTA
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Trigger when joystick button is pressed.
 * -- @name Binding.JOYSTICK
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Trigger when joystick axis is used.
 * -- @name Binding.JOYSTICK_AXIS
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Removes the binding.
 * --
 * -- @param self Binding.
 * function Binding.free(self)
 */
static int
Binding_free (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_BINDING);

	liscr_data_unref (self, NULL);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new binding.
 * --
 * -- @param self Binding class.
 * -- @param type Binding type.
 * -- @param action Bound action.
 * -- @param params Free form string passed to the action handler.
 * -- @param code Key code, mouse button, mouse axis, joystick button, or joystick axis.
 * -- @param mods Modifier mask.
 * -- @param multiplier Values will be premultiplied by this before passing to the handler.
 * -- @return New Binding.
 * function Binding.new(type, action, params, code, mods, multiplier)
 */
static int
Binding_new (lua_State* lua)
{
	int type;
	int code;
	int mods;
	float mult;
	const char* params;
	libndBinding* binding;
	liscrData* self;
	liscrData* action;
	liscrScript* script = liscr_script (lua);
	licliModule* module = liscr_script_get_userdata (script);

	/* Check arguments. */
	type = luaL_checkint (lua, 2);
	action = liscr_checkdata (lua, 3, LICLI_SCRIPT_ACTION);
	params = luaL_checkstring (lua, 4);
	code = luaL_checkint (lua, 5);
	mods = luaL_checkint (lua, 6);
	if (!lua_isnone (lua, 7))
		mult = luaL_checknumber (lua, 7);
	else
		mult = 1.0f;
	luaL_argcheck (lua, type >= 0 && type < LIBND_TYPE_MAX, 2, "invalid binding type");

	/* Allocate userdata. */
	binding = libnd_binding_new (module->bindings, type, action->data, params, code, mods, mult);
	if (binding == NULL)
		return 0;
	self = liscr_data_new (script, binding, LICLI_SCRIPT_BINDING, libnd_binding_free);
	if (self == NULL)
	{
		libnd_binding_free (binding);
		return 0;
	}
	libnd_binding_set_userdata (binding, self);
	liscr_data_ref (action, self);
	liscr_pushdata (lua, self);

	return 1;
}

/*****************************************************************************/

void
licliBindingScript (liscrClass* self,
                    void*       data)
{
	liscr_class_insert_func (self, "new", Binding_new);
	liscr_class_insert_func (self, "free", Binding_free);
	liscr_class_insert_enum (self, "KEYBOARD", LIBND_TYPE_KEYBOARD);
	liscr_class_insert_enum (self, "MOUSE", LIBND_TYPE_MOUSE);
	liscr_class_insert_enum (self, "MOUSE_AXIS", LIBND_TYPE_MOUSE_AXIS);
	liscr_class_insert_enum (self, "MOUSE_DELTA", LIBND_TYPE_MOUSE_DELTA);
	liscr_class_insert_enum (self, "JOYSTICK", LIBND_TYPE_JOYSTICK);
	liscr_class_insert_enum (self, "JOYSTICK_AXIS", LIBND_TYPE_JOYSTICK_AXIS);
}

/** @} */
/** @} */
