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

static int
private_action_callback (libndAction*  action,
                         libndBinding* binding,
                         float         value,
                         void*         data)
{
	liscrData* event;
	liscrScript* script = liscr_data_get_script (data);

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
	event = licom_event_newva (script,
		"binding", LICLI_SCRIPT_BINDING, libnd_binding_get_userdata (binding),
		"action", LICLI_SCRIPT_ACTION, libnd_action_get_userdata (action),
		"active", LISCR_TYPE_BOOLEAN, value != 0.0f,
		"value", LISCR_TYPE_FLOAT, value,
		"params", LISCR_TYPE_STRING, binding->params, NULL);
	if (event == NULL)
	{
		lua_pop (script->lua, 1);
		return 1;
	}
	licom_event_set_type (event, LICLI_EVENT_TYPE_ACTION);

	/* Call callback. */
	liscr_pushdata (script->lua, event);
	if (lua_pcall (script->lua, 1, 0, 0))
	{
		printf ("ERROR: %s.\n", lua_tostring (script->lua, -1));
		lua_pop (script->lua, 1);
	}

	/* Free event. */
	liscr_data_unref (event, NULL);

	return 1;
}

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Action"
 * ---
 * -- Specify the actions and controls of players.
 * -- @name Action
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Removes the action.
 * --
 * -- It is an error to free an action still used by bindings.
 * --
 * -- @param self Action.
 * function Action.free(self)
 */
static int
Action_free (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_ACTION);

	liscr_data_unref (self, NULL);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new action.
 * --
 * -- @param self Action class.
 * -- @param id Identifier string to be used internally.
 * -- @param name Human readable short name.
 * -- @param desc Human readable long description.
 * -- @param func Callback function.
 * -- @return New action.
 * function Action.new(id, name, desc, func)
 */
static int
Action_new (lua_State* lua)
{
	const char* id;
	const char* name;
	const char* desc;
	libndAction* action;
	licliModule* module;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_ACTION);

	/* Check arguments. */
	id = luaL_checkstring (lua, 2);
	name = luaL_checkstring (lua, 3);
	desc = luaL_checkstring (lua, 4);

	/* Allocate userdata. */
	action = libnd_action_new (module->bindings, id, name, desc, private_action_callback, NULL);
	if (action == NULL)
		return 0;
	self = liscr_data_new (script, action, LICLI_SCRIPT_ACTION, libnd_action_free);
	if (self == NULL)
	{
		libnd_action_free (action);
		return 0;
	}
	libnd_action_set_userdata (action, self);
	liscr_pushdata (lua, self);

	return 1;
}

/* @luadoc
 * ---
 * -- Enables or disables the action.
 * --
 * -- @name Action.enabled
 * -- @class table
 */
static int
Action_getter_enabled (lua_State* lua)
{
	liscrData* action;

	action = liscr_checkdata (lua, 1, LICLI_SCRIPT_ACTION);

	lua_pushboolean (lua, libnd_action_get_enabled (action->data));
	return 1;
}
static int
Action_setter_enabled (lua_State* lua)
{
	int value;
	liscrData* action;

	action = liscr_checkdata (lua, 1, LICLI_SCRIPT_ACTION);
	value = lua_toboolean (lua, 3);

	libnd_action_set_enabled (action->data, value);
	return 0;
}

/*****************************************************************************/

void
licliActionScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_ACTION, data);
	liscr_class_insert_func (self, "new", Action_new);
	liscr_class_insert_func (self, "free", Action_free);
	liscr_class_insert_getter (self, "enabled", Action_getter_enabled);
	liscr_class_insert_setter (self, "enabled", Action_setter_enabled);
}

/** @} */
/** @} */
