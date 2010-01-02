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
private_binding_event (licliClient* client,
                       SDL_Event*   event)
{
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
			break;
	}

	return 1;
}

static int
private_binding_tick (licliClient* client,
                      float        secs)
{
	int x;
	int y;
	int cx;
	int cy;

	if (!client->moving)
		return 1;

	/* Pointer state. */
	cx = client->window->mode.width / 2;
	cy = client->window->mode.height / 2;
	client->video.SDL_GetMouseState (&x, &y);
	client->video.SDL_WarpMouse (cx, cy);

	/* Cursor delta events. */
	if (x != cx)
	{
		libnd_manager_event (client->bindings, LIBND_TYPE_MOUSE_DELTA,
			0, client->video.SDL_GetModState (), x - cx);
	}
	if (y != cy)
	{
		libnd_manager_event (client->bindings, LIBND_TYPE_MOUSE_DELTA,
			1, client->video.SDL_GetModState (), y - cy);
	}

	return 1;
}

int
licli_client_init_callbacks_binding (licliClient* self)
{
	lical_callbacks_insert (self->callbacks, self, "event", 0, private_binding_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self, "tick", 0, private_binding_tick, self, NULL);
	return 1;
}

/*****************************************************************************/
/* Miscellaneous. */

static void
private_camera_clip (licliClient* client)
{
	int hit;
	float frac;
	limatAabb aabb;
	limatTransform start;
	limatTransform end;
	limatVector diff;
	liphyCollision tmp;
	liphyShape* shape;

	/* Create sweep shape. */
	/* FIXME: Could use a more accurate shape. */
	lialg_camera_get_bounds (client->camera, &aabb);
	shape = liphy_shape_new_aabb (client->engine->physics, &aabb);
	if (shape == NULL)
		return;

	/* Sweep the shape. */
	lialg_camera_get_center (client->camera, &start);
	lialg_camera_get_transform (client->camera, &end);
	diff = limat_vector_subtract (end.position, start.position);
	hit = liphy_physics_cast_shape (client->engine->physics, &start, &end, shape,
		LICLI_PHYSICS_GROUP_CAMERA, LIPHY_GROUP_STATICS | LIPHY_GROUP_TILES, NULL, 0, &tmp);
	liphy_shape_free (shape);

	/* Clip the camera. */
	if (hit)
	{
		frac = tmp.fraction * limat_vector_get_length (diff);
		lialg_camera_clip (client->camera, frac);
	}
}

static int
private_miscellaneous_event (licliClient* client,
                             SDL_Event*   event)
{
	switch (event->type)
	{
		case SDL_QUIT:
			client->quit = 1;
			break;
		case SDL_ACTIVEEVENT:
			/*if (event.active.state & SDL_APPINPUTFOCUS)
				active = event.active.gain;*/
			break;
		case SDL_VIDEORESIZE:
			if (!licli_window_set_size (client->window, event->resize.w, event->resize.h))
				return 1;
			break;
	}

	return 1;
}

static int
private_miscellaneous_object_new (licliClient* client,
                                  liengObject* object)
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

static int
private_miscellaneous_tick (licliClient* client,
                            float        secs)
{
	liengObject* player;
	limatAabb bounds;
	limdlNode* node;
	limatTransform transform;
	limatTransform transform0;

	/* Update script. */
	liscr_script_update (client->script, secs);

	/* Update network state. */
	if (client->network != NULL)
	{
		if (!licli_network_update (client->network, secs))
			return 0;
	}

	/* Update player transformation. */
	player = licli_client_get_player (client);
	if (player != NULL && client->network != NULL)
	{
		lieng_object_get_target (player, &transform);
		transform.rotation = client->network->curr.direction;
		lieng_object_set_transform (player, &transform);
	}

	/* Update engine state. */
	lieng_engine_update (client->engine, secs);

	/* Update camera center. */
	if (player != NULL && client->network != NULL)
	{
		lieng_object_get_transform (player, &transform);
		node = lieng_object_find_node (player, client->camera_node);
		if (node != NULL && lialg_camera_get_driver (client->camera) == LIALG_CAMERA_FIRSTPERSON)
		{
			limdl_node_get_world_transform (node, &transform0);
			transform = limat_transform_multiply (transform, transform0);
		}
		else
		{
			lieng_object_get_bounds (player, &bounds);
			transform.position.y += bounds.max.y;
		}
		lialg_camera_set_center (client->camera, &transform);
		lialg_camera_update (client->camera, secs);
		if (lialg_camera_get_driver (client->camera) != LIALG_CAMERA_FIRSTPERSON)
			private_camera_clip (client);
	}

	return 1;
}

int
licli_client_init_callbacks_misc (licliClient* self)
{
	lical_callbacks_insert (self->callbacks, self, "event", -5, private_miscellaneous_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self, "packet", 0, licli_client_handle_packet, self, NULL);
	lical_callbacks_insert (self->callbacks, self, "object-new", -65535, private_miscellaneous_object_new, self, NULL);
	lical_callbacks_insert (self->callbacks, self, "tick", 0, private_miscellaneous_tick, self, NULL);
	return 1;
}

/*****************************************************************************/
/* Widgets. */

static int
private_widget_event (licliClient* client,
                      SDL_Event*   event)
{
	if (client->moving)
		return 1;
	if (!liwdg_manager_event_sdl (client->widgets, event))
		return 1;
	return 0;
}

static int
private_widget_tick (licliClient* client,
                     float        secs)
{
	liwdg_manager_update (client->widgets, secs);
	return 1;
}

int
licli_client_init_callbacks_widget (licliClient* self)
{
	lical_callbacks_insert (self->callbacks, self, "event", -10, private_widget_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self, "tick", 1, private_widget_tick, self, NULL);
	return 1;
}

/** @} */
/** @} */

