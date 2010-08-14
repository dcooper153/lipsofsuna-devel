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
 * \addtogroup licliCallbacks Callbacks
 * @{
 */

#include "client.h"
#include "client-callbacks.h"
#include "client-window.h"

/*****************************************************************************/
/* Miscellaneous. */

static int
private_miscellaneous_event (LICliClient* client,
                             SDL_Event*   event)
{
	switch (event->type)
	{
		case SDL_QUIT:
			limai_program_event (client->program, "quit", NULL);
			break;
		case SDL_ACTIVEEVENT:
			if (event->active.state & SDL_APPINPUTFOCUS)
				client->program->sleep = 5000;
			else
				client->program->sleep = 0;
			break;
		case SDL_VIDEORESIZE:
			if (!licli_window_set_size (client->window, event->resize.w, event->resize.h))
				return 1;
			break;
	}

	return 1;
}

int
licli_client_init_callbacks_misc (LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->engine, "event", -5, private_miscellaneous_event, self, NULL);
	return 1;
}

/** @} */
/** @} */

