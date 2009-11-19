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

#include "client.h"
#include "client-callbacks.h"
#include "client-window.h"

/*****************************************************************************/
/* Bindings. */

static int
private_binding_event (licliModule* module,
                       SDL_Event*   event)
{
	switch (event->type)
	{
		case SDL_JOYAXISMOTION:
			return !libnd_manager_event (
				module->bindings,
				LIBND_TYPE_JOYSTICK_AXIS,
				event->jaxis.axis,
				module->client->video.SDL_GetModState (),
				event->jaxis.value / 32768.0f);
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			return !libnd_manager_event (
				module->bindings,
				LIBND_TYPE_JOYSTICK,
				event->jbutton.button,
				module->client->video.SDL_GetModState (),
				event->jbutton.state == SDL_PRESSED);
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			return !libnd_manager_event (
				module->bindings,
				LIBND_TYPE_KEYBOARD,
				event->key.keysym.sym,
				event->key.keysym.mod,
				event->key.state);
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			return !libnd_manager_event (
				module->bindings,
				LIBND_TYPE_MOUSE,
				event->button.button,
				module->client->video.SDL_GetModState (),
				event->button.state == SDL_PRESSED);
		case SDL_MOUSEMOTION:
			libnd_manager_event (
				module->bindings,
				LIBND_TYPE_MOUSE_AXIS, 0,
				module->client->video.SDL_GetModState (),
				-1.0 + 2.0f * event->motion.x / (float) module->window->mode.width);
			libnd_manager_event (
				module->bindings,
				LIBND_TYPE_MOUSE_AXIS, 1,
				module->client->video.SDL_GetModState (),
				-1.0 + 2.0f * event->motion.y / (float) module->window->mode.height);
			break;
	}

	return 1;
}

static int
private_binding_tick (licliModule* module,
                      float        secs)
{
	int x;
	int y;
	int cx;
	int cy;

	if (!module->moving)
		return 1;

	/* Pointer state. */
	cx = module->window->mode.width / 2;
	cy = module->window->mode.height / 2;
	module->client->video.SDL_GetMouseState (&x, &y);
	module->client->video.SDL_WarpMouse (cx, cy);

	/* Cursor delta events. */
	if (x != cx)
	{
		libnd_manager_event (module->bindings, LIBND_TYPE_MOUSE_DELTA,
			0, module->client->video.SDL_GetModState (), x - cx);
	}
	if (y != cy)
	{
		libnd_manager_event (module->bindings, LIBND_TYPE_MOUSE_DELTA,
			1, module->client->video.SDL_GetModState (), y - cy);
	}

	return 1;
}

int
licli_module_init_callbacks_binding (licliModule* self)
{
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_EVENT, 0, private_binding_event, self, NULL);
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_TICK, 0, private_binding_tick, self, NULL);
	return 1;
}

/*****************************************************************************/
/* Miscellaneous. */

static int
private_miscellaneous_event (licliModule* module,
                             SDL_Event*   event)
{
	switch (event->type)
	{
		case SDL_QUIT:
			module->quit = 1;
			break;
		case SDL_ACTIVEEVENT:
			/*if (event.active.state & SDL_APPINPUTFOCUS)
				active = event.active.gain;*/
			break;
		case SDL_VIDEORESIZE:
			if (!licli_window_set_size (module->window, event->resize.w, event->resize.h))
				return 1;
			break;
	}

	return 1;
}

static int
private_miscellaneous_tick (licliModule* module,
                            float        secs)
{
	liengObject* player;
	limatAabb bounds;
	limatTransform transform;

	/* Update script. */
	liscr_script_update (module->script, secs);

	/* Update network state. */
	if (module->network != NULL)
	{
		if (!licli_network_update (module->network, secs))
			return 0;
	}

	/* Update engine state. */
	lieng_engine_update (module->engine, secs);

	/* Check for player. */
	player = licli_module_get_player (module);
	if (player != NULL && module->network != NULL)
	{
		/* Update player transform. */
		lieng_object_get_transform (player, &transform);
		transform.rotation = module->network->curr.direction;
		lieng_object_set_transform (player, &transform);

		/* Update camera center. */
		lieng_object_get_bounds (player, &bounds);
		transform.position.y += bounds.max.y;
		lieng_camera_set_center (module->camera, &transform);
		lieng_camera_update (module->camera, secs);
	}

	return 1;
}

int
licli_module_init_callbacks_misc (licliModule* self)
{
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_EVENT, -5, private_miscellaneous_event, self, NULL);
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_PACKET, 0, licli_module_handle_packet, self, NULL);
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_TICK, 0, private_miscellaneous_tick, self, NULL);
	return 1;
}

/*****************************************************************************/
/* Widgets. */

static int
private_widget_event (licliModule* module,
                      SDL_Event*   event)
{
	if (module->moving)
		return 1;
	if (!liwdg_manager_event_sdl (module->widgets, event))
		return 1;
	return 0;
}

static int
private_widget_tick (licliModule* module,
                     float        secs)
{
	liwdg_manager_update (module->widgets, secs);
	return 1;
}

int
licli_module_init_callbacks_widget (licliModule* self)
{
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_EVENT, -10, private_widget_event, self, NULL);
	lieng_engine_insert_call (self->engine, LICLI_CALLBACK_TICK, 1, private_widget_tick, self, NULL);
	return 1;
}

/** @} */
/** @} */

