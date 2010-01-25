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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgBusy Busy
 * @{
 */

#include "widget-busy.h"
#include "widget-button.h"
#include "widget-progress.h"

static int
private_init (LIWdgBusy*    self,
              LIWdgManager* manager);

static void
private_free (LIWdgBusy* self);

static void
private_event (LIWdgBusy*  self,
               LIWdgEvent* event);

const LIWdgClass liwdg_widget_busy =
{
	LIWDG_BASE_STATIC, &liwdg_widget_window, "Busy", sizeof (LIWdgBusy),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event,
};

/****************************************************************************/

LIWdgWidget*
liwdg_busy_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdg_widget_busy);
}

void
liwdg_busy_set_cancel (LIWdgBusy*   self,
                       liwdgHandler handler,
                       void*        data)
{
	liwdg_widget_insert_callback (self->button, "pressed", handler, data);
	liwdg_widget_set_visible (self->button, (handler != NULL));
}

void
liwdg_busy_set_progress (LIWdgBusy* self,
                         float      value)
{
	liwdg_progress_set_value (LIWDG_PROGRESS (self->progress), value);
}

void
liwdg_busy_set_text (LIWdgBusy*  self,
                     const char* value)
{
	liwdg_progress_set_text (LIWDG_PROGRESS (self->progress), value);
}

void
liwdg_busy_set_update (LIWdgBusy*   self,
                       liwdgHandler handler,
                       void*        data)
{
	self->update_func = handler;
	self->update_data = data;
}

/****************************************************************************/

static int
private_init (LIWdgBusy*   self,
              LIWdgManager* manager)
{
	int i;
	LIWdgWidget* widgets[] =
	{
		liwdg_progress_new (manager),
		liwdg_button_new (manager),
	};

	/* Check memory. */
	if (!liwdg_group_set_size (LIWDG_GROUP (self), 1, 2))
		goto error;
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (LIWdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			goto error;
	}

	/* Assign widgets. */
	self->progress = widgets[(i = 0)];
	self->button = widgets[++i];

	/* Pack self. */
	liwdg_button_set_text (LIWDG_BUTTON (self->button), "Cancel");
	liwdg_widget_set_request (self->progress, 100, -1);
	liwdg_widget_set_visible (self->button, 0);
	liwdg_window_set_title (LIWDG_WINDOW (self), "Busy...");
	liwdg_group_set_margins (LIWDG_GROUP (self), 5, 5, 5, 5);
	liwdg_group_set_spacings (LIWDG_GROUP (self), 5, 5);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 0, self->progress);
	liwdg_group_set_child (LIWDG_GROUP (self), 0, 1, self->button);

	return 1;

error:
	for (i = 0 ; i < (int)(sizeof (widgets) / sizeof (LIWdgWidget*)) ; i++)
	{
		if (widgets[i] == NULL)
			liwdg_widget_free (widgets[i]);
	}
	return 0;
}

static void
private_free (LIWdgBusy* self)
{
}

static void
private_event (LIWdgBusy*  self,
               LIWdgEvent* event)
{
	/* Call update callback. */
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		if (self->update_func != NULL)
			self->update_func (self->update_data, event->update.secs);
	}

	/* Call base class. */
	liwdg_widget_window.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */

