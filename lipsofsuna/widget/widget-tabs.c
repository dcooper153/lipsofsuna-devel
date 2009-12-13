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
 * \addtogroup liwdgTabs Tabs
 * @{
 */

#include "widget-tabs.h"

static int
private_init (liwdgTabs*    self,
              liwdgManager* manager);

static void
private_free (liwdgTabs* self);

static int
private_event (liwdgTabs*  self,
               liwdgEvent* event);

static liwdgWidget*
private_child_at (liwdgTabs* self,
                  int        pixx,
                  int        pixy);

static void
private_child_request (liwdgTabs*   self,
                       liwdgWidget* child);

static liwdgWidget*
private_cycle_focus (liwdgTabs*   self,
                     liwdgWidget* curr,
                     int          next);

static void
private_detach_child (liwdgTabs*   self,
                      liwdgWidget* child);

static void
private_foreach_child (liwdgTabs* self,
                       void     (*call)(),
                       void*      data);

static void
private_rebuild (liwdgTabs* self);

/*****************************************************************************/

const liwdgClass liwdgTabsType =
{
	LIWDG_BASE_STATIC, &liwdgContainerType, "Tabs", sizeof (liwdgTabs),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event,
};

liwdgWidget*
liwdg_tabs_new (liwdgManager* manager)
{
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liwdgTabsType);
	if (self == NULL)
		return NULL;
	private_rebuild (LIWDG_TABS (self));

	return self;
}

int
liwdg_tabs_append_tab (liwdgTabs*  self,
                      const char*  title,
                      liwdgWidget* widget)
{
	liwdgTab tmp;

	/* Format tab. */
	memset (&tmp, 0, sizeof (liwdgTab));
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
	widget->parent = LIWDG_WIDGET (self);

	/* Rebuild request. */
	private_rebuild (self);

	return 1;
}

/*****************************************************************************/

static int
private_init (liwdgTabs*    self,
              liwdgManager* manager)
{
	return 1;
}

static void
private_free (liwdgTabs* self)
{
	int i;

	for (i = 0 ; i < self->tabs.count ; i++)
	{
		if (self->tabs.array[i].widget != NULL)
			liwdg_widget_free (self->tabs.array[i].widget);
		if (self->tabs.array[i].layout != NULL)
			lifnt_layout_free (self->tabs.array[i].layout);
		lisys_free (self->tabs.array[i].text);
	}
	lisys_free (self->tabs.array);
}

static int
private_event (liwdgTabs*  self,
               liwdgEvent* event)
{
	int i;
	liwdgRect rect;
	liwdgStyle* style;
	liwdgTab* tab;
	liwdgWidget* child;

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
			(liwdgContainerForeachChildFunc) private_foreach_child,
			(liwdgContainerTranslateCoordsFunc) NULL
		};
		event->probe.result = &iface;
		return 0;
	}

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "tabs", &rect);
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
			style = liwdg_widget_get_style (LIWDG_WIDGET (self), "tabs");
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "tabs", &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), "tabs", NULL);
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
				liwdg_widget_render (child);
			return 0;
		case LIWDG_EVENT_TYPE_UPDATE:
			for (i = 0 ; i < self->tabs.count ; i++)
			{
				if (self->tabs.array[i].widget != NULL)
					liwdg_widget_update (self->tabs.array[i].widget, event->update.secs);
			}
			return 1;
	}

	return liwdgContainerType.event (LIWDG_WIDGET (self), event);
}

static liwdgWidget*
private_child_at (liwdgTabs* self,
                  int        pixx,
                  int        pixy)
{
	liwdgWidget* child;

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
private_child_request (liwdgTabs*   self,
                       liwdgWidget* child)
{
	private_rebuild (self);
}

static liwdgWidget*
private_cycle_focus (liwdgTabs*   self,
                     liwdgWidget* curr,
                     int          next)
{
	liwdgWidget* tmp;
	liwdgWidget* child;

	/* Find active widget. */
	if (curr != NULL)
		return NULL;
	if (self->tabs.count <= self->active)
		return NULL;
	child = self->tabs.array[self->active].widget;
	if (child == NULL)
		return NULL;

	/* Cycle focus. */
	if (liwdg_widget_typeis (child, &liwdgContainerType))
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
private_detach_child (liwdgTabs*   self,
                      liwdgWidget* child)
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
private_foreach_child (liwdgTabs* self,
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
private_rebuild (liwdgTabs* self)
{
	int i;
	liwdgRect rect;
	liwdgSize size;
	liwdgSize size1;
	liwdgStyle* style;
	liwdgTab* tab;
	liwdgWidget* child;

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
		size.width = LI_MAX (size.width, size1.width);
		size.height = LI_MAX (size.height, size1.height);
	}
	liwdg_widget_set_style_request (LIWDG_WIDGET (self), size.width, size.height, "tabs");

	/* Position tabs. */
	style = liwdg_widget_get_style (LIWDG_WIDGET (self), "tabs");
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
	liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "tabs", &rect);
	liwdg_widget_set_allocation (child, rect.x, rect.y, rect.width, rect.height);
}

/** @} */
/** @} */
