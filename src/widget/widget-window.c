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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup liwdgWindow Window
 * @{
 */

#include <class/lips-class.h>
#include "widget-label.h"
#include "widget-window.h"

#define LIWDG_WINDOW_DEFAULT_MARGIN 5
#define LIWDG_WINDOW_TITLEBAR_HEIGHT 10.0f

static int
private_init (liwdgWindow*  self,
              liwdgManager* manager);

static void
private_free (liwdgWindow* self);

static int
private_event (liwdgWindow* self,
               liwdgEvent*  event);

const liwdgWidgetClass liwdgWindowType =
{
	LI_CLASS_BASE_STATIC, &liwdgGroupType, "Window", sizeof (liwdgWindow),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/*****************************************************************************/

liwdgWidget*
liwdg_window_new (liwdgManager* manager,
                  int           width,
                  int           height)
{
	liwdgWidget* self;

	self = li_instance_new (&liwdgWindowType, manager);
	if (self == NULL)
		return NULL;
	if (!liwdg_group_set_size (LIWDG_GROUP (self), width, height))
	{
		liwdg_widget_free (self);
		return NULL;
	}

	return self;
}

const char*
liwdg_window_get_title (const liwdgWindow* self)
{
	return LIWDG_LABEL (self->label_title)->string;
}

void
liwdg_window_set_title (liwdgWindow* self,
                        const char*  title)
{
	liwdg_label_set_text (LIWDG_LABEL (self->label_title), title);
}

/*****************************************************************************/

static int
private_init (liwdgWindow*  self,
              liwdgManager* manager)
{
	self->label_title = liwdg_label_new (manager);
	if (self->label_title == NULL)
		return 0;
	liwdg_group_set_margins (LIWDG_GROUP (self),
		LIWDG_WINDOW_DEFAULT_MARGIN, LIWDG_WINDOW_DEFAULT_MARGIN,
		LIWDG_WINDOW_DEFAULT_MARGIN, LIWDG_WINDOW_DEFAULT_MARGIN);
	return 1;
}

static void
private_free (liwdgWindow* self)
{
}

static int
private_event (liwdgWindow* self,
               liwdgEvent*  event)
{
	liwdgRect rect;
	liwdgRect frame;
	liwdgSize size;
	liwdgSubimage* subimg;

	if (event->type == LIWDG_EVENT_TYPE_RENDER)
	{
		/* Render frame. */
		if (!LIWDG_WIDGET (self)->transparent)
		{
			liwdg_widget_get_style_request (LIWDG_WIDGET (self), "window", &size);
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			frame = rect;
			frame.width = size.width;
			frame.height = size.height;
			subimg = lialg_strdic_find (LIWDG_WIDGET (self)->manager->subimgs, "window");
			if (subimg != NULL)
			{
				frame.x -= subimg->pad[1];
				frame.y -= subimg->pad[3];
				liwdg_widget_paint (LIWDG_WIDGET (self), "window", &frame);
				glColor3f (1.0f, 1.0f, 1.0f);
				liwdg_widget_get_request (LIWDG_WIDGET (self->label_title), &size);
				liwdg_widget_set_allocation (self->label_title,
					frame.x + 2,
					frame.y + frame.height - subimg->pad[0] - 2,
					size.width,
					size.height);
				liwdg_widget_render (self->label_title);
			}
		}
	}

	/* Call base class. */
	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
