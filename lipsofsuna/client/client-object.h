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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliObject Object
 * @{
 */

#ifndef __CLIENT_OBJECT_H__
#define __CLIENT_OBJECT_H__

#include <algorithm/lips-algorithm.h>
#include <engine/lips-engine.h>
#include <math/lips-math.h>
#include <network/lips-network.h>
#include <render/lips-render.h>
#include <system/lips-system.h>
#include "client-types.h"

#define LICLI_OBJECT(o) ((licliObject*) lieng_object_get_userdata ((liengObject*)(o), LIENG_DATA_CLIENT))

typedef struct _licliControls licliControls;
struct _licliControls
{
	int flags;
	float tilt;
	float turn;
	float move;
};

struct _licliObject
{
	uint16_t animation;
	lialgList* speech;
	lialgList* sounds;
	licliModule* module;
};

liengObject*
licli_object_new (licliModule* module,
                  uint32_t     id,
                  int          flags);

void
licli_object_free (liengObject* self);

void
licli_object_update (liengObject* self,
                     float        secs);

void
licli_object_set_animation (liengObject* self,
                            int          value,
                            int          channel,
                            int          permanent,
                            float        priority);

void
licli_object_set_controls (liengObject*         self,
                           const licliControls* controls);

void
licli_object_set_effect (liengObject* self,
                         uint32_t     id,
                         int          flags);

void
licli_object_set_speech (liengObject* self,
                         const char*  message);

#endif

/** @} */
/** @} */
