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
 * \addtogroup liextsrvSkeleton Skeleton
 * @{
 */

#include <network/lips-network.h>
#include <server/lips-server.h>
#include "ext-skeleton.h"

static int
private_client_packet (liextSkeleton* self,
                       lisrvClient*   client,
                       liReader*      packet);

/*****************************************************************************/

liextSkeleton*
liext_skeleton_new (lisrvServer* server)
{
	liextSkeleton* self;

	self = calloc (1, sizeof (liextSkeleton));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->calls[0] = lieng_engine_call_insert (server->engine, LISRV_CALLBACK_CLIENT_PACKET, 1, private_client_packet, self);
	if (self->calls[0] == NULL)
	{
		free (self);
		return NULL;
	}

	return self;
}

void
liext_skeleton_free (liextSkeleton* self)
{
	lieng_engine_call_remove (self->server->engine, LISRV_CALLBACK_CLIENT_PACKET, self->calls[0]);
	free (self);
}

/*****************************************************************************/

static int
private_client_packet (liextSkeleton* self,
                       lisrvClient*   client,
                       liReader*      packet)
{
#if 0
	switch (((uint8_t*) packet->buffer)[0])
	{
		case LIEXT_SKELETON_PACKET_...:
			break;
	}
#endif

	return 1;
}

/** @} */
/** @} */
/** @} */
