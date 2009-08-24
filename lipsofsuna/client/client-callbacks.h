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
 * \addtogroup licliCallbacks Callbacks
 * @{
 */

#ifndef __CLIENT_CALLBACKS_H__
#define __CLIENT_CALLBACKS_H__

#include <render/lips-render.h>
#include "client-module.h"
#include "client-types.h"

enum
{
	LICLI_CALLBACK_EVENT = 32, /* (data, event) */
	LICLI_CALLBACK_PACKET, /* (data, type, packet) */
	LICLI_CALLBACK_SELECT, /* (data, selection) */
	LICLI_CALLBACK_TICK, /* (data, secs) */
	LICLI_CALLBACK_VISIBILITY, /* (data, object) */
};

int licli_module_init_callbacks_binding (licliModule* self);
int licli_module_init_callbacks_misc (licliModule* self);
int licli_module_init_callbacks_widget (licliModule* self);
int licli_render_init (licliModule* self);

#endif

/** @} */
/** @} */

