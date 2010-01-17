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
 * \addtogroup LIExtInventory Inventory
 * @{
 */

#include <lipsofsuna/main.h>
#include <lipsofsuna/server.h>
#include "ext-inventory.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Inventory"
 * ---
 * -- Synchronize objects automatically with clients.
 * -- @name Inventory
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Finds an inventory.
 * --
 * -- Arguments:
 * -- id: Inventory number.
 * -- owner: Owner object.
 * --
 * -- @param self Inventory class.
 * -- @param args Arguments.
 * -- @return Inventory or nil.
 * function Inventory.find(self, args)
 */
static void Inventory_find (LIScrArgs* args)
{
	int id;
	LIScrData* data;
	LIExtInventory* inventory;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_INVENTORY);
	if (liscr_args_gets_int (args, "id", &id))
	{
		inventory = liext_module_find_inventory (module, id);
		if (inventory != NULL)
			liscr_args_seti_data (args, inventory->script);
	}
	else if (liscr_args_gets_data (args, "owner", LISCR_SCRIPT_OBJECT, &data))
	{
		inventory = lialg_ptrdic_find (module->ptrdic, data->data);
		if (inventory != NULL)
			liscr_args_seti_data (args, inventory->script);
	}
}

/* @luadoc
 * ---
 * -- Creates a new inventory.
 * --
 * -- @param self Inventory class.
 * -- @param args Arguments.
 * -- @return New inventory.
 * function Inventory.new(self, args)
 */
static void Inventory_new (LIScrArgs* args)
{
	LIExtInventory* self;
	LIExtModule* module;
	LIScrData* data;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_INVENTORY);
	self = liext_inventory_new (module);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_INVENTORY, liext_inventory_free);
	if (data == NULL)
	{
		liext_inventory_free (self);
		return;
	}
	self->script = data;
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * Gets the object in a slot.
 *
 * Arguments:
 * slot: Slot index.
 *
 * @param self Inventory.
 * @param args Arguments.
 * @return Object or nil.
 */
static void Inventory_get_object (LIScrArgs* args)
{
	int slot;
	int size;
	LIEngObject* object;

	if (liscr_args_gets_int (args, "slot", &slot))
	{
		size = liext_inventory_get_size (args->self);
		if (slot >= 0 && slot <= size)
		{
			object = liext_inventory_get_object (args->self, slot - 1);
			if (object != NULL)
				liscr_args_seti_data (args, object->script);
		}
	}
}

/* @luadoc
 * Sets the object in a slot.
 *
 * Arguments:
 * slot: Slot index. (required)
 * object: Object.
 *
 * @param self Inventory.
 * @param args Arguments.
 * @return Object or nil.
 */
static void Inventory_set_object (LIScrArgs* args)
{
	int slot;
	int size;
	LIScrData* data;

	if (liscr_args_gets_int (args, "slot", &slot))
	{
		size = liext_inventory_get_size (args->self);
		if (slot >= 0 && slot <= size)
		{
			if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
			{
				liext_inventory_set_object (args->self, slot - 1, data->data);
				lieng_object_set_realized (data->data, 0);
			}
			else
				liext_inventory_set_object (args->self, slot - 1, NULL);
		}
	}
}

/* @luadoc
 * ---
 * -- Subscribes an object to the inventory.
 * --
 * -- Arguments:
 * -- object: Object to subscribe.
 * --
 * -- @param self Inventory.
 * -- @param args Arguments.
 * function Inventory.subscribe(self, args)
 */
static void Inventory_subscribe (LIScrArgs* args)
{
	LIScrData* data;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
		liext_inventory_insert_listener (args->self, data->data);
}

/* @luadoc
 * ---
 * -- Checks if an object is subscribed to the inventory.
 * --
 * -- Arguments:
 * -- object: Object to subscribe.
 * --
 * -- @param self Inventory.
 * -- @param args Arguments.
 * -- @return True if subscribed.
 * function Inventory.subscribed(self, args)
 */
static void Inventory_subscribed (LIScrArgs* args)
{
	LIScrData* data;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
	{
		liscr_args_seti_bool (args, liext_inventory_find_listener (
			args->self, LIENG_OBJECT (data->data)->id) != NULL);
	}
}

/* @luadoc
 * ---
 * -- Unsubscribes an object from the inventory.
 * --
 * -- Arguments:
 * -- object: Object to subscribe.
 * --
 * -- @param self Inventory.
 * -- @param args Arguments.
 * function Inventory.unsubscribe(self, args)
 */
static void Inventory_unsubscribe (LIScrArgs* args)
{
	LIScrData* data;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
		liext_inventory_remove_listener (args->self, data->data);
}

/* @luadoc
 * ---
 * -- Index of the first free inventory slot or nil if full.
 * --
 * -- @name Inventory.first_free_slot
 * -- @class table
 */
static void Inventory_getter_first_free_slot (LIScrArgs* args)
{
	int i;
	int size;
	LIEngObject* object;

	size = liext_inventory_get_size (args->self);
	for (i = 0 ; i < size ; i++)
	{
		object = liext_inventory_get_object (args->self, i);
		if (object == NULL)
		{
			liscr_args_seti_int (args, i + 1);
			break;
		}
	}
}

/* @luadoc
 * ---
 * -- Unique ID of the inventory.
 * --
 * -- @name Inventory.id
 * -- @class table
 */
static void Inventory_getter_id (LIScrArgs* args)
{
	liscr_args_seti_int (args, liext_inventory_get_id (args->self));
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
 * -- The object that owns the inventory.
 * --
 * -- Used by Inventory.find when searching by owner.
 * --
 * -- @name Inventory.owner
 * -- @class table
 */
static void Inventory_getter_owner (LIScrArgs* args)
{
	LIEngObject* object;

	object = liext_inventory_get_owner (args->self);
	if (object != NULL)
		liscr_args_seti_data (args, object->script);
}
static void Inventory_setter_owner (LIScrArgs* args)
{
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_OBJECT, &data))
		liext_inventory_set_owner (args->self, data->data);
	else
		liext_inventory_set_owner (args->self, NULL);
}

/* @luadoc
 * ---
 * -- Custom callback called when the owner is changed.
 * --
 * -- Arguments passed to the callback: inventory, object.
 * --
 * -- @name Inventory.owner_changed_cb
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
static void Inventory_getter_size (LIScrArgs* args)
{
	liscr_args_seti_int (args, liext_inventory_get_size (args->self));
}
static void Inventory_setter_size (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value) && value >= 0)
		liext_inventory_set_size (args->self, value);
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
liext_script_inventory (LIScrClass* self,
                        void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_INVENTORY, data);
	liscr_class_insert_cfunc (self, "find", Inventory_find);
	liscr_class_insert_cfunc (self, "new", Inventory_new);
	liscr_class_insert_mfunc (self, "get_object", Inventory_get_object);
	liscr_class_insert_mfunc (self, "set_object", Inventory_set_object);
	liscr_class_insert_mfunc (self, "subscribe", Inventory_subscribe);
	liscr_class_insert_mfunc (self, "subscribed", Inventory_subscribed);
	liscr_class_insert_mfunc (self, "unsubscribe", Inventory_unsubscribe);
	liscr_class_insert_mvar (self, "first_free_slot", Inventory_getter_first_free_slot, NULL);
	liscr_class_insert_mvar (self, "id", Inventory_getter_id, NULL);
	liscr_class_insert_mvar (self, "owner", Inventory_getter_owner, Inventory_setter_owner);
	liscr_class_insert_mvar (self, "size", Inventory_getter_size, Inventory_setter_size);
}

/** @} */
/** @} */
