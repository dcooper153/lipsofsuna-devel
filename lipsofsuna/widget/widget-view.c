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
 * \addtogroup liwdgView View
 * @{
 */

#include "widget-view.h"

static int
private_init (liwdgView*    self,
              liwdgManager* manager);

static void
private_free (liwdgView* self);

static int
private_event (liwdgView*  self,
               liwdgEvent* event);

static liwdgWidget*
private_child_at (liwdgView* self,
                  int        pixx,
                  int        pixy);

static void
private_child_request (liwdgView*   self,
                       liwdgWidget* child);

static liwdgWidget*
private_cycle_focus (liwdgView*   self,
                     liwdgWidget* curr,
                     int          next);

static void
private_detach_child (liwdgView*   self,
                      liwdgWidget* child);

static void
private_foreach_child (liwdgView* self,
                       void     (*call)(),
                       void*      data);

static void
private_rebuild (liwdgView* self);

static void
private_scroll (liwdgView* self,
                int        x,
                int        y);

/*****************************************************************************/

const liwdgClass liwdgViewType =
{
	LIWDG_BASE_STATIC, &liwdgContainerType, "View", sizeof (liwdgView),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event,
};

liwdgWidget*
liwdg_view_new (liwdgManager* manager)
{
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liwdgViewType);
	if (self == NULL)
		return NULL;
	private_rebuild (LIWDG_VIEW (self));

	return self;
}

liwdgWidget*
liwdg_view_get_child (liwdgView* self)
{
	return self->child;
}

void
liwdg_view_set_child (liwdgView*   self,
                      liwdgWidget* widget)
{
	/* Adopt widget. */
	self->child = widget;
	if (widget != NULL)
		widget->parent = LIWDG_WIDGET (self);

	/* Rebuild request. */
	self->hscrollpos = 0;
	self->vscrollpos = 0;
	private_rebuild (self);
}


void
liwdg_view_set_hscroll (liwdgView* self,
                        int        value)
{
	self->hscroll = value;
	private_rebuild (self);
}

void
liwdg_view_set_vscroll (liwdgView* self,
                        int        value)
{
	self->vscroll = value;
	private_rebuild (self);
}

/*****************************************************************************/

static int
private_init (liwdgView*    self,
              liwdgManager* manager)
{
	return 1;
}

static void
private_free (liwdgView* self)
{
	if (self->child != NULL)
		liwdg_widget_free (self->child);
}

static int
private_event (liwdgView*  self,
               liwdgEvent* event)
{
	liwdgRect rect;

	/* Container interface. */
	if (event->type == LIWDG_EVENT_TYPE_PROBE &&
	    event->probe.clss == &liwdgContainerType)
	{
		static liwdgContainerIface iface =
		{
			(liwdgContainerChildAtFunc) private_child_at,
			(liwdgContainerChildRequestFunc) private_child_request,
			(liwdgContainerCycleFocusFunc) private_cycle_focus,
			(liwdgContainerDetachChildFunc) private_detach_child,
			(liwdgContainerForeachChildFunc) private_foreach_child
		};
		event->probe.result = &iface;
		return 0;
	}

	switch (event->type)
	{
		/* Mouse coordinates need translation. */
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			if (event->button.button == 4)
			{
				private_scroll (self, 0, 32);
				return 0;
			}
			else if (event->button.button == 5)
			{
				private_scroll (self, 0, -32);
				return 0;
			}
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "view", &rect);
			event->button.x -= rect.x - self->hscrollpos;
			event->button.y -= rect.y - self->vscrollpos;
			break;
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "view", &rect);
			event->button.x -= rect.x - self->hscrollpos;
			event->button.y -= rect.y - self->vscrollpos;
			break;
		case LIWDG_EVENT_TYPE_MOTION:
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "view", &rect);
			event->motion.x -= rect.x - self->hscrollpos;
			event->motion.y -= rect.y - self->vscrollpos;
			break;

		case LIWDG_EVENT_TYPE_ALLOCATION:
			private_rebuild (self);
			break;
		case LIWDG_EVENT_TYPE_RENDER:
			/* Draw base. */
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "view", &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), "view", NULL);
			/* Draw child. */
			if (self->child != NULL)
			{
				glScissor (rect.x, rect.y, rect.width, rect.height);
				glEnable (GL_SCISSOR_TEST);
				glPushMatrix ();
				glTranslatef (rect.x - self->hscrollpos, rect.y - self->vscrollpos, 0.0f);
				liwdg_widget_render (self->child);
				glPopMatrix ();
				glDisable (GL_SCISSOR_TEST);
			}
			return 0;
		case LIWDG_EVENT_TYPE_UPDATE:
			if (self->child != NULL)
				liwdg_widget_update (self->child, event->update.secs);
			return 1;
	}

	return liwdgContainerType.event (LIWDG_WIDGET (self), event);
}

