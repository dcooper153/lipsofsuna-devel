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
/* Bindings. */

static int
private_binding_event (LICliClient* client,
                       SDL_Event*   event)
{
	int cx;
	int cy;

	switch (event->type)
	{
		case SDL_JOYAXISMOTION:
			return !libnd_manager_event (
				client->bindings,
				LIBND_TYPE_JOYSTICK_AXIS,
				event->jaxis.axis,
				client->video.SDL_GetModState (),
				event->jaxis.value / 32768.0f);
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return !libnd_manager_event (
				client->bindings,
				LIBND_TYPE_JOYSTICK,
				event->jbutton.button,
				client->video.SDL_GetModState (),
				event->jbutton.state == SDL_PRESSED);
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			return !libnd_manager_event (
				client->bindings,
				LIBND_TYPE_KEYBOARD,
				event->key.keysym.sym,
				event->key.keysym.mod,
				event->key.state);
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			return !libnd_manager_event (
				client->bindings,
				LIBND_TYPE_MOUSE,
				event->button.button,
				client->video.SDL_GetModState (),
				event->button.state == SDL_PRESSED);
		case SDL_MOUSEMOTION:
			if (client->moving)
			{
				cx = client->window->mode.width / 2;
				cy = client->window->mode.height / 2;
				if (event->motion.x != cx || event->motion.y != cy)
				{
					if (event->motion.x != cx)
					{
						libnd_manager_event (client->bindings, LIBND_TYPE_MOUSE_DELTA,
							0, client->video.SDL_GetModState (), event->motion.x - cx);
					}
					if (event->motion.y != cy)
					{
						libnd_manager_event (client->bindings, LIBND_TYPE_MOUSE_DELTA,
							1, client->video.SDL_GetModState (), event->motion.y - cy);
					}
				}
			}
			else
			{
				libnd_manager_event (
					client->bindings,
					LIBND_TYPE_MOUSE_AXIS, 0,
					client->video.SDL_GetModState (),
					-1.0 + 2.0f * event->motion.x / (float) client->window->mode.width);
				libnd_manager_event (
					client->bindings,
					LIBND_TYPE_MOUSE_AXIS, 1,
					client->video.SDL_GetModState (),
					-1.0 + 2.0f * event->motion.y / (float) client->window->mode.height);
			}
			break;
	}

	return 1;
}

static int
private_binding_tick (LICliClient* client,
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
	client->video.SDL_GetMouseState (&x, &y);
	if (x != cx || y != cy)
	{
		client->video.SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
		client->video.SDL_WarpMouse (cx, cy);
		client->video.SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
	}

	return 1;
}

int
licli_client_init_callbacks_binding (LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->engine, "event", 0, private_binding_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "tick", 0, private_binding_tick, self, NULL);
	return 1;
}

/*****************************************************************************/
/* Miscellaneous. */

static int
private_miscellaneous_event (LICliClient* client,
                             SDL_Event*   event)
{
	switch (event->type)
	{
		case SDL_QUIT:
			limai_program_shutdown (client->program);
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

static int
private_miscellaneous_object_new (LICliClient* client,
                                  LIEngObject* object)
{
	/* Set object mode. */
	lieng_object_set_smoothing (object, LICLI_OBJECT_POSITION_SMOOTHING, LICLI_OBJECT_ROTATION_SMOOTHING);
	lieng_object_set_userdata (object, client);

	/* Allocate script data. */
	object->script = liscr_data_new (client->script, object, LISCR_SCRIPT_OBJECT, lieng_object_free);
	if (object->script == NULL)
		return 0;

	return 1;
}

int
licli_client_init_callbacks_misc (LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->engine, "event", -5, private_miscellaneous_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "object-new", -65535, private_miscellaneous_object_new, self, NULL);
	return 1;
}

/*****************************************************************************/
/* Widgets. */

static void
private_widget_attach (LICliClient* client,
                       LIWdgWidget* widget,
                       LIWdgWidget* parent)
{
	if (widget->userdata != NULL)
	{
		if (parent != NULL)
			liscr_data_ref (widget->userdata, parent->userdata);
		else
			liscr_data_ref (widget->userdata, NULL);
	}
}

static void
private_widget_detach (LICliClient* client,
                       LIWdgWidget* widget,
                       int*         free)
{
	if (widget->userdata != NULL)
	{
		if (liscr_data_get_valid (widget->userdata))
		{
			if (widget->parent != NULL)
				liscr_data_unref (widget->userdata, widget->parent->userdata);
			else
				liscr_data_unref (widget->userdata, NULL);
		}
		*free = 0;
	}
}

static void
private_widget_free (LICliClient* client,
                     LIWdgWidget* widget)
{
	if (widget->userdata != NULL)
		liwdg_widget_detach (widget);
}

static int
private_widget_event (LICliClient* client,
                      SDL_Event*   event)
{
	if (client->moving)
		return 1;
	if (!liwdg_manager_event_sdl (client->widgets, event))
		return 1;
	return 0;
}

static int
private_widget_tick (LICliClient* client,
                     float        secs)
{
	liwdg_manager_update (client->widgets, secs);
	return 1;
}

int
licli_client_init_callbacks_widget (LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->widgets, "widget-attach", 5, private_widget_attach, self, NULL);
	lical_callbacks_insert (self->callbacks, self->widgets, "widget-detach", 5, private_widget_detach, self, NULL);
	lical_callbacks_insert (self->callbacks, self->widgets, "widget-free", 5, private_widget_free, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "event", -10, private_widget_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "tick", 1, private_widget_tick, self, NULL);
	return 1;
}

/** @} */
/** @} */

