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
 * -- Slot type for private slots.
 * --
 * -- Objects stored to private slots are only transmitted to the client
 * -- who is the owner of the slots.
 * --
 * -- @name Skills.INTERNAL
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Slot type for public slots.
 * --
 * -- Objects stored to public slots are transmitted to all clients who
 * -- can see the owner of the slots.
 * --
 * -- @name Skills.INTERNAL
 * -- @class table
 */

static int
Slots___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SLOTS);

	liext_slots_free (self->data);
	liscr_data_free (self);
	return 0;
}

static int
Slots___index (lua_State* lua)
{
	const char* slot;
	liscrData* self;
	liengObject* object;

	/* Try slots first. */
	if (lua_isstring (lua, 2))
	{
		self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SLOTS);
		if (self != NULL)
		{
			slot = lua_tostring (lua, 2);
			if (liext_slots_get_object (self->data, slot, &object))
			{
				if (object != NULL)
					liscr_pushdata (lua, object->script);
				else
					lua_pushnil (lua);
				return 1;
			}
		}
	}

	return liscr_class_default___index (lua);
}

static int
Slots___newindex (lua_State* lua)
{
	const char* slot;
	liscrData* data;
	liscrData* self;
	liextSlot* slot_;

	/* Try slots first. */
	if (lua_isstring (lua, 2))
	{
		self = liscr_isdata (lua, 1, LIEXT_SCRIPT_SLOTS);
		if (self != NULL)
		{
			slot = lua_tostring (lua, 2);
			slot_ = liext_slots_find_slot (self->data, slot);
			if (slot_ != NULL)
			{
				if (!lua_isnil (lua, 3))
				{
					data = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);
					liext_slots_set_object (self->data, slot, data->data);
				}
				else
					liext_slots_set_object (self->data, slot, NULL);
				return 1;
			}
		}
	}

	return liscr_class_default___newindex (lua);
}

/* @luadoc
 * ---
 * -- Creates a new slots object.
 * --
 * -- @param self Slots class.
 * -- @param table Optional table of arguments.
 * -- @return New slots.
 * function Slots.new(self, table)
 */
static int
Slots_new (lua_State* lua)
{
	liextModule* module;
	liextSlots* slots;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SLOTS);

	/* Allocate self. */
	slots = liext_slots_new (module);
	if (slots == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, slots, LIEXT_SCRIPT_SLOTS);
	if (self == NULL)
	{
		liext_slots_free (slots);
		lua_pushnil (lua);
		return 1;
	}
	slots->script = self;

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Registers a slot.
 * --
 * -- @param self Slots.
 * -- @param type Slot protection type.
 * -- @param name Slot name.
 * -- @param node Model node attachment hint for client.
 * function Slots.register(self, type, name, node)
 */
static int
Slots_register (lua_State* lua)
{
	int type;
	const char* name;
	const char* node;
	liscrData* self;
	liextSlot* slot;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SLOTS);
	type = (int) luaL_checknumber (lua, 2);
	name = luaL_checkstring (lua, 3);
	node = luaL_checkstring (lua, 4);
	luaL_argcheck (lua, type >= 0 && type <= LIEXT_SLOT_TYPE_MAX, 2, "invalid slot type");

	/* Check for existing slot. */
	/* TODO: Override type if different and inform clients? */
	slot = liext_slots_find_slot (self->data, name);
	if (slot != NULL)
		return 0;

	/* Create the slot. */
	liext_slots_insert_slot (self->data, type, name, node);

	return 0;
}

/* @luadoc
 * ---
 * -- Owner object.
 * -- @name Slots.owner
 * -- @class table
 */
static int
Slots_getter_owner (lua_State* lua)
{
	liengObject* object;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SLOTS);

	object = liext_slots_get_owner (self->data);
	if (object != NULL && object->script != NULL)
		liscr_pushdata (lua, object->script);
	else
		lua_pushnil (lua);
	return 1;
}
static int
Slots_setter_owner (lua_State* lua)
{
	liscrData* object;
	liscrData* self;
	liengObject* value;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_SLOTS);
	if (!lua_isnil (lua, 3))
	{
		object = liscr_checkdata (lua, 3, LICOM_SCRIPT_OBJECT);
		value = object->data;
	}
	else
		value = NULL;
#warning Should check that the object is not added to another slot already.

	liext_slots_set_owner (self->data, value);
	return 0;
}

/*****************************************************************************/

void
liextSlotsScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SLOTS, data);
	liscr_class_insert_enum (self, "PRIVATE", LIEXT_SLOT_TYPE_PRIVATE);
	liscr_class_insert_enum (self, "PUBLIC", LIEXT_SLOT_TYPE_PUBLIC);
	liscr_class_insert_func (self, "__gc", Slots___gc);
	liscr_class_insert_func (self, "__index", Slots___index);
	liscr_class_insert_func (self, "__newindex", Slots___newindex);
	liscr_class_insert_func (self, "new", Slots_new);
	liscr_class_insert_func (self, "register", Slots_register);
	liscr_class_insert_getter (self, "owner", Slots_getter_owner);
	liscr_class_insert_setter (self, "owner", Slots_setter_owner);
}

/** @} */
/** @} */
/** @} */
