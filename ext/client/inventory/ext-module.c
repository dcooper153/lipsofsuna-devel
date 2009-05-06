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
 * \addtogroup liextcliInventory Inventory
 * @{
 */

#include <client/lips-client.h>
#include "ext-module.h"
#include "ext-inventory.h"

static int
private_packet (liextModule* module,
                int          type,
                liReader*    reader);

static int
private_packet_diff (liextModule* module,
                     liReader*    reader);

static int
private_packet_reset (liextModule* module,
                      liReader*    reader);

static int
private_visibility (liextModule* self,
                    liengObject* object,
                    int          value);

/*****************************************************************************/

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Inventory",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	self = calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->inventory = liext_inventory_new (self);
	if (self->inventory == NULL)
	{
		free (self);
		return NULL;
	}
	self->calls[0] = lieng_engine_call_insert (module->engine, LICLI_CALLBACK_PACKET, 0, private_packet, self);
	self->calls[1] = lieng_engine_call_insert (module->engine, LICLI_CALLBACK_VISIBILITY, 0, private_visibility, self);

	liscr_script_insert_class (module->script, "InventoryWidget", liextInventoryWidgetScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	liext_inventory_free (self->inventory);
	lieng_engine_call_remove (self->module->engine, LICLI_CALLBACK_PACKET, self->calls[0]);
	lieng_engine_call_remove (self->module->engine, LICLI_CALLBACK_VISIBILITY, self->calls[1]);
	free (self);
}

liextInventory*
liext_module_get_inventory (liextModule* self)
{
	return self->inventory;
}

/*****************************************************************************/

static int
private_packet (liextModule* self,
                int          type,
                liReader*    reader)
{
	switch (type)
	{
		case LIEXT_INVENTORY_PACKET_RESET:
			private_packet_reset (self, reader);
			return 0;
		case LIEXT_INVENTORY_PACKET_DIFF:
			private_packet_diff (self, reader);
			return 0;
	}

	return 1;
}

static int
private_packet_diff (liextModule* self,
                     liReader*    reader)
{
	uint8_t slot;
	uint16_t object;
	uint32_t id;

	if (!li_reader_get_uint32 (reader, &id))
		return 0;

	/* Insert items. */
	while (!li_reader_check_end (reader))
	{
		if (!li_reader_get_uint8 (reader, &slot) ||
		    !li_reader_get_uint16 (reader, &object) ||
		    !liext_inventory_set_object (self->inventory, slot, object))
		    return 0;
	}

	return 1;
}

static int
private_packet_reset (liextModule* self,
                      liReader*    reader)
{
	uint8_t slot;
	uint8_t size;
	uint16_t object;
	uint32_t id;

	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_uint8 (reader, &size))
		return 0;
	if (!liext_inventory_set_size (self->inventory, size))
		return 0;
	liext_inventory_clear (self->inventory);

	/* Insert items. */
	while (!li_reader_check_end (reader))
	{
		if (!li_reader_get_uint8 (reader, &slot) ||
		    !li_reader_get_uint16 (reader, &object) ||
		    !liext_inventory_set_object (self->inventory, slot, object))
		    return 0;
	}

	return 1;
}

static int
private_visibility (liextModule* self,
                    liengObject* object,
                    int          value)
{
	int i;

	if (!value)
	{
		for (i = 0 ; i < self->inventory->slots.count ; i++)
		{
			if (self->inventory->slots.array[i] == object)
				self->inventory->slots.array[i] = NULL;
		}
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
