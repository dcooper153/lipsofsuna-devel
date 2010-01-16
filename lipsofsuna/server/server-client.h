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
 * \addtogroup LISerClient Client
 * @{
 */

#ifndef __SERVER_CLIENT_H__
#define __SERVER_CLIENT_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/archive.h>
#include <lipsofsuna/script.h>
#include "server-callbacks.h"
#include "server-object.h"
#include "server-types.h"

struct _LISerClient
{
	LIAlgU32dic* vision;
	LICalHandle calls[5];
	LIEngObject* object;
	LISerServer* server;
	LISerClient* next;
	LISerClient* prev;
	uint8_t radius;
	struct
	{
		grapple_user user;
	} network;
};

LISerClient*
liser_client_new (LISerServer* server,
                  LIEngObject* object,
                  grapple_user user);

void
liser_client_free (LISerClient* self);

void
liser_client_send (LISerClient* self,
                   LIArcWriter* writer,
                   int          flags);

int
liser_client_get_near (const LISerClient* self,
                       const LIEngObject* object);

int
liser_client_set_object (LISerClient* self,
                         LIEngObject* value);

#endif

/** @} */
/** @} */
