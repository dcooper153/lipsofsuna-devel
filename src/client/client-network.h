/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup licliClient Client
 * @{
 */

#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#ifdef BUILTIN_GRAPPLE
#include <grapple.h>
#else
#include <grapple/grapple.h>
#endif
#include <config/lips-config.h>
#include <algorithm/lips-algorithm.h>
#include "client-module.h"
#include "client-types.h"
#include "client-object.h"

struct _licliNetwork
{
	int analog;
	uint32_t id;
	uint32_t features;
	licfgHost* host;
	licliModule* module;
	grapple_client client;
	struct
	{
		Uint32 tick;
		float movement;
		float rotation;
	} delta;
	struct
	{
		licliControls controls;
		limatQuaternion direction;
	} prev, curr;
};

licliNetwork*
licli_network_new (licliModule* module,
                   const char*  name,
                   const char*  pass);

void
licli_network_free (licliNetwork* self);

void
licli_network_tilt (licliNetwork* self,
                    float         value,
                    int           keep);

void
licli_network_turn (licliNetwork* self,
                    float         value,
                    int           keep);

int
licli_network_update (licliNetwork* self,
                      float         secs);

int
licli_network_get_connected (const licliNetwork* self);

int
licli_network_get_dirty (const licliNetwork* self);

void
licli_network_get_rotation (const licliNetwork* self,
                            limatQuaternion*    value);

void
licli_network_set_rotation (licliNetwork*          self,
                            const limatQuaternion* value);

#endif

/** @} */
/** @} */
