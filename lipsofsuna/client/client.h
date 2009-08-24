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
 * \addtogroup licliClient Client
 * @{
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#ifndef LI_DISABLE_SOUND
#include <sound/lips-sound.h>
#endif
#include <video/lips-video.h>
#include "client-module.h"
#include "client-types.h"
#include "client-window.h"

#define LICLI_EVENT_TYPE_ACTION 0

struct _licliClient
{
	licliModule* module;
	licliWindow* window;
#ifndef LI_DISABLE_SOUND
	lisndSystem* sound;
#endif
	lividCalls video;
};

licliClient*
licli_client_new (lividCalls* video,
                  const char* path,
                  const char* name);

void
licli_client_free (licliClient* self);

int
licli_client_main (licliClient* self);

// FIXME
#define LI_CLIENT_ROTATION_SPEED 1.5f
#define LI_CLIENT_SELECT_MAXIMUM_COUNT 128
#define LI_CLIENT_SELECT_RECTANGLE_SIZE 1
extern const libndAction li_binding_actions[];
const libndAction* li_binding_action_get (const char* identifier);

#endif

/** @} */
/** @} */
