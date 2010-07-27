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

