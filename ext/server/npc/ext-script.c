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
 * \addtogroup liextsrvNpc Npc
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-npc.h"

/* @luadoc
 * module "Extension.Server.Npc"
 * ---
 * -- Create non-player characters.
 * -- @name Inventory
 * -- @class table
 */

static int
Npc___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LIEXT_SCRIPT_NPC);

	liext_npc_free (self->data);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new non-player character logic.
 * --
 * -- @param self Npc class.
 * -- @param table Optional table of arguments.
 * -- @return New NPC.
 * function Npc.new(self, table)
 */
static int
Npc_new (lua_State* lua)
{
	liextModule* module;
	liextNpc* logic;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_NPC);

	/* Allocate self. */
	logic = liext_npc_new (module->server);
	if (logic == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, logic, LIEXT_SCRIPT_NPC);
	if (self == NULL)
	{
		liext_npc_free (logic);
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
 * -- Alertness flag.
 * -- @name Npc.alert
 * -- @class table
 */
static int
Npc_getter_alert (lua_State* lua)
{
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;

	lua_pushboolean (lua, data->alert);
	return 1;
}
static int
Npc_setter_alert (lua_State* lua)
{
	int value;
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;
	value = lua_toboolean (lua, 3);

	data->alert = (value != 0);
	return 0;
}

/* @luadoc
 * ---
 * -- Controlled object.
 * -- @name Npc.object
 * -- @class table
 */
static int
Npc_getter_object (lua_State* lua)
{
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;

	if (data->object != NULL && data->object->script != NULL)
		liscr_pushdata (lua, data->object->script);
	else
		lua_pushnil (lua);
	return 1;
}
static int
Npc_setter_object (lua_State* lua)
{
	liscrData* self;
	liscrData* tmp;
	liengObject* object;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	if (!lua_isnil (lua, 3))
	{
		tmp = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);
		object = tmp->data;
	}
	else
		object = NULL;

	liext_npc_set_object (self->data, object);
	return 0;
}

/* @luadoc
 * ---
 * -- Target scanning radius.
 * -- @name Npc.radius
 * -- @class table
 */
static int
Npc_getter_radius (lua_State* lua)
{
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;

	lua_pushnumber (lua, data->radius);
	return 1;
}
static int
Npc_setter_radius (lua_State* lua)
{
	float value;
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;
	value = luaL_checknumber (lua, 3);
	luaL_argcheck (lua, value >= 0.0f, 3, "negative radius");

	data->radius = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Refresh delay.
 * -- @name Npc.refresh
 * -- @class table
 */
static int
Npc_getter_refresh (lua_State* lua)
{
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;

	lua_pushnumber (lua, data->refresh);
	return 1;
}
static int
Npc_setter_refresh (lua_State* lua)
{
	float value;
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;
	value = luaL_checknumber (lua, 3);
	luaL_argcheck (lua, value >= 0.0f, 3, "negative refresh");

	data->refresh = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Targeted object.
 * -- @name Npc.target
 * -- @class table
 */
static int
Npc_getter_target (lua_State* lua)
{
	liscrData* self;
	liextNpc* data;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;

	if (data->target != NULL)
		liscr_pushdata (lua, data->target->script);
	else
		lua_pushnil (lua);
	return 1;
}
static int
Npc_setter_target (lua_State* lua)
{
	liscrData* self;
	liscrData* object;
	liextNpc* data;
	liengObject* tmp;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC);
	data = self->data;
	object = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);
	tmp = object->data;
	luaL_argcheck (lua, tmp->sector != NULL, 3, "object not in any sector");

	liext_npc_set_target (data, tmp);
	return 0;
}

/*****************************************************************************/

void
liextNpcScript (liscrClass* self,
                void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_NPC, data);
	liscr_class_insert_func (self, "__gc", Npc___gc);
	liscr_class_insert_func (self, "new", Npc_new);
	liscr_class_insert_getter (self, "alert", Npc_getter_alert);
	liscr_class_insert_getter (self, "object", Npc_getter_object);
	liscr_class_insert_getter (self, "radius", Npc_getter_radius);
	liscr_class_insert_getter (self, "refresh", Npc_getter_refresh);
	liscr_class_insert_getter (self, "target", Npc_getter_target);
	liscr_class_insert_setter (self, "alert", Npc_setter_alert);
	liscr_class_insert_setter (self, "object", Npc_setter_object);
	liscr_class_insert_setter (self, "radius", Npc_setter_radius);
	liscr_class_insert_setter (self, "refresh", Npc_setter_refresh);
	liscr_class_insert_setter (self, "target", Npc_setter_target);
}

/** @} */
/** @} */
/** @} */
