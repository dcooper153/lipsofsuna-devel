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

const liwdgClass liwdgWindowType =
{
	LIWDG_BASE_STATIC, &liwdgGroupType, "Window", sizeof (liwdgWindow),
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

	self = liwdg_widget_new (manager, &liwdgWindowType);
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
	return self->title;
}

int
liwdg_window_set_title (liwdgWindow* self,
                        const char*  title)
{
	char* tmp;

	/* Set string. */
	tmp = listr_dup (title);
	if (tmp == NULL)
		return 0;
	lisys_free (self->title);
	self->title = tmp;

	/* Rebuild label. */
	lifnt_layout_clear (self->text);
	if (self->font != NULL)
		lifnt_layout_append_string (self->text, self->font, title);

	return 1;
}

/*****************************************************************************/

static int
private_init (liwdgWindow*  self,
              liwdgManager* manager)
{
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
		return 0;
	liwdg_widget_set_style (LIWDG_WIDGET (self), "window");
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

	if (event->type == LIWDG_EVENT_TYPE_RENDER)
	{
		/* Render frame. */
		if (!LIWDG_WIDGET (self)->transparent)
		{
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			glColor4fv (LIWDG_WIDGET (self)->style->color);
			lifnt_layout_render (self->text, rect.x + 4, rect.y + 4); /* FIXME: Hardcoded. */
		}
	}

	/* Call base class. */
	return liwdgGroupType.event (LIWDG_WIDGET (self), event);
}

/** @} */
/** @} */
