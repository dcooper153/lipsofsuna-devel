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

#include <network/lips-network.h>
#include <server/lips-server.h>
#include "ext-module.h"
#include "ext-slots.h"

#define LIEXT_SLOTS_VERSION 0xFF

static int
private_send_clear (liextSlots* self);

static int
private_send_reset (liextSlots* self);

static int
private_send_slot (liextSlots* self,
                   liextSlot*  slot);

/*****************************************************************************/

/**
 * \brief Creates a new slots type.
 *
 * \param module Module.
 * \return New slots type or NULL.
 */
liextSlots*
liext_slots_new (liextModule* module)
{
	liextSlots* self;

	self = calloc (1, sizeof (liextSlots));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->server = module->server;

	return self;
}

/**
 * \brief Frees the slots.
 *
 * Objects are managed by the garbage collector so the stored objects
 * must not be freed here. The garbage collector is already doing it.
 *
 * \param self Inventory.
 */
void
liext_slots_free (liextSlots* self)
{
	int i;

	for (i = 0 ; i < self->slots.count ; i++)
	{
		free (self->slots.array[i].name);
		free (self->slots.array[i].node);
	}
	free (self->slots.array);
	free (self);
}

liextSlot*
liext_slots_find_slot (liextSlots* self,
                       const char* name)
{
	int i;

	/* FIXME: Could use bsearch. */
	for (i = 0 ; i < self->slots.count ; i++)
	{
		if (!strcmp (name, self->slots.array[i].name))
			return self->slots.array + i;
	}

	return NULL;
}

/**
 * \brief Inserts a slot.
 *
 * \param self Slots.
 * \param type Slot protection type.
 * \param name Slot name.
 * \param node Model node name.
 */
int
liext_slots_insert_slot (liextSlots*   self,
                         liextSlotType type,
                         const char*   name,
                         const char*   node)
{
	liextSlot tmp;

	tmp.type = type;
	tmp.name = strdup (name);
	tmp.node = strdup (node);
	tmp.object = NULL;
	if (tmp.name == NULL || tmp.node == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (tmp.name);
		free (tmp.node);
		return 0;
	}
	if (!lialg_array_append (&self->slots, &tmp))
	{
		free (tmp.name);
		free (tmp.node);
		return 0;
	}

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_slot (self, &tmp))
			return 0;
	}

	return 1;
}

int
liext_slots_get_object (liextSlots*   self,
                        const char*   slot,
                        liengObject** object)
{
	liextSlot* s;

	s = liext_slots_find_slot (self, slot);
	if (s == NULL)
		return 0;
	*object = s->object;
	return 1;
}

int
liext_slots_set_object (liextSlots*  self,
                        const char*  slot,
                        liengObject* object)
{
	liextSlot* s;

	/* Store to slot. */
	s = liext_slots_find_slot (self, slot);
	if (s == NULL)
		return 0;
	if (s->object == object)
		return 1;
	if (s->object != NULL)
		liscr_data_unref (s->object->script, self->script);
	s->object = object;
	if (object != NULL)
	{
		liscr_data_ref (object->script, self->script);
		lieng_object_set_realized (object, 0);
	}

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_slot (self, s))
			return 0;
	}

	return 1;
}

liengObject*
liext_slots_get_owner (liextSlots* self)
{
	return self->owner;
}

int
liext_slots_set_owner (liextSlots*  self,
                       liengObject* value)
{
	if (self->owner == value)
		return 1;

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_clear (self))
			return 0;
	}

	/* Set new owner. */
	if (self->owner != NULL)
	{
		liext_module_remove_slots (self->module, self->owner, self);
		liscr_data_unref (self->owner->script, self->script);
	}
	self->owner = value;
	if (self->owner != NULL)
	{
		liscr_data_ref (self->owner->script, self->script);
		liext_module_insert_slots (self->module, self->owner, self);
	}

	/* Inform clients. */
	if (self->owner != NULL)
	{
		if (!private_send_reset (self))
			return 0;
	}

	return 1;
}

/*****************************************************************************/

static int
private_send_clear (liextSlots* self)
{
	liarcWriter* writer;
	liengObject* object;
	lisrvObserverIter iter;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SLOTS_PACKET_RESET);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);

	/* Send to all observers. */
	LISRV_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	return 1;
}

static int
private_send_reset (liextSlots* self)
{
	int i;
	liarcWriter* writer;
	liengObject* object;
	liextSlot* slot;
	lisrvObserverIter iter;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SLOTS_PACKET_RESET);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);
	for (i = 0 ; i < self->slots.count ; i++)
	{
		slot = self->slots.array + i;
		liarc_writer_append_string (writer, slot->name);
		liarc_writer_append_nul (writer);
		liarc_writer_append_string (writer, slot->node);
		liarc_writer_append_nul (writer);
		if (slot->object != NULL)
			liarc_writer_append_uint16 (writer, lieng_object_get_model_code (slot->object));
		else
			liarc_writer_append_uint16 (writer, LINET_INVALID_MODEL);
	}

	/* Send to all observers. */
	LISRV_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	return 1;
}

static int
private_send_slot (liextSlots* self,
                   liextSlot*  slot)
{
	liarcWriter* writer;
	liengObject* object;
	lisrvObserverIter iter;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SLOTS_PACKET_DIFF);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);
	liarc_writer_append_string (writer, slot->name);
	liarc_writer_append_nul (writer);
	liarc_writer_append_string (writer, slot->node);
	liarc_writer_append_nul (writer);
	if (slot->object != NULL)
		liarc_writer_append_uint16 (writer, lieng_object_get_model_code (slot->object));
	else
		liarc_writer_append_uint16 (writer, LINET_INVALID_MODEL);

	/* Send to all observers. */
	if (slot->type == LIEXT_SLOT_TYPE_PUBLIC)
	{
		LISRV_FOREACH_OBSERVER (iter, self->owner, 1)
		{
			object = iter.object;
			lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		}
	}
	else if (LISRV_OBJECT (self->owner)->client != NULL)
	{
		lisrv_client_send (LISRV_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);
	}
	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
/** @} */
