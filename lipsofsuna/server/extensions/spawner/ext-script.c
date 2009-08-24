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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvSpawner Spawner
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-spawner.h"

/* @luadoc
 * module "Extension.Server.Spawner"
 * ---
 * -- Spawn objects periodically.
 * -- @name Spawner
 * -- @class table
 */

static int
Spawner___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SPAWNER);

	liext_spawner_free (self->data);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new spawner logic.
 * --
 * -- @param self Spawner class.
 * -- @param table Optional table of arguments.
 * -- @return New spawner.
 * function Spawner.new(self, table)
 */
static int
Spawner_new (lua_State* lua)
{
	liextModule* module;
	liextSpawner* logic;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SPAWNER);

	/* Allocate self. */
	logic = liext_spawner_new (module);
	if (logic == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, logic, LIEXT_SCRIPT_SPAWNER);
	if (self == NULL)
	{
		liext_spawner_free (logic);
		lua_pushnil (lua);
		return 1;
	}
	logic->script = self;

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Spawn delay.
 * -- @name Spawner.delay
 * -- @class table
 */
static int
Spawner_getter_delay (lua_State* lua)
{
	liscrData* self;
	liextSpawner* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SPAWNER);
	data = self->data;

	lua_pushnumber (lua, data->delay);
	return 1;
}
static int
Spawner_setter_delay (lua_State* lua)
{
	float value;
	liscrData* self;
	liextSpawner* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SPAWNER);
	data = self->data;
	value = lua_tonumber (lua, 3);
	luaL_argcheck (lua, value >= 0.0, 3, "negative delay");

	data->delay = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Maximum number of spawned objects.
 * --
 * -- If the current number of spawned objects is higher than the limit,
 * -- the spawner will disown objects until the count is equal to the limit.
 * -- The order of disowning is undefined.
 * --
 * -- @name Spawner.limit
 * -- @class table
 */
static int
Spawner_getter_limit (lua_State* lua)
{
	liscrData* self;
	liextSpawner* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SPAWNER);
	data = self->data;

	lua_pushnumber (lua, data->limit);
	return 1;
}
static int
Spawner_setter_limit (lua_State* lua)
{
	int value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SPAWNER);
	value = (int) luaL_checknumber (lua, 3);
	luaL_argcheck (lua, value >= 0, 3, "negative limit");

	liext_spawner_set_limit (self->data, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Controlled object.
 * -- @name Spawner.object
 * -- @class table
 */
static int
Spawner_getter_object (lua_State* lua)
{
	liscrData* self;
	liextSpawner* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SPAWNER);
	data = self->data;

	if (data->object != NULL && data->object->script != NULL)
		liscr_pushdata (lua, data->object->script);
	else
		lua_pushnil (lua);
	return 1;
}
static int
Spawner_setter_object (lua_State* lua)
{
	liscrData* self;
	liscrData* tmp;
	liengObject* object;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SPAWNER);
	if (!lua_isnil (lua, 3))
	{
		tmp = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);
		object = tmp->data;
	}
	else
		object = NULL;

	liext_spawner_set_object (self->data, object);
	return 0;
}

/* @luadoc
 * ---
 * -- Spawn function.
 * --
 * -- The object controlled by the spawner is passed to the function as an
 * -- argument whenever there's a need to spawn an object. The function is
 * -- expected to return a new object or nil if nothing should be spawned.
 * --
 * -- @name Spawner.callback
 * -- @class table
 */

/*****************************************************************************/

void
liextSpawnerScript (liscrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SPAWNER, data);
	liscr_class_insert_func (self, "__gc", Spawner___gc);
	liscr_class_insert_func (self, "new", Spawner_new);
	liscr_class_insert_getter (self, "delay", Spawner_getter_delay);
	liscr_class_insert_getter (self, "limit", Spawner_getter_limit);
	liscr_class_insert_getter (self, "object", Spawner_getter_object);
	liscr_class_insert_setter (self, "delay", Spawner_setter_delay);
	liscr_class_insert_setter (self, "limit", Spawner_setter_limit);
	liscr_class_insert_setter (self, "object", Spawner_setter_object);
}

/** @} */
/** @} */
/** @} */
