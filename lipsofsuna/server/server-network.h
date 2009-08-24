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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvNetwork Network
 * @{
 */

#ifndef __SERVER_NETWORK_H__
#define __SERVER_NETWORK_H__

#include <pthread.h>
#ifdef BUILTIN_GRAPPLE
#include <grapple.h>
#else
#include <grapple/grapple.h>
#endif
#include <algorithm/lips-algorithm.h>
#include "server.h"
#include "server-client.h"
#include "server-types.h"

struct _lisrvNetwork
{
	pthread_mutex_t mutex;
	grapple_server socket;
	lialgU32dic* clients;
	lialgStrdic* passwords;
	lisrvServer* server;
};

lisrvNetwork*
lisrv_network_new (lisrvServer* server,
                   int          udp,
                   int          port);

void
lisrv_network_free (lisrvNetwork* self);

void
lisrv_network_update (lisrvNetwork* self,
                      float         secs);

lisrvClient*
lisrv_network_find_client (lisrvNetwork* self,
                           grapple_user  user);

#endif

/** @} */
/** @} */
