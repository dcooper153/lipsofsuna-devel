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
 * \addtogroup liwdgMenu Menu
 * @{
 */

#include "widget-label.h"
#include "widget-menu.h"
#include "widget-private.h"

#define SPACINGX 10
#define SPACINGY 5

static int
private_init (liwdgMenu*    self,
              liwdgManager* manager);

static void
private_free (liwdgMenu* self);

static int
private_event (liwdgMenu*  self,
               liwdgEvent* event);

static liwdgMenuProxy*
private_proxy_new (liwdgMenu*      self,
                   liwdgMenuGroup* group,
                   liwdgMenuItem*  item);

static void
private_proxy_free (liwdgMenu*      self,
                    liwdgMenuProxy* proxy);

static liwdgMenuProxy*
private_proxy_find (liwdgMenu* self,
                    int        x,
                    int        y);

static void
private_rebuild (liwdgMenu* self);

static const char*
private_style (const liwdgMenu* self);

const liwdgClass liwdgMenuType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Menu", sizeof (liwdgMenu),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

/**
 * \brief Creates a new menu widget.
 *
 * \param manager Widget manager.
 * \return New widget or NULL.
 */
liwdgWidget*
liwdg_menu_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgMenuType);
}

/**
 * \brief Inserts a group to the menu.
 *
 * \param self Menu.
 * \param group Menu group.
 * \return Nonzero on success.
 */
int
liwdg_menu_insert_group (liwdgMenu*      self,
                         liwdgMenuGroup* group)
{
	int i;
	liwdgMenuProxy* proxy;

	if (!lialg_list_prepend (&self->groups, group))
		return 0;
	for (i = 0 ; i < group->items.count ; i++)
	{
#warning No menu merging.
		proxy = private_proxy_new (self, group, group->items.array[i]);
		if (proxy == NULL)
			continue;
		if (!lialg_list_prepend (&self->proxies, proxy))
			private_proxy_free (self, proxy);
	}
	private_rebuild (self);

	return 1;
}

/**
 * \brief Removes a group from the menu.
 *
 * \param self Menu.
 * \param group Menu group.
 */
void
liwdg_menu_remove_group (liwdgMenu*      self,
                         liwdgMenuGroup* group)
{
	private_rebuild (self);
}

/**
 * \brief Gets the allocation of a menu item.
 *
 * \param self Menu.
 * \param name Item name.
 * \param value Return location for the rectangle.
 * \return Nonzero on success.
 */
int
liwdg_menu_get_item_rect (const liwdgMenu* self,
                          const char*      name,
                          liwdgRect*       value)
{
	lialgList* ptr;
	liwdgMenuItem* item;
	liwdgMenuProxy* proxy;
	liwdgRect rect;

	if (self->font == NULL)
		return 0;
	liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), private_style (self), &rect);
	if (self->vertical)
	{
		for (ptr = self->proxies ; ptr != NULL ; ptr = ptr->next)
		{
			proxy = ptr->data;
			item = proxy->items->data;
			if (!strcmp (name, item->text))
			{
				*value = rect;
				return 1;
			}
			rect.y += lifnt_font_get_height (self->font) + SPACINGY;
		}
	}
	else
	{
		for (ptr = self->proxies ; ptr->next != NULL ; ptr = ptr->next) {}
		for ( ; ptr != NULL ; ptr = ptr->prev)
		{
			proxy = ptr->data;
			item = proxy->items->data;
			if (!strcmp (name, item->text))
			{
				*value = rect;
				return 1;
			}
			rect.x += lifnt_layout_get_width (proxy->label) + SPACINGX;
		}
	}

	return 0;
}

int
liwdg_menu_get_vertical (const liwdgMenu* self)
{
	return self->vertical;
}

void
liwdg_menu_set_vertical (liwdgMenu* self,
                         int        value)
{
	value = (value != 0);
	if (value != self->vertical)
	{
		self->vertical = value;
		private_rebuild (self);
	}
}

/****************************************************************************/

static int
private_init (liwdgMenu*    self,
              liwdgManager* manager)
{
	self->font = liwdg_manager_find_font (manager, "default");

	return 1;
}

static void
private_free (liwdgMenu* self)
{
	lialgList* ptr;

	for (ptr = self->proxies ; ptr != NULL ; ptr = ptr->next)
		private_proxy_free (self, ptr->data);
	lialg_list_free (self->groups);
	lialg_list_free (self->proxies);
}

