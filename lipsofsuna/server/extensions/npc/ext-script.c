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
 * -- Create intelligent non-player characters.
 * -- @name Npc
 * -- @class table
 */

/* @luadoc
 * ---
 * -- @brief Finds the NPC logic for an object.
 * -- @param self Npc class.
 * -- @param object Object whose logic to find.
 * -- @return Npc or nil.
 * function Npc.find(self, object)
 */
static int
Npc_find (lua_State* lua)
{
	liextModule* module;
	liextNpc* npc;
	liscrData* object;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_NPC);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);

	/* Find logic. */
	npc = liext_module_find_npc (module, object->data);
	if (npc == NULL)
		return 0;
	liscr_pushdata (lua, npc->script);

	return 1;
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
	logic = liext_npc_new (module);
	if (logic == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, logic, LIEXT_SCRIPT_NPC, liext_npc_free);
	if (self == NULL)
	{
		liext_npc_free (logic);
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
 * -- Solves the path to a point for the NPC logic.
 * --
 * -- @paran self Npc.
 * -- @param vector Vector.
 * -- @return Path or nil.
 * function Npc.solve_path(self, vector)
 */
static int
Npc_solve_path (lua_State* lua)
{
	liaiPath* tmp;
	liextNpc* self;
	liscrData* path;
	liscrData* vector;
	liscrScript* script = liscr_script (lua);

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;
	vector = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	/* Solve path. */
	if (self->object == NULL)
		return 0;
	tmp = liext_module_solve_path (self->module, self->object, vector->data);
	if (tmp == NULL)
		return 0;

	/* Create path object. */
	path = liscr_data_new (script, tmp, LICOM_SCRIPT_PATH, liai_path_free);
	if (path == NULL)
	{
		liai_path_free (tmp);
		return 0;
	}

	liscr_pushdata (lua, path);
	liscr_data_unref (path, NULL);

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
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;

	lua_pushboolean (lua, self->alert);

	return 1;
}
static int
Npc_setter_alert (lua_State* lua)
{
	int value;
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;
	value = lua_toboolean (lua, 3);

	self->alert = (value != 0);

	return 0;
}

/* @luadoc
 * ---
 * -- Called when the controlled object is near the target.
 * --
 * -- Arguments passed to the callback: npc, object, target.
 * --
 * -- @name Npc.attack_cb
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Controlled object.
 * -- @name Npc.owner
 * -- @class table
 */
static int
Npc_getter_owner (lua_State* lua)
{
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;

	if (self->object == NULL || self->object->script == NULL)
		return 0;
	liscr_pushdata (lua, self->object->script);

	return 1;
}
static int
Npc_setter_owner (lua_State* lua)
{
	liengObject* object;
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;
	if (!lua_isnoneornil (lua, 3))
	{
		object = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT)->data;
		luaL_argcheck (lua,
			lialg_ptrdic_find (self->module->dictionary, object) == NULL,
			3, "object already has a logic attached");
	}
	else
		object = NULL;

	liext_npc_set_object (self, object);

	return 0;
}

/* @luadoc
 * ---
 * -- Sets the path for the object to traverse.
 * -- @name Npc.path
 * -- @class table
 */
static int
Npc_getter_path (lua_State* lua)
{
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;

	if (self->path == NULL)
		return 0;
	liscr_pushdata (lua, self->path);

	return 1;
}
static int
Npc_setter_path (lua_State* lua)
{
	liextNpc* self;
	liscrData* path;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;
	if (!lua_isnoneornil (lua, 3))
		path = liscr_checkdata (lua, 3, LICOM_SCRIPT_PATH);
	else
		path = NULL;

	liext_npc_set_path (self, path);

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
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;

	lua_pushnumber (lua, self->radius);

	return 1;
}
static int
Npc_setter_radius (lua_State* lua)
{
	float value;
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;
	value = luaL_checknumber (lua, 3);
	luaL_argcheck (lua, value >= 0.0f, 3, "negative radius");

	self->radius = value;

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
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;

	lua_pushnumber (lua, self->refresh);

	return 1;
}
static int
Npc_setter_refresh (lua_State* lua)
{
	float value;
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;
	value = luaL_checknumber (lua, 3);
	luaL_argcheck (lua, value >= 0.0f, 3, "negative refresh");

	self->refresh = value;

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
	liextNpc* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;

	if (self->target == NULL)
		return 0;

	liscr_pushdata (lua, self->target->script);

	return 1;
}
static int
Npc_setter_target (lua_State* lua)
{
	liextNpc* self;
	liengObject* object;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_NPC)->data;
	if (!lua_isnoneornil (lua, 3))
	{
		object = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT)->data;
		luaL_argcheck (lua, lieng_object_get_realized (object), 3, "object is not realized");
	}
	else
		object = NULL;

	liext_npc_set_target (self, object);

	return 0;
}

/*****************************************************************************/

void
liextNpcScript (liscrClass* self,
                void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_NPC, data);
	liscr_class_insert_func (self, "find", Npc_find);
	liscr_class_insert_func (self, "new", Npc_new);
	liscr_class_insert_func (self, "solve_path", Npc_solve_path);
	liscr_class_insert_getter (self, "alert", Npc_getter_alert);
	liscr_class_insert_getter (self, "owner", Npc_getter_owner);
	liscr_class_insert_getter (self, "path", Npc_getter_path);
	liscr_class_insert_getter (self, "radius", Npc_getter_radius);
	liscr_class_insert_getter (self, "refresh", Npc_getter_refresh);
	liscr_class_insert_getter (self, "target", Npc_getter_target);
	liscr_class_insert_setter (self, "alert", Npc_setter_alert);
	liscr_class_insert_setter (self, "owner", Npc_setter_owner);
	liscr_class_insert_setter (self, "path", Npc_setter_path);
	liscr_class_insert_setter (self, "radius", Npc_setter_radius);
	liscr_class_insert_setter (self, "refresh", Npc_setter_refresh);
	liscr_class_insert_setter (self, "target", Npc_setter_target);
}

/** @} */
/** @} */
/** @} */
