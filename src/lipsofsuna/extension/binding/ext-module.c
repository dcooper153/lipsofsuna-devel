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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtBinding Binding
 * @{
 */

#include <lipsofsuna/client.h>
#include "ext-module.h"

static int private_event (
	LIExtModule* self,
	SDL_Event*   event);

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_binding_info =
{
	LIMAI_EXTENSION_VERSION, "Binding",
	liext_bindings_new,
	liext_bindings_free
};

LIExtModule* liext_bindings_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Find the client component. Not finding this isn't a problem since
	   we can work in dummy mode. */
	self->client = limai_program_find_component (program, "client");

	/* Allocate binding manager. */
	self->bindings = libnd_manager_new ();
	if (self->bindings == NULL)
	{
		liext_bindings_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "event", 0, private_event, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_tick, self, self->calls + 1))
	{
		liext_bindings_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Action", liext_script_action, self);
	liscr_script_create_class (program->script, "Binding", liext_script_binding, self);

	return self;
}

void liext_bindings_free (
	LIExtModule* self)
{
	if (self->bindings != NULL)
		libnd_manager_free (self->bindings);
	lisys_free (self);
}

/*****************************************************************************/

static int private_event (
	LIExtModule* self,
	SDL_Event*   event)
{
	int cx;
	int cy;
	Uint32 mods = SDL_GetModState ();

	switch (event->type)
	{
		case SDL_JOYAXISMOTION:
			return !libnd_manager_event (self->bindings, LIBND_TYPE_JOYSTICK_AXIS,
				event->jaxis.axis, mods, event->jaxis.value / 32768.0f);
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return !libnd_manager_event (self->bindings, LIBND_TYPE_JOYSTICK,
				event->jbutton.button, mods, event->jbutton.state == SDL_PRESSED);
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			return !libnd_manager_event (self->bindings, LIBND_TYPE_KEYBOARD,
				event->key.keysym.sym, event->key.keysym.mod, event->key.state);
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			return !libnd_manager_event (self->bindings, LIBND_TYPE_MOUSE,
				event->button.button, mods, event->button.state == SDL_PRESSED);
		case SDL_MOUSEMOTION:
			if (self->client->moving)
			{
				cx = self->client->window->mode.width / 2;
				cy = self->client->window->mode.height / 2;
				if (event->motion.x != cx || event->motion.y != cy)
				{
					if (event->motion.x != cx)
					{
						libnd_manager_event (self->bindings, LIBND_TYPE_MOUSE_DELTA,
							0, mods, event->motion.x - cx);
					}
					if (event->motion.y != cy)
					{
						libnd_manager_event (self->bindings, LIBND_TYPE_MOUSE_DELTA,
							1, mods, event->motion.y - cy);
					}
				}
			}
			else
			{
				libnd_manager_event (self->bindings, LIBND_TYPE_MOUSE_AXIS, 0, mods,
					-1.0 + 2.0f * event->motion.x / (float) self->client->window->mode.width);
				libnd_manager_event (self->bindings, LIBND_TYPE_MOUSE_AXIS, 1, mods,
					-1.0 + 2.0f * event->motion.y / (float) self->client->window->mode.height);
			}
			break;
	}

	return 1;
}

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	int x;
	int y;
	int cx;
	int cy;

	if (!self->client->moving)
		return 1;

	/* Pointer warping in movement mode. */
	cx = self->client->window->mode.width / 2;
	cy = self->client->window->mode.height / 2;
	SDL_GetMouseState (&x, &y);
	if (x != cx || y != cy)
	{
		SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse (cx, cy);
		SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
	}

	return 1;
}

/** @} */
/** @} */
