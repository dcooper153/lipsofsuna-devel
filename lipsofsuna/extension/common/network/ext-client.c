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
 * \addtogroup LIExtNetwork Network
 * @{
 */

#include "ext-module.h"
#include "ext-client.h"

/**
 * \brief Allocates a new client block.
 *
 * \param module Module.
 * \param object Object associated to the client.
 * \param user Network user.
 * \return New client or NULL.
 */
LIExtClient*
liext_client_new (LIExtModule* module,
                  LIEngObject* object,
                  grapple_user user)
{
	LIExtClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtClient));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->net = user;

	/* Assign object. */
	if (!liext_client_set_object (self, object))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Disconnects and frees the client.
 *
 * \param self Client.
 */
void
liext_client_free (LIExtClient* self)
{
	/* Signal logout. */
	if (self->object != NULL)
	{
		lical_callbacks_call (self->module->program->engine->callbacks, self->module->program->engine, "client-logout", lical_marshal_DATA_PTR, self->object);
		lialg_u32dic_remove (self->module->objects, self->object->id);
		lieng_object_ref (self->object, -1);
	}

	/* Remove from the client list. */
	if (self->net != 0)
	{
		lialg_u32dic_remove (self->module->clients, self->net);
		grapple_server_disconnect_client (self->module->server_socket, self->net);
	}

	lisys_free (self);
}

/**
 * \brief Sends a network packet to the client.
 *
 * \param self Client.
 * \param writer Packet.
 * \param flags Grapple send flags.
 */
void
liext_client_send (LIExtClient* self,
                   LIArcWriter* writer,
                   int          flags)
{
	grapple_server_send (self->module->server_socket, self->net, flags,
		liarc_writer_get_buffer (writer),
		liarc_writer_get_length (writer));
}

/**
 * \brief Swaps the objects of the clients.
 *
 * \param self Client.
 * \param client Client.
 */
void
liext_client_swap (LIExtClient* self,
                   LIExtClient* client)
{
	LIEngObject* tmp;

	tmp = self->object;
	liext_client_set_object (self, client->object);
	liext_client_set_object (client, tmp);
}

/**
 * \brief Sets the object controlled by the client.
 *
 * \param self Client.
 * \param value Object.
 * \return Nonzero on success.
 */
int
liext_client_set_object (LIExtClient* self,
                         LIEngObject* value)
{
	lisys_assert (value != NULL);

	if (self->object == value)
		return 1;

	/* Clear old object. */
	if (self->object != NULL)
	{
		lialg_u32dic_remove (self->module->objects, self->object->id);
		lieng_object_ref (self->object, -1);
	}

	/* Set the new object. */
	self->object = value;
	lieng_object_ref (value, 1);
	lical_callbacks_call (self->module->program->engine->callbacks, self->module->program->engine, "object-client", lical_marshal_DATA_PTR, value);
	if (!lialg_u32dic_insert (self->module->objects, self->object->id, self))
		return 0;

	return 1;
}

/** @} */
/** @} */
