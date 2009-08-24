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
 * \addtogroup liextsrvCreature Creature
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-creature.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Creature"
 * ---
 * -- Make your objects move like creatures.
 * -- @name Creature
 * -- @class table
 */

static int
Creature___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LIEXT_SCRIPT_CREATURE);

	liext_creature_free (self->data);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new creature logic.
 * --
 * -- @param self Creature class.
 * -- @param table Optional table of arguments.
 * -- @return New creature logic.
 * function Creature.new(self, table)
 */
static int
Creature_new (lua_State* lua)
{
	liextCreature* logic;
	liextModule* module;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_CREATURE);

	/* Allocate self. */
	logic = liext_creature_new (module->server);
	if (logic == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, logic, LIEXT_SCRIPT_CREATURE);
	if (self == NULL)
	{
		liext_creature_free (logic);
		lua_pushnil (lua);
		return 1;
	}
	logic->data = self;

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Control state.
 * -- @name Creature.controls
 * -- @class table
 */
static int
Creature_getter_controls (lua_State* lua)
{
	liscrData* self;
	liextCreature* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_CREATURE);
	data = self->data;

	lua_pushnumber (lua, data->controls);
	return 1;
}
static int
Creature_setter_controls (lua_State* lua)
{
	int value;
	liscrData* self;
	liextCreature* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_CREATURE);
	data = self->data;
	value = (int) luaL_checknumber (lua, 3);

	data->controls = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Controlled object.
 * -- @name Creature.object
 * -- @class table
 */
static int
Creature_getter_object (lua_State* lua)
{
	liscrData* self;
	liextCreature* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_CREATURE);
	data = self->data;

	if (data->object != NULL && data->object->script != NULL)
		liscr_pushdata (lua, data->object->script);
	else
		lua_pushnil (lua);
	return 1;
}
static int
Creature_setter_object (lua_State* lua)
{
	liscrData* self;
	liscrData* tmp;
	liengObject* object;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_CREATURE);
	if (!lua_isnil (lua, 3))
	{
		tmp = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);
		object = tmp->data;
	}
	else
		object = NULL;

	liext_creature_set_object (self->data, object);
	return 0;
}

/*****************************************************************************/

void
liextCreatureScript (liscrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_CREATURE, data);
	liscr_class_insert_func (self, "__gc", Creature___gc);
	liscr_class_insert_func (self, "new", Creature_new);
	liscr_class_insert_getter (self, "controls", Creature_getter_controls);
	liscr_class_insert_getter (self, "object", Creature_getter_object);
	liscr_class_insert_setter (self, "controls", Creature_setter_controls);
	liscr_class_insert_setter (self, "object", Creature_setter_object);
}

/** @} */
/** @} */
/** @} */
