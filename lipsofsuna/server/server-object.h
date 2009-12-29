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
 * \addtogroup lisrvObject Object
 * @{
 */

#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <archive/lips-archive.h>
#include <engine/lips-engine.h>
#include <script/lips-script.h>
#include <string/lips-string.h>
#include "server-types.h"

#define LISRV_OBJECT(o) ((lisrvObject*) lieng_object_get_userdata ((liengObject*)(o), LIENG_DATA_SERVER))

struct _lisrvObject
{
	uint8_t flags;
	lialgU32dic* animations;
	lisrvClient* client;
	lisrvServer* server;
};

int
lisrv_object_animate (liengObject* self,
                      const char*  name,
                      int          channel,
                      float        priority,
                      int          permanent);

void
lisrv_object_disconnect (liengObject* self);

void
lisrv_object_effect (liengObject* self,
                     const char*  value,
                     int          flags);

int
lisrv_object_moved (liengObject* self);

int
lisrv_object_purge (liengObject* self);

int
lisrv_object_sees (const liengObject* self,
                   const liengObject* target);

int
lisrv_object_serialize (liengObject* self,
                        int          save);

void
lisrv_object_swap (liengObject* self,
                   liengObject* object);

int
lisrv_object_set_client (liengObject* self,
                         lisrvClient* value);

#endif

/** @} */
/** @} */
