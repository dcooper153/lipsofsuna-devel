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
 * \addtogroup LIWdgTabs Tabs
 * @{
 */

#include "widget-tabs.h"

static int
private_init (LIWdgTabs*    self,
              LIWdgManager* manager);

static void
private_free (LIWdgTabs* self);

static int
private_event (LIWdgTabs*  self,
               LIWdgEvent* event);

static LIWdgWidget*
private_child_at (LIWdgTabs* self,
                  int        pixx,
                  int        pixy);

static void
private_child_request (LIWdgTabs*   self,
                       LIWdgWidget* child);

static LIWdgWidget*
private_cycle_focus (LIWdgTabs*   self,
                     LIWdgWidget* curr,
                     int          next);

static void
private_detach_child (LIWdgTabs*   self,
                      LIWdgWidget* child);

static void
private_foreach_child (LIWdgTabs* self,
                       void     (*call)(),
                       void*      data);

static void
private_call_detach (LIWdgTabs* self,
                     int        tab);

static void
private_rebuild (LIWdgTabs* self);

/*****************************************************************************/

const LIWdgClass liwdg_widget_tabs =
{
	LIWDG_BASE_STATIC, &liwdg_widget_container, "Tabs", sizeof (LIWdgTabs),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event,
};

LIWdgWidget*
liwdg_tabs_new (LIWdgManager* manager)
{
	LIWdgWidget* self;

	self = liwdg_widget_new (manager, &liwdg_widget_tabs);
	if (self == NULL)
		return NULL;
	private_rebuild (LIWDG_TABS (self));

	return self;
}

int
liwdg_tabs_append_tab (LIWdgTabs*  self,
                      const char*  title,
                      LIWdgWidget* widget)
{
	LIWdgTab tmp;
	LIWdgManager* manager;

	/* Format tab. */
	memset (&tmp, 0, sizeof (LIWdgTab));
	tmp.text = listr_dup (title);
	if (tmp.text == NULL)
		return 0;
	tmp.font = liwdg_manager_find_font (LIWDG_WIDGET (self)->manager, "default");
	if (tmp.font != NULL)
	{
		tmp.layout = lifnt_layout_new ();
		if (tmp.layout == NULL)
		{
			lisys_free (tmp.text);
			return 0;
		}
		lifnt_layout_append_string (tmp.layout, tmp.font, tmp.text);
	}
	tmp.widget = widget;

	/* Append tab. */
	if (!lialg_array_append (&self->tabs, &tmp))
	{
		if (tmp.layout != NULL)
			lifnt_layout_free (tmp.layout);
		lisys_free (tmp.text);
		return 0;
	}
	manager = LIWDG_WIDGET (self)->manager;
	widget->parent = LIWDG_WIDGET (self);
	lical_callbacks_call (manager->callbacks, manager, "widget-attach", lical_marshal_DATA_PTR_PTR, widget, self);

	/* Rebuild request. */
	private_rebuild (self);

	return 1;
}

/*****************************************************************************/

static int
private_init (LIWdgTabs*    self,
              LIWdgManager* manager)
{
	liwdg_widget_set_style (LIWDG_WIDGET (self), "tabs");
	return 1;
}

static void
private_free (LIWdgTabs* self)
{
	int i;

	for (i = 0 ; i < self->tabs.count ; i++)
	{
		/* Free widget. */
		private_call_detach (self, i);

		/* Free label. */
		if (self->tabs.array[i].layout != NULL)
			lifnt_layout_free (self->tabs.array[i].layout);
		lisys_free (self->tabs.array[i].text);
	}
	lisys_free (self->tabs.array);
}

static int
private_event (LIWdgTabs*  self,
               LIWdgEvent* event)
{
	int i;
	LIWdgRect rect;
	LIWdgStyle* style;
	LIWdgTab* tab;
	LIWdgWidget* child;

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
			(LIWdgContainerTranslateCoordsFunc) NULL
		};
		event->probe.result = &iface;
		return 0;
	}

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			if (event->button.y >= rect.y + rect.height)
			{
				for (i = 0 ; i < self->tabs.count ; i++)
				{
					tab = self->tabs.array + i;
					if (tab->rect.x <= event->button.x && event->button.x < tab->rect.x + tab->rect.width)
					{
						self->active = i;
						private_rebuild (self);
						break;
					}
				}
				return 0;
			}
			break;
		case LIWDG_EVENT_TYPE_ALLOCATION:
			private_rebuild (self);
			break;
		case LIWDG_EVENT_TYPE_RENDER:
			/* Draw base. */
			style = liwdg_widget_get_style (LIWDG_WIDGET (self));
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			/* Draw tabs. */
			for (i = 0 ; i < self->tabs.count ; i++)
			{
				tab = self->tabs.array + i;
				if (tab->layout != NULL)
				{
					if (i == self->active)
						glColor4fv (style->selection);
					else
						glColor4fv (style->color);
					lifnt_layout_render (tab->layout, tab->rect.x, tab->rect.y);
				}
			}
			/* Draw active child. */
			if (self->tabs.count <= self->active)
				return 0;
			child = self->tabs.array[self->active].widget;
			if (child != NULL)
				liwdg_widget_draw (child);
			return 0;
		case LIWDG_EVENT_TYPE_UPDATE:
			for (i = 0 ; i < self->tabs.count ; i++)
			{
				if (self->tabs.array[i].widget != NULL)
					liwdg_widget_update (self->tabs.array[i].widget, event->update.secs);
			}
			return 1;
	}

	return liwdg_widget_container.event (LIWDG_WIDGET (self), event);
}

