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

#include <network/lips-network.h>
#include "ext-module.h"
#include "ext-inventory.h"

#define LIEXT_INVENTORY_VERSION 0xFF

static int
private_send_clear (liextInventory* self);

static int
private_send_reset (liextInventory* self);

static int
private_send_object (liextInventory* self,
                     int             slot,
                     liengObject*    object);

/*****************************************************************************/

/**
 * \brief Creates a new inventory.
 *
 * \param module Extension module.
 * \return New inventory or NULL.
 */
liextInventory*
liext_inventory_new (liextModule* module)
{
	liextInventory* self;

	self = calloc (1, sizeof (liextInventory));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->server = module->server;
	if (!liext_inventory_set_size (self, 10))
	{
		free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the inventory.
 *
 * Objects are managed by the garbage collector so the stored objects
 * must not be freed here. The garbage collector is already doing it.
 *
 * \param self Inventory.
 */
void
liext_inventory_free (liextInventory* self)
{
	free (self->slots.array);
	free (self);
}

/**
 * \brief Serializes or deserializes the inventory object.
 *
 * \param self Inventory.
 * \param serialize Serializer.
 * \return Nonzero on success.
 */
int
liext_inventory_serialize (liextInventory* self,
                           liarcSerialize* serialize)
{
	int i;
	uint8_t version;
	uint32_t count;
	liengObject* object;

	if (liarc_serialize_get_write (serialize))
	{
		/* Write header. */
		if (!liarc_writer_append_uint8 (serialize->writer, LIEXT_INVENTORY_VERSION) ||
		    !liarc_writer_append_uint32 (serialize->writer, self->slots.count))
			return 0;

		/* Write owner. */
		if (!lieng_engine_write_object (self->server->engine, serialize, self->owner))
			return 0;

		/* Write slots. */
		for (i = 0 ; i < self->slots.count ; i++)
		{
			object = self->slots.array[i];
			if (!lieng_engine_write_object (self->server->engine, serialize, object))
				return 0;
		}
	}
	else
	{
		/* Read header. */
		if (!li_reader_get_uint8 (serialize->reader, &version) ||
		    !li_reader_get_uint32 (serialize->reader, &count))
			return 0;
		if (version != LIEXT_INVENTORY_VERSION)
		{
			lisys_error_set (EINVAL, "unsupported inventory version");
			return 0;
		}
		if (!liext_inventory_set_size (self, count))
			return 0;

		/* Read owner. */
		if (!lieng_engine_read_object (self->server->engine, serialize, &object) ||
		    !liext_inventory_set_owner (self, object))
			return 0;

		/* Read slots. */
		for (i = 0 ; i < count ; i++)
		{
			if (!lieng_engine_read_object (self->server->engine, serialize, &object))
				return 0;
			if (!liext_inventory_set_object (self, i, object))
				return 0;
		}
	}

	return 1;
}

/**
 * \brief Gets an object in the specified slot.
 *
 * \param self Inventory.
 * \param slot Slot number.
 * \return Object or NULL.
 */
liengObject*
liext_inventory_get_object (liextInventory* self,
                            int             slot)
{
	if (slot < 0 || slot >= self->slots.count)
		return NULL;

	return self->slots.array[slot];
}

/**
 * \brief Sets an object in the specified slot.
 *
 * \param self Inventory.
 * \param slot Slot number.
 * \param object Object to set.
 * \return Nonzero on success.
 */
int
liext_inventory_set_object (liextInventory* self,
                            int             slot,
                            liengObject*    object)
{
	if (slot < 0 || slot >= self->slots.count)
		return 0;
	if (self->slots.array[slot] != NULL)
		liscr_data_unref (self->slots.array[slot]->script, self->script);
	self->slots.array[slot] = object;
	if (object != NULL)
		liscr_data_ref (object->script, self->script);
	private_send_object (self, slot, object);

	return 1;
}

/**
 * \brief Gets the owner of the inventory.
 *
 * \param self Inventory.
 * \return Object or NULL.
 */
liengObject*
liext_inventory_get_owner (liextInventory* self)
{
	return self->owner;
}

/**
 * \brief Sets the owner of the inventory.
 *
 * \param self Inventory.
 * \param value Object.
 * \return Nonzero on success.
 */
int
liext_inventory_set_owner (liextInventory* self,
                           liengObject*    value)
{
	if (self->owner == value)
		return 1;

	/* Inform old owner. */
	if (self->owner != NULL)
	{
		if (!private_send_clear (self))
			return 0;
	}

	/* Set new owner. */
	if (self->owner != NULL)
	{
		liext_module_remove_inventory (self->module, self->owner, self);
		liscr_data_unref (self->owner->script, self->script);
	}
	self->owner = value;
	if (self->owner != NULL)
	{
		liscr_data_ref (self->owner->script, self->script);
		liext_module_insert_inventory (self->module, self->owner, self);
	}

	/* Inform new owner. */
	if (self->owner != NULL)
	{
		if (!private_send_reset (self))
			return 0;
	}

	return 1;
}

/**
 * \brief Gets the number of items the inventory can hold.
 *
 * \param self Inventory.
 * \return Integer.
 */
int
liext_inventory_get_size (liextInventory* self)
{
	return self->slots.count;
}

/**
 * \brief Sets the number of items the inventory can hold.
 *
 * \param self Inventory.
 * \param value Integer.
 * \return Nonzero on success.
 */
int
liext_inventory_set_size (liextInventory* self,
                          int             value)
{
	int i;
	liengObject** tmp;

	if (value == self->slots.count)
		return 1;
	if (value > self->slots.count)
	{
		tmp = realloc (self->slots.array, value * sizeof (liengObject*));
		if (tmp == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			return 0;
		}
		self->slots.array = tmp;
		tmp += self->slots.count;
		memset (tmp, 0, (value - self->slots.count) * sizeof (liengObject*));
		self->slots.count = value;
	}
	else
	{
		for (i = value ; i < self->slots.count ; i++)
		{
			if (self->slots.array[i] != NULL)
				liscr_data_unref (self->slots.array[i]->script, self->script);
		}
		tmp = realloc (self->slots.array, value * sizeof (liengObject*));
		if (tmp != NULL || !value)
			self->slots.array = tmp;
		self->slots.count = value;
	}
	private_send_reset (self);

	return 1;
}

/*****************************************************************************/

static int
private_send_clear (liextInventory* self)
{
	liarcWriter* writer;

	if (self->owner != NULL && LISRV_OBJECT (self->owner)->client != NULL)
	{
		/* Create packet. */
		writer = liarc_writer_new_packet (LIEXT_INVENTORY_PACKET_RESET);
		if (writer == NULL)
			return 0;
		liarc_writer_append_uint32 (writer, self->owner->id);
		liarc_writer_append_uint8 (writer, 0);

		/* Send to owner. */
		lisrv_client_send (LISRV_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);

		liarc_writer_free (writer);
	}

	return 1;
}

static int
private_send_reset (liextInventory* self)
{
	int i;
	liarcWriter* writer;
	liengObject* object;

	if (self->owner != NULL && LISRV_OBJECT (self->owner)->client != NULL)
	{
		/* Create packet. */
		writer = liarc_writer_new_packet (LIEXT_INVENTORY_PACKET_RESET);
		if (writer == NULL)
			return 0;
		liarc_writer_append_uint32 (writer, self->owner->id);
		liarc_writer_append_uint8 (writer, self->slots.count);
		for (i = 0 ; i < self->slots.count ; i++)
		{
			object = self->slots.array[i];
			if (object != NULL)
			{
				liarc_writer_append_uint8 (writer, i);
				liarc_writer_append_uint16 (writer, lieng_object_get_model_code (object));
			}
		}

		/* Send to owner. */
		lisrv_client_send (LISRV_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);

		liarc_writer_free (writer);
	}

	return 1;
}

static int
private_send_object (liextInventory* self,
                     int             slot,
                     liengObject*    object)
{
	liarcWriter* writer;

	if (self->owner != NULL && LISRV_OBJECT (self->owner)->client != NULL)
	{
		/* Create packet. */
		writer = liarc_writer_new_packet (LIEXT_INVENTORY_PACKET_DIFF);
		if (writer == NULL)
			return 0;
		liarc_writer_append_uint32 (writer, self->owner->id);
		liarc_writer_append_uint8 (writer, slot);
		if (object != NULL)
			liarc_writer_append_uint16 (writer, lieng_object_get_model_code (object));
		else
			liarc_writer_append_uint16 (writer, LINET_INVALID_MODEL);

		/* Send to owner. */
		lisrv_client_send (LISRV_OBJECT (self->owner)->client, writer, GRAPPLE_RELIABLE);

		liarc_writer_free (writer);
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
