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
 * \addtogroup LISer Server
 * @{
 * \addtogroup LISerCallbacks Callbacks
 * @{
 */

#include <lipsofsuna/network.h>
#include "server-callbacks.h"

static int private_object_free (
	LISerServer* server,
	LIEngObject* object)
{
	/* Unrealize before server data is freed. */
	lieng_object_set_realized (object, 0);

	return 1;
}

int liser_server_init_callbacks_client (
	LISerServer* server)
{
	lical_callbacks_insert (server->callbacks, server->engine, "object-free", 65535, private_object_free, server, NULL);
	return 1;
}

/** @} */
/** @} */
