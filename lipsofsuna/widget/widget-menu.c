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
 * \addtogroup LIWdgMenu Menu
 * @{
 */

#include "widget-label.h"
#include "widget-menu.h"
#include "widget-private.h"

#define SPACINGX 10
#define SPACINGY 5

static int
private_init (LIWdgMenu*    self,
              LIWdgManager* manager);

static void
private_free (LIWdgMenu* self);

static int
private_event (LIWdgMenu*  self,
               liwdgEvent* event);

static void
private_render_horizontal (LIWdgMenu* self);

static void
private_render_vertical (LIWdgMenu* self);

static LIWdgMenuItem*
private_item_create (LIWdgMenu*     self,
                     LIWdgMenuItem* parent,
                     const char*    text);

static void
private_item_free (LIWdgMenu*     self,
                   LIWdgMenuItem* item);

static LIWdgMenuItem*
private_item_find (LIWdgMenu* self,
                   int        x,
                   int        y);

static void
private_rebuild (LIWdgMenu* self);

const LIWdgClass liwdg_widget_menu =
{
	LIWDG_BASE_STATIC, &liwdg_widget_widget, "Menu", sizeof (LIWdgMenu),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event
};

/****************************************************************************/

/**
 * \brief Creates a new menu widget.
 *
 * \param manager Widget manager.
 * \return New widget or NULL.
 */
LIWdgWidget*
liwdg_menu_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdg_widget_menu);
}

/**
 * \brief Inserts a menu item to the menu.
 *
 * \param self Menu.
 * \param label Label.
 * \param icon Icon name or NULL.
 * \param call Callback.
 * \param data Userdata.
 * \return Nonzero on success.
 */
int
liwdg_menu_insert_item (LIWdgMenu*   self,
                        const char*  label,
                        const char*  icon,
                        liwdgHandler call,
                        void*        data)
{
	LIWdgMenuItem* item;

#warning Menu icons not implemented
	item = private_item_create (self, NULL, label);
	if (item == NULL)
		return 0;
	item->callback.call = call;
	item->callback.data = data;
	private_rebuild (self);

	return 1;
}

int
liwdg_menu_get_autohide (const LIWdgMenu* self)
{
	return self->autohide;
}

void
liwdg_menu_set_autohide (LIWdgMenu* self,
                         int        value)
{
	self->autohide = value;
}

LIWdgMenuItem*
liwdg_menu_get_item (const LIWdgMenu* self,
                     int              index)
{
	return self->items.array[index];
}

