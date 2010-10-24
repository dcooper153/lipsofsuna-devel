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
 * \brief Allocates a new client.
 * \param module Module.
 * \param user Network user.
 * \return New client or NULL.
 */
LIExtClient* liext_client_new (
	LIExtModule* module,
	grapple_user user)
{
	LIExtClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtClient));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->net = user;

	return self;
}

/**
 * \brief Disconnects and frees the client.
 *
 * \param self Client.
 */
void liext_client_free (
	LIExtClient* self)
{
	/* Signal logout. */
	limai_program_event (self->module->program, "logout",
		"client", LISCR_TYPE_INT, (int) self->net, NULL);

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
void liext_client_send (
	LIExtClient* self,
	LIArcWriter* writer,
	int          flags)
{
	grapple_server_send (self->module->server_socket, self->net, flags,
		liarc_writer_get_buffer (writer),
		liarc_writer_get_length (writer));
}

/** @} */
/** @} */
