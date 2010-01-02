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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#include "ext-slots.h"

liextSlots*
liext_slots_new (liextModule* module,
                 liengObject* object)
{
	liextSlots* self;

	self = lisys_calloc (1, sizeof (liextSlots));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->object = object;
	self->slots = lialg_strdic_new ();
	if (self->slots == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
liext_slots_free (liextSlots* self)
{
	if (self->slots != NULL)
	{
		liext_slots_clear (self);
		lialg_strdic_free (self->slots);
	}
	lisys_free (self);
}

void
liext_slots_clear (liextSlots* self)
{
	lialgStrdicIter iter;

	LI_FOREACH_STRDIC (iter, self->slots)
		liext_slot_free (iter.value);
	lialg_strdic_clear (self->slots);
}

void
liext_slots_clear_object (liextSlots*  self,
                          liengObject* object)
{
	lialgStrdicIter iter;
	liextSlot* slot;

	LI_FOREACH_STRDIC (iter, self->slots)
	{
		slot = iter.value;
		if (slot->object == object)
		{
			lialg_strdic_remove (self->slots, iter.key);
			slot->object = NULL;
			liext_slot_free (slot);
			break;
		}
	}
}

int
liext_slots_set_slot (liextSlots* self,
                      const char* slot,
                      const char* node,
                      int         model)
{
	liextSlot* slot_;

	/* Remove old slot. */
	slot_ = lialg_strdic_find (self->slots, slot);
	if (slot_ != NULL)
	{
		lialg_strdic_remove (self->slots, slot);
		liext_slot_free (slot_);
	}

	/* Create new slot. */
	if (model != LINET_INVALID_MODEL)
	{
		slot_ = liext_slot_new (self->module->client, self->object, node, "#root", model);
		if (slot_ == NULL)
			return 0;
		if (!lialg_strdic_insert (self->slots, slot, slot_))
		{
			liext_slot_free (slot_);
			return 0;
		}
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
