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
 * \addtogroup liser Server
 * @{
 * \addtogroup LISerNetwork Network
 * @{
 */

#ifndef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include <pthread.h>
#include <grapple/grapple.h>
#include <lipsofsuna/algorithm.h>
#include "server.h"
#include "server-client.h"
#include "server-types.h"

struct _LISerNetwork
{
	pthread_mutex_t mutex;
	grapple_server socket;
	LIAlgU32dic* clients;
	LIAlgStrdic* passwords;
	LIArcWriter* resources;
	LISerServer* server;
};

LISerNetwork*
liser_network_new (LISerServer* server,
                   int          udp,
                   int          port);

void
liser_network_free (LISerNetwork* self);

void
liser_network_update (LISerNetwork* self,
                      float         secs);

LISerClient*
liser_network_find_client (LISerNetwork* self,
                           grapple_user  user);

#endif

/** @} */
/** @} */
