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
 * \addtogroup liextsrvSkeleton Skeleton
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/server.h>
#include "ext-skeleton.h"

static int
private_client_packet (LIExtSkeleton* self,
                       LISerClient*   client,
                       LIArcReader*   packet);

/*****************************************************************************/

LIExtSkeleton*
liext_skeleton_new (LISerServer* server)
{
	LIExtSkeleton* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtSkeleton));
	if (self == NULL)
		return NULL;
	self->server = server;

	/* Register callbacks. */
	if (!lical_callbacks_insert (server->callbacks, server->engine, "client-packet", 1, private_client_packet, self, self->calls + 0))
	{
		liext_skeleton_free (self);
		return NULL;
	}

	return self;
}

void
liext_skeleton_free (LIExtSkeleton* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int
private_client_packet (LIExtSkeleton* self,
                       LISerClient*   client,
                       LIArcReader*   packet)
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
