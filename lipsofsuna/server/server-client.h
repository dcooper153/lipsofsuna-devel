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
 * \addtogroup lisrvClient Client
 * @{
 */

#ifndef __SERVER_CLIENT_H__
#define __SERVER_CLIENT_H__

#include <algorithm/lips-algorithm.h>
#include <archive/lips-archive.h>
#include <config/lips-config.h>
#include <script/lips-script.h>
#include "server-callbacks.h"
#include "server-object.h"
#include "server-types.h"

struct _lisrvClient
{
	lialgU32dic* vision;
	licalHandle calls[5];
	liengObject* object;
	lisrvServer* server;
	lisrvClient* next;
	lisrvClient* prev;
	uint8_t radius;
	struct
	{
		grapple_user user;
	} network;
};

lisrvClient*
lisrv_client_new (lisrvServer* server,
                  liengObject* object,
                  grapple_user user);

void
lisrv_client_free (lisrvClient* self);

void
lisrv_client_send (lisrvClient* self,
                   liarcWriter* writer,
                   int          flags);

int
lisrv_client_get_near (const lisrvClient* self,
                       const liengObject* object);

int
lisrv_client_set_object (lisrvClient* self,
                         liengObject* value);

#endif

/** @} */
/** @} */
