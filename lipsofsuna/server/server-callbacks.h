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
 * \addtogroup lisrvCallbacks Callbacks
 * @{
 */

#ifndef __SERVER_CALLBACKS_H__
#define __SERVER_CALLBACKS_H__

#include <engine/lips-engine.h>
#include "server.h"
#include "server-client.h"
#include "server-object.h"
#include "server-types.h"

enum
{
	LISRV_CALLBACK_CLIENT_CONTROL = LIENG_CALLBACK_LAST, /* (data, object, quat, flags) */
	LISRV_CALLBACK_CLIENT_LOGIN, /* (data, object, name, pass) */
	LISRV_CALLBACK_CLIENT_LOGOUT, /* (data, object) */
	LISRV_CALLBACK_CLIENT_PACKET, /* (data, client, packet) */
	LISRV_CALLBACK_OBJECT_ANIMATION, /* (data, object, animation) */
	LISRV_CALLBACK_OBJECT_CLIENT, /* (data, object) */
	LISRV_CALLBACK_OBJECT_SAMPLE, /* (data, object, sample, flags) */
	LISRV_CALLBACK_OBJECT_MOTION, /* (data, object) */
	LISRV_CALLBACK_TICK, /* (data, secs) */
	LISRV_CALLBACK_VISION_HIDE, /* (data, object, object) */
	LISRV_CALLBACK_VISION_SHOW, /* (data, object, object) */
};

int lisrv_server_init_callbacks_client (lisrvServer* server);

#endif

/** @} */
/** @} */

