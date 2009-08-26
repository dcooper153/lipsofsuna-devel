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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#include <client/lips-client.h>
#include "ext-module.h"
#include "ext-slots.h"

static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader);

static int
private_packet_diff (liextModule* self,
                     liarcReader* reader);

static int
private_packet_reset (liextModule* self,
                      liarcReader* reader);

static int
private_tick (liextModule* self,
              float        secs);

static int
private_visibility (liextModule* self,
                    liengObject* object,
                    int          value);

/*****************************************************************************/

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Slots",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->dictionary = lialg_u32dic_new ();
	if (self->dictionary == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lieng_engine_insert_call (module->engine, LICLI_CALLBACK_PACKET, 0,
	     	private_packet, self, self->calls + 0) ||
	    !lieng_engine_insert_call (module->engine, LICLI_CALLBACK_TICK, 0,
	     	private_tick, self, self->calls + 1) ||
	    !lieng_engine_insert_call (module->engine, LIENG_CALLBACK_OBJECT_VISIBILITY, 0,
	     	private_visibility, self, self->calls + 2))
	{
		liext_module_free (self);
		return NULL;
	}

	return self;
}

void
liext_module_free (liextModule* self)
{
	lialgU32dicIter iter;

	LI_FOREACH_U32DIC (iter, self->dictionary)
		liext_slots_free (iter.value);
	lialg_u32dic_free (self->dictionary);
	lieng_engine_remove_calls (self->module->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader)
{
	reader->pos = 1;
	switch (type)
	{
		case LIEXT_SLOTS_PACKET_RESET:
			private_packet_reset (self, reader);
			break;
		case LIEXT_SLOTS_PACKET_DIFF:
			private_packet_diff (self, reader);
			break;
	}

	return 1;
}

static int
private_packet_diff (liextModule* self,
                     liarcReader* reader)
{
	char* slot = NULL;
	char* node = NULL;
	uint32_t id;
	uint16_t model;
	liengObject* object;
	liextSlots* slots;

	/* Find or create slots block. */
	if (!liarc_reader_get_uint32 (reader, &id))
		return 0;
	slots = lialg_u32dic_find (self->dictionary, id);
	if (slots == NULL)
	{
		object = licli_module_find_object (self->module, id);
		if (object == NULL)
			return 0;
		slots = liext_slots_new (self, object);
		if (slots == NULL)
			return 0;
		if (!lialg_u32dic_insert (self->dictionary, id, slots))
		{
			liext_slots_free (slots);
			return 0;
		}
	}

	/* Insert models to slots. */
	while (!liarc_reader_check_end (reader))
	{
		if (!liarc_reader_get_text (reader, "", &slot) ||
		    !liarc_reader_get_text (reader, "", &node) ||
		    !liarc_reader_get_uint16 (reader, &model))
		{
			lisys_free (slot);
			lisys_free (node);
			return 0;
		}
		liext_slots_set_slot (slots, slot, node, model);
		lisys_free (slot);
		lisys_free (node);
	}

	return 1;
}

static int
private_packet_reset (liextModule* self,
                      liarcReader* reader)
{
	char* slot = NULL;
	char* node = NULL;
	uint32_t id;
	uint16_t model;
	liengObject* object;
	liextSlots* slots;

	/* Create or clear slots block. */
	if (!liarc_reader_get_uint32 (reader, &id))
		return 0;
	slots = lialg_u32dic_find (self->dictionary, id);
	if (slots == NULL)
	{
		object = licli_module_find_object (self->module, id);
		if (object == NULL)
			return 0;
		slots = liext_slots_new (self, object);
		if (slots == NULL)
			return 0;
		if (!lialg_u32dic_insert (self->dictionary, id, slots))
		{
			liext_slots_free (slots);
			return 0;
		}
	}
	else
		liext_slots_clear (slots);

	/* Insert models to slots. */
	while (!liarc_reader_check_end (reader))
	{
		if (!liarc_reader_get_text (reader, "", &slot) ||
		    !liarc_reader_get_text (reader, "", &node) ||
		    !liarc_reader_get_uint16 (reader, &model))
		{
			lisys_free (slot);
			lisys_free (node);
			return 0;
		}
		liext_slots_set_slot (slots, slot, node, model);
		lisys_free (slot);
		lisys_free (node);
	}

	return 1;
}

static int
private_tick (liextModule* self,
              float        secs)
{
	return 1;
}

static int
private_visibility (liextModule* self,
                    liengObject* object,
                    int          value)
{
	lialgU32dicIter iter;
	liextSlots* slots;

	if (!value)
	{
		/* Free slots block. */
		slots = lialg_u32dic_find (self->dictionary, object->id);
		if (slots != NULL)
		{
			lialg_u32dic_remove (self->dictionary, object->id);
			liext_slots_free (slots);
		}

		/* Disown slot objects. */
		LI_FOREACH_U32DIC (iter, self->dictionary)
		{
			slots = iter.value;
			liext_slots_clear_object (slots, object);
		}
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
