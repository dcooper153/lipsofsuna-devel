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
 * \addtogroup liextsrvInventory Inventory
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-inventory.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Inventory"
 * ---
 * -- Synchronize objects automatically with clients.
 * -- @name Inventory
 * -- @class table
 */

static int
Inventory___index (lua_State* lua)
{
	int slot;
	int size;
	liengObject* object;
	liscrData* self;

	/* Try slots first. */
	if (lua_isnumber (lua, 2))
	{
		self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
		slot = luaL_checkinteger (lua, 2);
		size = liext_inventory_get_size (self->data);
		luaL_argcheck (lua, slot >= 1 && slot <= size, 2, "inventory slot out of bounds");
		object = liext_inventory_get_object (self->data, slot - 1);
		if (object != NULL && object->script != NULL)
			liscr_pushdata (lua, object->script);
		else
			lua_pushnil (lua);
		return 1;
	}

	return liscr_class_default___index (lua);
}

static int
Inventory___newindex (lua_State* lua)
{
	int slot;
	int size;
	liscrData* object;
	liscrData* self;

	/* Try slots first. */
	if (lua_isnumber (lua, 2))
	{
		self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
		slot = luaL_checkinteger (lua, 2);
		size = liext_inventory_get_size (self->data);
		luaL_argcheck (lua, slot >= 1 && slot <= size, 2, "inventory slot out of bounds");
		if (!lua_isnil (lua, 3))
		{
			object = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);
			liext_inventory_set_object (self->data, slot - 1, object->data);
			lieng_object_set_realized (object->data, 0);
		}
		else
			liext_inventory_set_object (self->data, slot - 1, NULL);
		return 1;
	}

	return liscr_class_default___newindex (lua);
}

/* @luadoc
 * ---
 * -- Finds an inventory by ID.
 * --
 * -- @param self Inventory class.
 * -- @param id Inventory number.
 * -- @return Inventory or nil.
 * function Inventory.find(self, id)
 */
static int
Inventory_find (lua_State* lua)
{
	int slot;
	liextInventory* inventory;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
	slot = luaL_checkinteger (lua, 2);

	inventory = liext_module_find_inventory (self, slot);
	if (inventory == NULL)
		return 0;
	liscr_pushdata (lua, inventory->script);

	return 1;
}

/* @luadoc
 * ---
 * -- Creates a new inventory.
 * --
 * -- @param self Inventory class.
 * -- @param table Optional table of arguments.
 * -- @return New inventory.
 * function Inventory.new(self, table)
 */
static int
Inventory_new (lua_State* lua)
{
	liextInventory* inventory;
	liextModule* module;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_INVENTORY);

	/* Allocate self. */
	inventory = liext_inventory_new (module);
	if (inventory == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, inventory, LIEXT_SCRIPT_INVENTORY, liext_inventory_free);
	if (self == NULL)
	{
		liext_inventory_free (inventory);
		lua_pushnil (lua);
		return 1;
	}
	inventory->script = self;

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Subscribes an object to the inventory.
 * --
 * -- @param self Inventory.
 * -- @param object Object to subscribe.
 * function Inventory.subscribe(self, object)
 */
static int
Inventory_subscribe (lua_State* lua)
{
	liscrData* object;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);

	liext_inventory_insert_listener (self->data, object->data);

	return 1;
}

/* @luadoc
 * ---
 * -- Checks if an object is subscribed to the inventory.
 * --
 * -- @param self Inventory.
 * -- @param object Object.
 * -- @return True if subscribed.
 * function Inventory.subscribed(self, object)
 */
static int
Inventory_subscribed (lua_State* lua)
{
	liscrData* object;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);

	lua_pushboolean (lua, liext_inventory_find_listener (self->data,
		LIENG_OBJECT (object->data)->id) != NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Unsubscribes an object from the inventory.
 * --
 * -- @param self Inventory.
 * -- @param object Object to unsubscribe.
 * function Inventory.unsubscribe(self, object)
 */
static int
Inventory_unsubscribe (lua_State* lua)
{
	liscrData* object;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
	object = liscr_checkdata (lua, 2, LICOM_SCRIPT_OBJECT);

	liext_inventory_remove_listener (self->data, object->data);

	return 1;
}

/* @luadoc
 * ---
 * -- Index of the first free inventory slot or nil if full.
 * --
 * -- @name Inventory.first_free_slot
 * -- @class table
 */
static int
Inventory_getter_first_free_slot (lua_State* lua)
{
	int i;
	int size;
	liscrData* self;
	liengObject* object;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
	size = liext_inventory_get_size (self->data);

	for (i = 0 ; i < size ; i++)
	{
		object = liext_inventory_get_object (self->data, i);
		if (object == NULL)
		{
			lua_pushnumber (lua, i + 1);
			return 1;
		}
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Unique ID of the inventory.
 * --
 * -- @name Inventory.id
 * -- @class table
 */
static int
Inventory_getter_id (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);

	lua_pushnumber (lua, liext_inventory_get_id (self->data));
	return 1;
}

/* @luadoc
 * ---
 * -- Custom callback called when an item is added or changed.
 * --
 * -- Arguments passed to the callback: inventory, listener, slot, object.
 * --
 * -- @name Inventory.item_added_cb
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Custom callback called when an item is removed.
 * --
 * -- Arguments passed to the callback: inventory, listener, slot.
 * --
 * -- @name Inventory.item_removed_cb
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Size of the inventory.
 * --
 * -- If the number of slots is reduced and there are objects in
 * -- the removed slots, the objects in question are destroyed.
 * --
 * -- @name Inventory.size
 * -- @class table
 */
static int
Inventory_getter_size (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);

	lua_pushnumber (lua, liext_inventory_get_size (self->data));
	return 1;
}
static int
Inventory_setter_size (lua_State* lua)
{
	int size;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_INVENTORY);
	size = luaL_checkinteger (lua, 3);
	luaL_argcheck (lua, size >= 0, 3, "invalid inventory slot count");

	liext_inventory_set_size (self->data, size);
	return 0;
}

/* @luadoc
 * ---
 * -- Custom callback called when a user is added.
 * --
 * -- Arguments passed to the callback: inventory, object.
 * --
 * -- @name Inventory.user_added_cb
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Custom callback called when a user is removed.
 * --
 * -- Arguments passed to the callback: inventory, object.
 * --
 * -- @name Inventory.user_removed_cb
 * -- @class table
 */

/*****************************************************************************/

void
liextInventoryScript (liscrClass* self,
                      void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_INVENTORY, data);
	liscr_class_insert_func (self, "__index", Inventory___index);
	liscr_class_insert_func (self, "__newindex", Inventory___newindex);
	liscr_class_insert_func (self, "find", Inventory_find);
	liscr_class_insert_func (self, "new", Inventory_new);
	liscr_class_insert_func (self, "subscribe", Inventory_subscribe);
	liscr_class_insert_func (self, "subscribed", Inventory_subscribed);
	liscr_class_insert_func (self, "unsubscribe", Inventory_unsubscribe);
	liscr_class_insert_getter (self, "first_free_slot", Inventory_getter_first_free_slot);
	liscr_class_insert_getter (self, "id", Inventory_getter_id);
	liscr_class_insert_getter (self, "size", Inventory_getter_size);
	liscr_class_insert_setter (self, "size", Inventory_setter_size);
}

/** @} */
/** @} */
/** @} */
