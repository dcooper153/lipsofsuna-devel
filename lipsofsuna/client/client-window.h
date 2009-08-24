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
 * \addtogroup licliWindow Window
 * @{
 */

#ifndef __CLIENT_WINDOW_H__
#define __CLIENT_WINDOW_H__

#include <video/lips-video.h>
#include "client-types.h"

struct _licliWindow
{
	licliClient* client;
	SDL_Joystick* joystick;
	SDL_Surface* screen;
	struct
	{
		int width;
		int height;
		int fsaa;
	} mode;
};

licliWindow*
licli_window_new (licliClient* client);

void
licli_window_free (licliWindow* self);

void
licli_window_set_fsaa (licliWindow* self,
                       int          samples);

void
licli_window_get_size (const licliWindow* self,
                       int*               width,
                       int*               height);

int
licli_window_set_size (licliWindow* self,
                       int          width,
                       int          height);

#endif

/** @} */
/** @} */

