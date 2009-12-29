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
 * \addtogroup liextsrvSlots Slots
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-slots.h"

/* @luadoc
 * module "Extension.Server.Slots"
 * ---
 * -- Create and synchronize visible equipment slots.
 * -- @name Slots
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Gets an object in a slot.
 * --
 * -- Arguments:
 * -- slot: Slot name.
 * --
 * -- @param self Slots.
 * -- @param args Arguments.
 * -- @return Object or nil.
 * function Slots.get_object(self, args)
 */
static void Slots_get_object (liscrArgs* args)
{
	const char* name;
	liextSlot* slot;

	if (!liscr_args_gets_string (args, "slot", &name))
		return;
	slot = liext_slots_find_slot (args->self, name);
	if (slot == NULL)
		return;
	if (slot->object != NULL)
		liscr_args_seti_data (args, slot->object->script);
}

/* @luadoc
 * ---
 * -- Gets information on a slot.
 * --
 * -- Arguments:
 * -- slot: Slot name.
 * --
 * -- @param self Slots.
 * -- @param args Arguments.
 * -- @return Slot information or nil.
 * function Slots.get_slot(self, args)
 */
static void Slots_get_slot (liscrArgs* args)
{
	const char* name;
	liextSlot* slot;

	/* Find slot. */
	if (!liscr_args_gets_string (args, "slot", &name))
		return;
	slot = liext_slots_find_slot (args->self, name);
	if (slot == NULL)
		return;

	/* Collect information. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "name", slot->name);
	liscr_args_sets_string (args, "node", slot->node);
	liscr_args_sets_string (args, "type", (slot->type == LIEXT_SLOT_TYPE_PRIVATE)? "private" : "public");
	if (slot->object != NULL)
		liscr_args_sets_data (args, "object", slot->object->script);
}

/* @luadoc
 * ---
 * -- Creates a new slots object.
 * --
 * -- @param self Slots class.
 * -- @param args Arguments.
 * -- @return New slots.
 * function Slots.new(self, args)
 */
static void Slots_new (liscrArgs* args)
{
	liextModule* module;
	liextSlots* self;
	liscrData* data;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SLOTS);
	self = liext_slots_new (module);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_SLOTS, liext_slots_free);
	if (data == NULL)
	{
		liext_slots_free (self);
		return;
	}
	self->script = data;
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Registers a slot.
 * --
 * -- Arguments:
 * -- node: Armature node name. (required)
 * -- object: Object to store.
 * -- slot: Slot name.
 * -- type: Protection type ("public"/"private")
 * --
 * -- @param self Slots.
 * -- @param args Arguments.
 * function Slots.register(self, args)
 */
static void Slots_register (liscrArgs* args)
{
	int type;
	const char* name;
	const char* prot = "private";
	const char* node = "#root";
	liextSlot* slot;
	liscrData* data;

	if (!liscr_args_gets_string (args, "slot", &name))
		return;
	liscr_args_gets_string (args, "prot", &prot);
	liscr_args_gets_string (args, "node", &node);
	liscr_args_gets_data (args, "object", LICOM_SCRIPT_OBJECT, &data);

	/* Check for existing slot. */
	/* TODO: Override type if different and inform clients? */
	slot = liext_slots_find_slot (args->self, name);
	if (slot != NULL)
		return;

	/* Create the slot. */
	if (!strcmp (prot, "public"))
		type = LIEXT_SLOT_TYPE_PUBLIC;
	else
		type = LIEXT_SLOT_TYPE_PRIVATE;
	liext_slots_insert_slot (args->self, type, name, node);

	/* Insert the object. */
	if (data != NULL)
		liext_slots_set_object (args->self, name, data->data);
}

/* @luadoc
 * ---
 * -- Sets the object in a slot.
 * --
 * -- Arguments:
 * -- slot: Slot name.
 * -- object: Object.
 * --
 * -- @param self Slots.
 * -- @param args Arguments.
 * function Slots.set_object(self, args)
 */
static void Slots_set_object (liscrArgs* args)
{
	const char* name;
	liextSlot* slot;
	liscrData* data;

	if (!liscr_args_gets_string (args, "slot", &name))
		return;
	slot = liext_slots_find_slot (args->self, name);
	if (slot == NULL)
		return;
	liscr_args_gets_data (args, "object", LICOM_SCRIPT_OBJECT, &data);
	if (data != NULL)
		liext_slots_set_object (args->self, name, data->data);
	else
		liext_slots_set_object (args->self, name, NULL);
}

/* @luadoc
 * ---
 * -- Owner object.
 * -- @name Slots.owner
 * -- @class table
 */
static void Slots_getter_owner (liscrArgs* args)
{
	liengObject* object;

	object = liext_slots_get_owner (args->self);
	if (object != NULL)
		liscr_args_seti_data (args, object->script);
}
static void Slots_setter_owner (liscrArgs* args)
{
	liscrData* data;

	if (liscr_args_geti_data (args, 0, LICOM_SCRIPT_OBJECT, &data))
		liext_slots_set_owner (args->self, data->data);
	else
		liext_slots_set_owner (args->self, NULL);
}

/*****************************************************************************/

void
liextSlotsScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SLOTS, data);
	liscr_class_insert_mfunc (self, "get_object", Slots_get_object);
	liscr_class_insert_mfunc (self, "get_slot", Slots_get_slot);
	liscr_class_insert_cfunc (self, "new", Slots_new);
	liscr_class_insert_mfunc (self, "register", Slots_register);
	liscr_class_insert_mfunc (self, "set_object", Slots_set_object);
	liscr_class_insert_mvar (self, "owner", Slots_getter_owner, Slots_setter_owner);
}

/** @} */
/** @} */
/** @} */
