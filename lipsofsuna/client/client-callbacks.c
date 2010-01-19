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

#define MOUSE_DELTA_REFRESH 0.05f

/*****************************************************************************/
/* Bindings. */

static int
private_binding_event (LICliClient* client,
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
private_binding_tick (LICliClient* client,
                      float        secs)
{
	int x;
	int y;
	int cx;
	int cy;
	static float accum = 0.0f;

	if (!client->moving)
	{
		accum = 0.0f;
		return 1;
	}
	accum += secs;
	if (accum < MOUSE_DELTA_REFRESH)
		return 1;
	while (accum > MOUSE_DELTA_REFRESH)
		accum -= MOUSE_DELTA_REFRESH;

	/* Pointer state. */
	cx = client->window->mode.width / 2;
	cy = client->window->mode.height / 2;
	client->video.SDL_GetMouseState (&x, &y);
	if (x != cx || y != cy)
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
licli_client_init_callbacks_binding (LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->engine, "event", 0, private_binding_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "tick", 0, private_binding_tick, self, NULL);
	return 1;
}

/*****************************************************************************/
/* Miscellaneous. */

static void
private_camera_clip (LICliClient* client)
{
	int hit;
	float frac;
	LIMatAabb aabb;
	LIMatTransform start;
	LIMatTransform end;
	LIMatVector diff;
	LIPhyCollision tmp;
	LIPhyShape* shape;

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

static int
private_miscellaneous_tick (LICliClient* client,
                            float        secs)
{
	LIEngObject* player;
	LIMatAabb bounds;
	LIMdlNode* node;
	LIMatTransform transform;
	LIMatTransform transform0;

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
licli_client_init_callbacks_misc (LICliClient* self)
{
	lical_callbacks_insert (self->callbacks, self->engine, "event", -5, private_miscellaneous_event, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "packet", 0, licli_client_handle_packet, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "object-new", -65535, private_miscellaneous_object_new, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "tick", 0, private_miscellaneous_tick, self, NULL);
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