static int
private_event (liwdgMenu*  self,
               liwdgEvent* event)
{
	lialgList* ptr;
	liwdgMenuItem* item;
	liwdgMenuProxy* proxy;
	liwdgRect rect;
	liwdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			proxy = private_proxy_find (self, event->button.x, event->button.y);
			if (proxy != NULL)
			{
				for (ptr = proxy->items ; ptr != NULL ; ptr = ptr->next)
				{
					item = ptr->data;
					if (item->group->callback.call != NULL)
						item->group->callback.call (item->group->callback.data, item->id);
				}
			}
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			if (!LIWDG_WIDGET (self)->transparent)
				liwdg_widget_paint (LIWDG_WIDGET (self), private_style (self), NULL);
			if (self->font != NULL)
			{
				style = liwdg_widget_get_style (LIWDG_WIDGET (self), "menu");
				liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), private_style (self), &rect);
				if (self->vertical)
				{
					for (ptr = self->proxies ; ptr != NULL ; ptr = ptr->next)
					{
						proxy = ptr->data;
						glColor4fv (style->color);
						lifnt_layout_render (proxy->label, rect.x, rect.y);
						rect.y += lifnt_font_get_height (self->font) + SPACINGY;
					}
				}
				else
				{
					for (ptr = self->proxies ; ptr->next != NULL ; ptr = ptr->next) {}
					for ( ; ptr != NULL ; ptr = ptr->prev)
					{
						proxy = ptr->data;
						glColor4fv (style->color);
						lifnt_layout_render (proxy->label, rect.x, rect.y);
						rect.x += lifnt_layout_get_width (proxy->label) + SPACINGX;
					}
				}
			}
			break;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static liwdgMenuProxy*
private_proxy_new (liwdgMenu*      self,
                   liwdgMenuGroup* group,
                   liwdgMenuItem*  item)
{
	liwdgMenuProxy* proxy;

	/* Allocate proxy. */
	proxy = calloc (1, sizeof (liwdgMenuProxy));
	if (proxy == NULL)
		return NULL;

	/* Allocate label. */
	proxy->label = lifnt_layout_new ();
	if (proxy->label == NULL)
	{
		free (proxy);
		return NULL;
	}
	if (self->font != NULL)
		lifnt_layout_append_string (proxy->label, self->font, item->text);

	/* Store item reference. */
	if (!lialg_list_prepend (&proxy->items, item))
	{
		lifnt_layout_free (proxy->label);
		free (proxy);
		return NULL;
	}

	return proxy;
}

static void
private_proxy_free (liwdgMenu*      self,
                    liwdgMenuProxy* proxy)
{
	lialg_list_free (proxy->items);
	lialg_list_free (proxy->proxies);
	lifnt_layout_free (proxy->label);
	free (proxy);
}

static liwdgMenuProxy*
private_proxy_find (liwdgMenu* self,
                    int        x,
                    int        y)
{
	int size;
	lialgList* ptr;
	liwdgMenuProxy* proxy;
	liwdgRect rect;

	if (self->font == NULL || self->proxies == NULL)
		return NULL;
	liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), private_style (self), &rect);
	if (self->vertical)
	{
		size = lifnt_font_get_height (self->font);
		for (ptr = self->proxies ; ptr != NULL ; ptr = ptr->next)
		{
			proxy = ptr->data;
			if (rect.y <= y && y < rect.y + size)
				return proxy;
			rect.y += size + SPACINGY;
		}
	}
	else
	{
		for (ptr = self->proxies ; ptr->next != NULL ; ptr = ptr->next) {}
		for ( ; ptr != NULL ; ptr = ptr->prev)
		{
			proxy = ptr->data;
			size = lifnt_layout_get_width (proxy->label);
			if (rect.x <= x && x < rect.x + size)
				return proxy;
			rect.x += size + SPACINGX;
		}
	}

	return NULL;
}

static void
private_rebuild (liwdgMenu* self)
{
	lialgList* ptr;
	liwdgMenuProxy* proxy;
	liwdgSize size;

	size.width = 0;
	size.height = 0;
	if (self->font != NULL)
	{
		if (self->vertical)
		{
			for (ptr = self->proxies ; ptr != NULL ; ptr = ptr->next)
			{
				proxy = ptr->data;
				size.width = LI_MAX (size.width, lifnt_layout_get_width (proxy->label));
				size.height += lifnt_font_get_height (self->font);
				if (ptr->next != NULL)
					size.height += SPACINGY;
			}
		}
		else
		{
			size.height = lifnt_font_get_height (self->font);
			for (ptr = self->proxies ; ptr != NULL ; ptr = ptr->next)
			{
				proxy = ptr->data;
				size.width += lifnt_layout_get_width (proxy->label);
				if (ptr->next != NULL)
					size.width += SPACINGX;
			}
		}
	}
	liwdg_widget_set_style_request (LIWDG_WIDGET (self), size.width, size.height, private_style (self));
}

static const char*
private_style (const liwdgMenu* self)
{
	if (self->vertical)
		return "menu";
	else
		return "menu-top";
}

/** @} */
/** @} */
