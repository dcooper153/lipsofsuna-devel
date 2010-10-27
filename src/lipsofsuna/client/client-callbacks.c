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
 * \addtogroup LICli Client
 * @{
 * \addtogroup LICliCallbacks Callbacks
 * @{
 */

#include "client.h"
#include "client-callbacks.h"
#include "client-window.h"

static int private_event (
	LICliClient* client,
	SDL_Event*   event)
{
	char* str = NULL;

	switch (event->type)
	{
		case SDL_JOYAXISMOTION:
			limai_program_event (client->program, "joystickmotion",
				"axis", LISCR_TYPE_INT, event->jaxis.axis + 1,
				"value", LISCR_TYPE_INT, event->jaxis.value / 32768.0f, NULL);
			return 0;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			limai_program_event (client->program,
				(event->type == SDL_JOYBUTTONDOWN)? "joystickpress" : "joystickrelease",
				"button", LISCR_TYPE_INT, event->jbutton.button, NULL);
			return 0;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			limai_program_event (client->program,
				(event->type == SDL_MOUSEBUTTONDOWN)? "mousepress" : "mouserelease",
				"button", LISCR_TYPE_INT, event->button.button,
				"x", LISCR_TYPE_INT, event->button.x,
				"y", LISCR_TYPE_INT, event->button.y, NULL);
			return 0;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (event->key.keysym.unicode != 0)
				str = listr_wchar_to_utf8 (event->key.keysym.unicode);
			if (str != NULL)
			{
				limai_program_event (client->program,
					(event->type == SDL_KEYDOWN)? "keypress" : "keyrelease",
					"code", LISCR_TYPE_INT, event->key.keysym.sym,
					"mods", LISCR_TYPE_INT, event->key.keysym.mod,
					"text", LISCR_TYPE_STRING, str, NULL);
				lisys_free (str);
			}
			else
			{
				limai_program_event (client->program,
					(event->type == SDL_KEYDOWN)? "keypress" : "keyrelease",
					"code", LISCR_TYPE_INT, event->key.keysym.sym,
					"mods", LISCR_TYPE_INT, event->key.keysym.mod, NULL);
			}
			return 0;
		case SDL_MOUSEMOTION:
			limai_program_event (client->program, "mousemotion",
				"x", LISCR_TYPE_INT, event->motion.x,
				"y", LISCR_TYPE_INT, event->motion.y,
				"dx", LISCR_TYPE_INT, event->motion.xrel,
				"dy", LISCR_TYPE_INT, event->motion.yrel, NULL);
			return 0;
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

static int private_tick (
	LICliClient* client,
	float        secs)
{
	int x;
	int y;
	int cx;
	int cy;

	if (!client->moving)
		return 1;

	/* Pointer warping in movement mode. */
	cx = client->window->mode.width / 2;
	cy = client->window->mode.height / 2;
	SDL_GetMouseState (&x, &y);
	if (x != cx || y != cy)
	{
		SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse (cx, cy);
		SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
	}

	return 1;
}

int licli_client_init_callbacks_misc (
	LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->engine, "event", -5, private_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "tick", -5, private_tick, self, NULL);
	return 1;
}

/** @} */
/** @} */