int
liwdg_menu_get_item_count (const LIWdgMenu* self)
{
	return self->items.count;
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
liwdg_menu_get_item_rect (const LIWdgMenu* self,
                          const char*      name,
                          LIWdgRect*       value)
{
	int i;
	LIWdgMenuItem* item;
	LIWdgRect rect;

	if (self->font == NULL)
		return 0;
	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
	if (self->vertical)
	{
		for (i = 0 ; i < self->items.count ; i++)
		{
			item = self->items.array[i];
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
		for (i = self->items.count - 1 ; i >= 0 ; i--)
		{
			item = self->items.array[i];
			if (!strcmp (name, item->text))
			{
				*value = rect;
				return 1;
			}
			rect.x += lifnt_layout_get_width (item->label) + SPACINGX;
		}
	}

	return 0;
}

int
liwdg_menu_get_vertical (const LIWdgMenu* self)
{
	return self->vertical;
}

void
liwdg_menu_set_vertical (LIWdgMenu* self,
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
private_init (LIWdgMenu*    self,
              LIWdgManager* manager)
{
	self->font = liwdg_manager_find_font (manager, "default");

	return 1;
}

static void
private_free (LIWdgMenu* self)
{
	int i;

	for (i = 0 ; i < self->items.count ; i++)
		private_item_free (self, self->items.array[i]);
	lisys_free (self->items.array);
}

static int
private_event (LIWdgMenu*  self,
               liwdgEvent* event)
{
	LIWdgMenuItem* item;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			item = private_item_find (self, event->button.x, event->button.y);
			if (item != NULL)
			{
				if (item->callback.call != NULL)
					item->callback.call (item->callback.data, item);
				if (self->autohide)
					liwdg_widget_set_visible (LIWDG_WIDGET (self), 0);
			}
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			if (!LIWDG_WIDGET (self)->transparent)
				liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			if (self->font != NULL)
			{
				if (self->vertical)
					private_render_vertical (self);
				else
					private_render_horizontal (self);
			}
			break;
	}

	return liwdg_widget_widget.event (LIWDG_WIDGET (self), event);
}

static void
private_render_horizontal (LIWdgMenu*  self)
{
	int i;
	LIWdgManager* manager;
	LIWdgMenuItem* item;
	LIWdgRect rect;
	LIWdgStyle* style;

	/* Get style allocation. */
	manager = LIWDG_WIDGET (self)->manager;
	style = liwdg_widget_get_style (LIWDG_WIDGET (self));
	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);

	/* Render each item. */
	for (i = self->items.count - 1 ; i >= 0 ; i--)
	{
		item = self->items.array[i];
		glColor4fv (style->color);
		lifnt_layout_render (item->label, rect.x, rect.y);
		rect.x += lifnt_layout_get_width (item->label) + SPACINGX;
	}
}

static void
private_render_vertical (LIWdgMenu*  self)
{
	int i;
	int lineh;
	int pointer[2];
	LIWdgManager* manager;
	LIWdgMenuItem* item;
	LIWdgRect rect;
	LIWdgStyle* style;

	/* Get style allocation. */
	style = liwdg_widget_get_style (LIWDG_WIDGET (self));
	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
	lineh = lifnt_font_get_height (self->font) + SPACINGY;

	/* Get relative pointer position. */
	manager = LIWDG_WIDGET (self)->manager;
	liwdg_widget_translate_coords (LIWDG_WIDGET (self),
		manager->pointer.x, manager->pointer.y, pointer + 0, pointer + 1);

	/* Render each item. */
	for (i = 0 ; i < self->items.count ; i++)
	{
		item = self->items.array[i];

		/* Render hover. */
		if (pointer[0] >= rect.x && pointer[0] < rect.x + rect.width &&
		    pointer[1] >= rect.y && pointer[1] < rect.y + lineh)
		{
			glColor4fv (style->hover);
			glBindTexture (GL_TEXTURE_2D, 0);
			glBegin (GL_QUADS);
			glVertex2i (rect.x, rect.y);
			glVertex2i (rect.x + rect.width, rect.y);
			glVertex2i (rect.x + rect.width, rect.y + lineh);
			glVertex2i (rect.x, rect.y + lineh);
			glEnd ();
		}

		/* Render text. */
		glColor4fv (style->color);
		lifnt_layout_render (item->label, rect.x, rect.y + SPACINGY / 2);
		rect.y += lineh;
	}
}

static LIWdgMenuItem*
private_item_create (LIWdgMenu*     self,
                     LIWdgMenuItem* parent,
                     const char*    text)
{
	LIWdgMenuItem* item;

	/* Allocate item. */
	item = lisys_calloc (1, sizeof (LIWdgMenuItem));
	if (item == NULL)
		return NULL;
	item->id = -1;
	item->text = listr_dup (text);
	if (item->text == NULL)
	{
		lisys_free (item);
		return NULL;
	}

	/* Allocate label. */
	item->label = lifnt_layout_new ();
	if (item->label == NULL)
	{
		lisys_free (item->text);
		lisys_free (item);
		return NULL;
	}
	if (self->font != NULL)
		lifnt_layout_append_string (item->label, self->font, item->text);

	/* Append to item list. */
	if (parent != NULL)
	{
		if (!lialg_array_append (&parent->items, &item))
		{
			lisys_free (item->text);
			lisys_free (item);
			return NULL;
		}
	}
	else
	{
		if (!lialg_array_append (&self->items, &item))
		{
			lisys_free (item->text);
			lisys_free (item);
			return NULL;
		}
	}

	return item;
}

static void
private_item_free (LIWdgMenu*     self,
                   LIWdgMenuItem* item)
{
	int i;

	lifnt_layout_free (item->label);
	for (i = 0 ; i < item->items.count ; i++)
		private_item_free (self, item->items.array[i]);
	lisys_free (item->items.array);
	lisys_free (item->icon);
	lisys_free (item->text);
	lisys_free (item);
}

static LIWdgMenuItem*
private_item_find (LIWdgMenu* self,
                   int        x,
                   int        y)
{
	int i;
	int size;
	LIWdgMenuItem* item;
	LIWdgRect rect;

	if (self->font == NULL)
		return NULL;
	liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
	if (self->vertical)
	{
		size = lifnt_font_get_height (self->font);
		for (i = 0 ; i < self->items.count ; i++)
		{
			item = self->items.array[i];
			if (rect.y <= y && y < rect.y + size)
				return item;
			rect.y += size + SPACINGY;
		}
	}
	else
	{
		for (i = self->items.count - 1 ; i >= 0 ; i--)
		{
			item = self->items.array[i];
			size = lifnt_layout_get_width (item->label);
			if (rect.x <= x && x < rect.x + size)
				return item;
			rect.x += size + SPACINGX;
		}
	}

	return NULL;
}

static void
private_rebuild (LIWdgMenu* self)
{
	int i;
	LIWdgMenuItem* item;
	LIWdgSize size;

	size.width = 0;
	size.height = 0;
	if (self->font != NULL)
	{
		if (self->vertical)
		{
			for (i = 0 ; i < self->items.count ; i++)
			{
				item = self->items.array[i];
				size.width = LI_MAX (size.width, lifnt_layout_get_width (item->label));
				size.height += lifnt_font_get_height (self->font);
				size.height += SPACINGY;
			}
		}
		else
		{
			size.height = lifnt_font_get_height (self->font);
			for (i = 0 ; i < self->items.count ; i++)
			{
				item = self->items.array[i];
				size.width += lifnt_layout_get_width (item->label);
				size.width += SPACINGX;
			}
		}
	}
	liwdg_widget_set_style (LIWDG_WIDGET (self), self->vertical? "menu" : "menu-top");
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self), size.width, size.height);
}

/** @} */
/** @} */
