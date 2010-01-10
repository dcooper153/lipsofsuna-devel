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
 * \addtogroup LIWdgView View
 * @{
 */

#include "widget-view.h"

static int
private_init (LIWdgView*    self,
              LIWdgManager* manager);

static void
private_free (LIWdgView* self);

static int
private_event (LIWdgView*  self,
               liwdgEvent* event);

static LIWdgWidget*
private_child_at (LIWdgView* self,
                  int        pixx,
                  int        pixy);

static void
private_child_request (LIWdgView*   self,
                       LIWdgWidget* child);

static LIWdgWidget*
private_cycle_focus (LIWdgView*   self,
                     LIWdgWidget* curr,
                     int          next);

static void
private_detach_child (LIWdgView*   self,
                      LIWdgWidget* child);

static void
private_foreach_child (LIWdgView* self,
                       void     (*call)(),
                       void*      data);

static void
private_call_detach (LIWdgView* self);

static void
private_rebuild (LIWdgView* self);

static void
private_scroll (LIWdgView* self,
                int        x,
                int        y);

static void
private_translate_coords (LIWdgView* self,
                          int        containerx,
                          int        containery,
                          int*       childx,
                          int*       childy);

/*****************************************************************************/

const LIWdgClass liwdg_widget_view =
{
	LIWDG_BASE_STATIC, &liwdg_widget_container, "View", sizeof (LIWdgView),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event,
};

LIWdgWidget*
liwdg_view_new (LIWdgManager* manager)
{
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, &liwdg_widget_view);
	if (self == NULL)
		return NULL;
	private_rebuild (LIWDG_VIEW (self));

	return self;
}

LIWdgWidget*
liwdg_view_get_child (LIWdgView* self)
{
	return self->child;
}

void
liwdg_view_set_child (LIWdgView*   self,
                      LIWdgWidget* widget)
{
	if (self->child == widget)
		return;

	/* Free old widget. */
	private_call_detach (self);

	/* Adopt new widget. */
	self->child = widget;
	if (widget != NULL)
		widget->parent = LIWDG_WIDGET (self);

	/* Rebuild request. */
	self->hscrollpos = 0;
	self->vscrollpos = 0;
	private_rebuild (self);
}

int
liwdg_view_get_hscroll (LIWdgView* self)
{
	return self->hscroll;
}

void
liwdg_view_set_hscroll (LIWdgView* self,
                        int        value)
{
	self->hscroll = value;
	private_rebuild (self);
}

int
liwdg_view_get_vscroll (LIWdgView* self)
{
	return self->vscroll;
}

void
liwdg_view_set_vscroll (LIWdgView* self,
                        int        value)
{
	self->vscroll = value;
	private_rebuild (self);
}

/*****************************************************************************/

static int
private_init (LIWdgView*    self,
              LIWdgManager* manager)
{
	liwdg_widget_set_style (LIWDG_WIDGET (self), "view");
	return 1;
}

static void
private_free (LIWdgView* self)
{
	private_call_detach (self);
}

static int
private_event (LIWdgView*  self,
               liwdgEvent* event)
{
	LIWdgRect rect;
	LIWdgManager* manager;

	/* Container interface. */
	if (event->type == LIWDG_EVENT_TYPE_PROBE &&
	    event->probe.clss == &liwdg_widget_container)
	{
		static LIWdgContainerIface iface =
		{
			(LIWdgContainerChildAtFunc) private_child_at,
			(LIWdgContainerChildRequestFunc) private_child_request,
			(LIWdgContainerCycleFocusFunc) private_cycle_focus,
			(LIWdgContainerDetachChildFunc) private_detach_child,
			(LIWdgContainerForeachChildFunc) private_foreach_child,
			(LIWdgContainerTranslateCoordsFunc) private_translate_coords
		};
		event->probe.result = &iface;
		return 0;
	}

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			if (event->button.button == 4)
			{
				private_scroll (self, 0, -32);
				return 0;
			}
			else if (event->button.button == 5)
			{
				private_scroll (self, 0, 32);
				return 0;
			}
			break;
		case LIWDG_EVENT_TYPE_ALLOCATION:
			private_rebuild (self);
			break;
		case LIWDG_EVENT_TYPE_RENDER:
			/* Draw base. */
			manager = LIWDG_WIDGET (self)->manager;
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			/* Draw child. */
			if (self->child != NULL)
			{
				glPushAttrib (GL_SCISSOR_BIT);
				glScissor (rect.x, manager->height - rect.y - rect.height, rect.width, rect.height);
				glEnable (GL_SCISSOR_TEST);
				glPushMatrix ();
				glTranslatef (rect.x - self->hscrollpos, rect.y - self->vscrollpos, 0.0f);
				liwdg_widget_draw (self->child);
				glPopMatrix ();
				glPopAttrib ();
			}
			return 0;
		case LIWDG_EVENT_TYPE_UPDATE:
			if (self->child != NULL)
				liwdg_widget_update (self->child, event->update.secs);
			return 1;
	}

	return liwdg_widget_container.event (LIWDG_WIDGET (self), event);
}

static LIWdgWidget*
private_child_at (LIWdgView* self,
                  int        pixx,
                  int        pixy)
{
	if (liwdg_widget_get_visible (self->child))
		return self->child;

	return NULL;
}

static void
private_child_request (LIWdgView*   self,
                       LIWdgWidget* child)
{
	private_rebuild (self);
}

static LIWdgWidget*
private_cycle_focus (LIWdgView*   self,
                     LIWdgWidget* curr,
                     int          next)
{
	LIWdgWidget* tmp;

	/* Cycle focus. */
	if (liwdg_widget_typeis (self->child, &liwdg_widget_container))
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
private_detach_child (LIWdgView*   self,
                      LIWdgWidget* child)
{
	assert (self->child == child);

	self->child = NULL;
	child->parent = NULL;
	private_rebuild (self);
}

static void
private_foreach_child (LIWdgView* self,
                       void     (*call)(),
                       void*      data)
{
	if (self->child != NULL)
		call (data, self->child);
}

static void
private_call_detach (LIWdgView* self)
{
	int free = 1;
	LIWdgManager* manager;
	LIWdgWidget* child;

	manager = LIWDG_WIDGET (self)->manager;
	child = self->child;
	if (child != NULL)
	{
		lical_callbacks_call (manager->callbacks, manager, "widget-detach", lical_marshal_DATA_PTR_PTR, child, &free);
		if (free)
			liwdg_widget_free (child);
		else
			child->parent = NULL;
		self->child = NULL;
	}
}

static void
private_rebuild (LIWdgView* self)
{
	LIWdgRect rect;
	LIWdgSize size;
	LIWdgSize size1;

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
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self), size1.width, size1.height);

	/* Allocate virtual rectangle for child. */
	if (self->child != NULL)
	{
		liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
		rect.width = LI_MAX (size.width, rect.width);
		rect.height = LI_MAX (size.height, rect.height);
		liwdg_widget_set_allocation (self->child, 0, 0, rect.width, rect.height);
	}

	/* Validate scroll position. */
	private_scroll (self, 0, 0);
}

static void
private_scroll (LIWdgView* self,
                int        x,
                int        y)
{
	LIWdgRect rect;

	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
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

static void
private_translate_coords (LIWdgView* self,
                          int        containerx,
                          int        containery,
                          int*       childx,
                          int*       childy)
{
	LIWdgRect rect;

	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
	*childx = containerx - (rect.x - self->hscrollpos);
	*childy = containery - (rect.y - self->vscrollpos);
}

/** @} */
/** @} */
