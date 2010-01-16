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
 * \addtogroup licli Client
 * @{
 * \addtogroup LICliClient Client
 * @{
 */

#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#include <grapple/grapple.h>
#include <lipsofsuna/algorithm.h>
#include "client.h"
#include "client-types.h"
#include "client-object.h"

typedef struct _LICliControls LICliControls;
struct _LICliControls
{
	int flags;
	float tilt;
	float turn;
	float move;
};

struct _LICliNetwork
{
	int analog;
	int port;
	int udp;
	char* addr;
	uint32_t id;
	uint32_t features;
	LICliClient* client;
	grapple_client socket;
	struct
	{
		Uint32 tick;
		float movement;
		float rotation;
	} delta;
	struct
	{
		LICliControls controls;
		LIMatQuaternion direction;
	} prev, curr;
};

LICliNetwork*
licli_network_new (LICliClient* client,
                   const char*  addr,
                   int          port,
                   int          udp,
                   const char*  name,
                   const char*  pass);

void
licli_network_free (LICliNetwork* self);

void
licli_network_tilt (LICliNetwork* self,
                    float         value,
                    int           keep);

void
licli_network_turn (LICliNetwork* self,
                    float         value,
                    int           keep);

int
licli_network_update (LICliNetwork* self,
                      float         secs);

int
licli_network_get_connected (const LICliNetwork* self);

int
licli_network_get_dirty (const LICliNetwork* self);

void
licli_network_get_rotation (const LICliNetwork* self,
                            LIMatQuaternion*    value);

void
licli_network_set_rotation (LICliNetwork*          self,
                            const LIMatQuaternion* value);

#endif

/** @} */
/** @} */
