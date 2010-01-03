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
 * \addtogroup liextsrv Server 
 * @{
 * \addtogroup liextsrvSlots Slots
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-module.h"
#include "ext-slots.h"

static int
private_vision_show (LIExtModule* self,
                     LIEngObject* object,
                     LIEngObject* observer);

/*****************************************************************************/

LISerExtensionInfo liextInfo =
{
	LISER_EXTENSION_VERSION, "Slots",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LISerServer* server)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->dictionary = lialg_ptrdic_new ();
	if (self->dictionary == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (server->callbacks, server->engine, "vision-show", 0, private_vision_show, self, self->calls + 0))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (server->script, "Slots", liext_script_slots, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	lialg_ptrdic_free (self->dictionary);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int
private_vision_show (LIExtModule* self,
                     LIEngObject* object,
                     LIEngObject* target)
{
	int i;
	int perm;
	LIArcWriter* writer;
	LIExtSlot* slot;
	LIExtSlots* slots;

	/* Get skills data. */
	slots = lialg_ptrdic_find (self->dictionary, target);
	if (slots == NULL)
		return 1;
	perm = (target == object);

	/* Create reset packet. */
	writer = liarc_writer_new_packet (LIEXT_SLOTS_PACKET_RESET);
	if (writer == NULL)
		return 1;
	liarc_writer_append_uint32 (writer, target->id);
	for (i = 0 ; i < slots->slots.count ; i++)
	{
		slot = slots->slots.array + i;
		if (!perm && slot->type == LIEXT_SLOT_TYPE_PRIVATE)
			continue;
		liarc_writer_append_string (writer, slot->name);
		liarc_writer_append_nul (writer);
		liarc_writer_append_string (writer, slot->node);
		liarc_writer_append_nul (writer);
		if (slot->object != NULL)
			liarc_writer_append_uint16 (writer, lieng_object_get_model_code (slot->object));
		else
			liarc_writer_append_uint16 (writer, LINET_INVALID_MODEL);
	}

	/* Send to the client. */
	liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
	liarc_writer_free (writer);

	return 1;
}

/** @} */
/** @} */
/** @} */