static LIWdgWidget*
private_child_at (LIWdgTabs* self,
                  int        pixx,
                  int        pixy)
{
	LIWdgWidget* child;

	if (self->tabs.count <= self->active)
		return NULL;
	child = self->tabs.array[self->active].widget;
	if (child == NULL)
		return NULL;
	if (liwdg_widget_get_visible (child))
		return child;

	return NULL;
}

static void
private_child_request (LIWdgTabs*   self,
                       LIWdgWidget* child)
{
	private_rebuild (self);
}

static LIWdgWidget*
private_cycle_focus (LIWdgTabs*   self,
                     LIWdgWidget* curr,
                     int          next)
{
	LIWdgWidget* tmp;
	LIWdgWidget* child;

	/* Find active widget. */
	if (curr != NULL)
		return NULL;
	if (self->tabs.count <= self->active)
		return NULL;
	child = self->tabs.array[self->active].widget;
	if (child == NULL)
		return NULL;

	/* Cycle focus. */
	if (liwdg_widget_typeis (child, &liwdg_widget_container))
	{
		tmp = liwdg_container_cycle_focus (LIWDG_CONTAINER (child), NULL, next);
		if (tmp != NULL)
			return tmp;
	}
	else
	{
		if (liwdg_widget_get_focusable (child))
			return child;
	}

	return NULL;
}

static void
private_detach_child (LIWdgTabs*   self,
                      LIWdgWidget* child)
{
	int i;

	for (i = 0 ; i < self->tabs.count ; i++)
	{
		if (self->tabs.array[i].widget != child)
		{
			self->tabs.array[i].widget = NULL;
			break;
		}
	}
	private_rebuild (self);
}

static void
private_foreach_child (LIWdgTabs* self,
                       void     (*call)(),
                       void*      data)
{
	int i;

	for (i = 0 ; i < self->tabs.count ; i++)
	{
		if (self->tabs.array[i].widget != NULL)
			call (data, self->tabs.array[i].widget);
	}
}

static void
private_call_detach (LIWdgTabs* self,
                     int        tab)
{
	int free = 1;
	LIWdgManager* manager;
	LIWdgWidget* child;

	manager = LIWDG_WIDGET (self)->manager;
	child = self->tabs.array[tab].widget;
	if (child != NULL)
	{
		lical_callbacks_call (manager->callbacks, manager, "widget-detach", lical_marshal_DATA_PTR_PTR, child, &free);
		if (free)
			liwdg_widget_free (child);
		else
			child->parent = NULL;
		self->tabs.array[tab].widget = NULL;
	}
}

static void
private_rebuild (LIWdgTabs* self)
{
	int i;
	LIWdgRect rect;
	LIWdgSize size;
	LIWdgSize size1;
	LIWdgStyle* style;
	LIWdgTab* tab;
	LIWdgWidget* child;

	/* Calculate size request. */
	size.width = 0;
	size.height = 0;
	for (i = 0 ; i < self->tabs.count ; i++)
	{
		child = self->tabs.array[i].widget;
		if (child == NULL)
			continue;
		if (!liwdg_widget_get_visible (child))
			continue;
		liwdg_widget_get_request (child, &size1);
		size.width = LIMAT_MAX (size.width, size1.width);
		size.height = LIMAT_MAX (size.height, size1.height);
	}
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self), size.width, size.height);

	/* Position tabs. */
	style = liwdg_widget_get_style (LIWDG_WIDGET (self));
	liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
	rect.x += style->w[0];
	rect.y += rect.height - style->h[2];
	for (i = 0 ; i < self->tabs.count ; i++)
	{
		tab = self->tabs.array + i;
		tab->rect.x = rect.x;
		tab->rect.y = rect.y;
		if (tab->layout != NULL)
		{
			tab->rect.width = lifnt_layout_get_width (tab->layout);
			tab->rect.height = lifnt_font_get_height (tab->font);
		}
		else
		{
			tab->rect.width = 0;
			tab->rect.height = 0;
		}
		rect.x += tab->rect.width + 5;
	}

	/* Find active widget. */
	if (self->tabs.count <= self->active)
		return;
	child = self->tabs.array[self->active].widget;
	if (child == NULL)
		return;

	/* Allocate active widget. */
	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
	liwdg_widget_set_allocation (child, rect.x, rect.y, rect.width, rect.height);
}

/** @} */
/** @} */