static liwdgWidget*
private_child_at (liwdgView* self,
                  int        pixx,
                  int        pixy)
{
	if (liwdg_widget_get_visible (self->child))
		return self->child;

	return NULL;
}

static void
private_child_request (liwdgView*   self,
                       liwdgWidget* child)
{
	private_rebuild (self);
}

static liwdgWidget*
private_cycle_focus (liwdgView*   self,
                     liwdgWidget* curr,
                     int          next)
{
	liwdgWidget* tmp;

	/* Cycle focus. */
	if (liwdg_widget_typeis (self->child, &liwdgContainerType))
	{
		tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (self->child), NULL, next);
		if (tmp != NULL)
			return tmp;
	}
	else
	{
		if (liwdg_widget_get_focusable (self->child))
			return self->child;
	}

	return NULL;
}

static void
private_detach_child (liwdgView*   self,
                      liwdgWidget* child)
{
	assert (self->child == child);

	self->child = NULL;
	private_rebuild (self);
}

static void
private_foreach_child (liwdgView* self,
                       void     (*call)(),
                       void*      data)
{
	if (self->child != NULL)
		call (data, self->child);
}

static void
private_rebuild (liwdgView* self)
{
	liwdgRect rect;
	liwdgSize size;
	liwdgSize size1;

	/* Calculate own request. */
	size.width = size1.width = 0;
	size.height = size1.height = 0;
	if (self->child != NULL && liwdg_widget_get_visible (self->child))
	{
		liwdg_widget_get_request (self->child, &size);
		size1 = size;
	}
	if (self->hscroll) size1.width = 0;
	if (self->vscroll) size1.height = 0;
	liwdg_widget_set_style_request (LIWDG_WIDGET (self), size1.width, size1.height, "view");

	/* Allocate virtual rectangle for child. */
	if (self->child != NULL)
	{
		liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "view", &rect);
		rect.width = LI_MAX (size.width, rect.width);
		rect.height = LI_MAX (size.height, rect.height);
		liwdg_widget_set_allocation (self->child, 0, 0, rect.width, rect.height);
	}

	/* Validate scroll position. */
	private_scroll (self, 0, 0);
}

static void
private_scroll (liwdgView* self,
                int        x,
                int        y)
{
	liwdgRect rect;

	liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "view", &rect);
	self->hscrollpos += x;
	self->vscrollpos += y;
	if (self->child != NULL)
	{
		if (self->hscrollpos > self->child->allocation.width - rect.width)
			self->hscrollpos = self->child->allocation.width - rect.width;
		if (self->hscrollpos < 0)
			self->hscrollpos = 0;
		if (self->vscrollpos > self->child->allocation.height - rect.height)
			self->vscrollpos = self->child->allocation.height - rect.height;
		if (self->vscrollpos < 0)
			self->vscrollpos = 0;
	}
	else
	{
		self->vscrollpos = 0;
		self->hscrollpos = 0;
	}
}

/** @} */
/** @} */
