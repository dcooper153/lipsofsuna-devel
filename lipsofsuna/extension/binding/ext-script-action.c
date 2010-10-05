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

static int
private_action_callback (LIBndAction*  action,
                         LIBndBinding* binding,
                         float         value,
                         void*         data)
{
	LIScrData* event;
	LIScrScript* script = liscr_data_get_script (data);

	/* Check for callback. */
	liscr_pushdata (script->lua, libnd_action_get_userdata (action));
	lua_getfield (script->lua, -1, "callback");
	lua_remove (script->lua, -2);
	if (!lua_isfunction (script->lua, -1))
	{
		lua_pop (script->lua, 1);
		return 1;
	}

	/* Create event. */
	event = liscr_event_newva (script,
		"binding", LIEXT_SCRIPT_BINDING, libnd_binding_get_userdata (binding),
		"action", LIEXT_SCRIPT_ACTION, libnd_action_get_userdata (action),
		"active", LISCR_TYPE_BOOLEAN, value != 0.0f,
		"value", LISCR_TYPE_FLOAT, value,
		"params", LISCR_TYPE_STRING, binding->params, NULL);
	if (event == NULL)
	{
		lua_pop (script->lua, 1);
		return 1;
	}
	liscr_event_set_type (event, "action");

	/* Call callback. */
	liscr_pushdata (script->lua, event);
	if (lua_pcall (script->lua, 1, 0, 0))
	{
		lisys_error_set (EINVAL, lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}

	/* Free event. */
	liscr_data_unref (event, NULL);

	return 1;
}

/*****************************************************************************/

/* @luadoc
 * module "Extension.Binding"
 * ---
 * -- Specify the actions and controls of players.
 * -- @name Action
 * -- @class table
 */

/* @luadoc
 * --- Removes the action.
 * --
 * -- @param self Action.
 * function Action.free(self)
 */
static void Action_free (LIScrArgs* args)
{
	if (args->data->refcount)
		liscr_data_unref (args->data, NULL);
}

/* @luadoc
 * --- Creates a new action.
 * --
 * -- @param self Action class.
 * -- @param args Arguments.<ul>
 * --   <li>id: Identifier string to be used internally.</li>
 * --   <li>name: Human readable short name.</li>
 * --   <li>desc: Human readable long description.</li>
 * --   <li>func: Callback function.</li></ul>
 * -- @return New action.
 * function Action.new(self, args)
 */
static void Action_new (LIScrArgs* args)
{
	const char* id;
	const char* name = "";
	const char* desc = "";
	LIBndAction* self;
	LIExtModule* module;
	LIScrData* data;

	/* Check arguments. */
	if (!liscr_args_gets_string (args, "id", &id))
		return;
	liscr_args_gets_string (args, "name", &name);
	liscr_args_gets_string (args, "desc", &desc);

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_ACTION);
	self = libnd_action_new (module->bindings, id, name, desc, private_action_callback, NULL);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, args->clss, libnd_action_free);
	if (data == NULL)
	{
		libnd_action_free (self);
		return;
	}
	libnd_action_set_userdata (self, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
}

/* @luadoc
 * --- Enables or disables the action.
 * --
 * -- @name Action.enabled
 * -- @class table
 */
static void Action_getter_enabled (LIScrArgs* args)
{
	liscr_args_seti_bool (args, libnd_action_get_enabled (args->self));
}
static void Action_setter_enabled (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		libnd_action_set_enabled (args->self, value);
}

/*****************************************************************************/

void liext_script_action (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_ACTION, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Action_new);
	liscr_class_insert_mfunc (self, "free", Action_free);
	liscr_class_insert_mvar (self, "enabled", Action_getter_enabled, Action_setter_enabled);
}

/** @} */
/** @} */
