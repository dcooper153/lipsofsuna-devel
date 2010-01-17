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
 * \addtogroup LIExtSlots Slots
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-module.h"
#include "ext-slots.h"

static int
private_send_clear (LIExtSlots* self);

static int
private_send_reset (LIExtSlots* self);

static int
private_send_slot (LIExtSlots* self,
                   LIExtSlot*  slot);

/*****************************************************************************/

/**
 * \brief Creates a new slots type.
 *
 * \param module Module.
 * \return New slots type or NULL.
 */
LIExtSlots*
liext_slots_new (LIExtModule* module)
{
	LIExtSlots* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtSlots));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Allocate script. */
	self->script = liscr_data_new (module->program->script, self, LIEXT_SCRIPT_SLOTS, liext_slots_free);
	if (self == NULL)
	{
		lisys_free (self);
		return NULL;
	}

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
liext_slots_free (LIExtSlots* self)
{
	int i;

	for (i = 0 ; i < self->slots.count ; i++)
	{
		lisys_free (self->slots.array[i].name);
		lisys_free (self->slots.array[i].node);
	}
	lisys_free (self->slots.array);
	lisys_free (self);
}

LIExtSlot*
liext_slots_find_slot (LIExtSlots* self,
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
liext_slots_insert_slot (LIExtSlots*   self,
                         LIExtSlotType type,
                         const char*   name,
                         const char*   node)
{
	LIExtSlot tmp;

	tmp.type = type;
	tmp.name = listr_dup (name);
	tmp.node = listr_dup (node);
	tmp.object = NULL;
	if (tmp.name == NULL || tmp.node == NULL)
	{
		lisys_free (tmp.name);
		lisys_free (tmp.node);
		return 0;
	}
	if (!lialg_array_append (&self->slots, &tmp))
	{
		lisys_free (tmp.name);
		lisys_free (tmp.node);
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
liext_slots_get_object (LIExtSlots*   self,
                        const char*   slot,
                        LIEngObject** object)
{
	LIExtSlot* s;

	s = liext_slots_find_slot (self, slot);
	if (s == NULL)
		return 0;
	*object = s->object;
	return 1;
}

int
liext_slots_set_object (LIExtSlots*  self,
                        const char*  slot,
                        LIEngObject* object)
{
	LIExtSlot* s;

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

LIEngObject*
liext_slots_get_owner (LIExtSlots* self)
{
	return self->owner;
}

int
liext_slots_set_owner (LIExtSlots*  self,
                       LIEngObject* value)
{
	LIExtSlots* old;

	if (self->owner == value)
		return 1;

	/* Clear old client. */
	if (self->owner != NULL)
	{
		if (!private_send_clear (self))
			return 0;
	}

	/* Clear old slots. */
	if (value != NULL)
	{
		old = lialg_ptrdic_find (self->module->dictionary, value);
		if (old != NULL)
		{
			private_send_clear (old);
			liscr_data_unref (old->owner->script, old->script);
			liscr_data_unref (old->script, old->owner->script);
			lialg_ptrdic_remove (self->module->dictionary, value);
			old->owner = NULL;
		}
	}

	/* Set new owner. */
	if (self->owner != NULL)
	{
		liscr_data_unref (self->owner->script, self->script);
		liscr_data_unref (self->script, self->owner->script);
		lialg_ptrdic_remove (self->module->dictionary, self->owner);
	}
	if (value != NULL)
	{
		liscr_data_ref (value->script, self->script);
		liscr_data_ref (self->script, value->script);
		lialg_ptrdic_insert (self->module->dictionary, value, self);
	}

	/* Set new client. */
	self->owner = value;
	if (self->owner != NULL)
	{
		if (!private_send_reset (self))
			return 0;
	}

	return 1;
}

/*****************************************************************************/

static int
private_send_clear (LIExtSlots* self)
{
	LIArcWriter* writer;
	LIEngObject* object;
	LISerObserverIter iter;

	/* Create packet. */
	writer = liarc_writer_new_packet (LIEXT_SLOTS_PACKET_RESET);
	if (writer == NULL)
		return 0;
	liarc_writer_append_uint32 (writer, self->owner->id);

	/* Send to all observers. */
	LISER_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	return 1;
}

static int
private_send_reset (LIExtSlots* self)
{
	int i;
	LIArcWriter* writer;
	LIEngObject* object;
	LIExtSlot* slot;
	LISerObserverIter iter;

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
	LISER_FOREACH_OBSERVER (iter, self->owner, 1)
	{
		object = iter.object;
		liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	}

	return 1;
}

static int
private_send_slot (LIExtSlots* self,
                   LIExtSlot*  slot)
{
	LIArcWriter* writer;
	LIEngObject* object;
	LISerObserverIter iter;

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
		LISER_FOREACH_OBSERVER (iter, self->owner, 1)
		{
			object = iter.object;
			liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		}
	}
	else if (LISER_OBJECT (self->owner)->client != NULL)
	{
		liser_client_send (LISER_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);
	}
	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
