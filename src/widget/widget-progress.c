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
 * \addtogroup liwdgProgress Progress
 * @{
 */

#include "widget-manager.h"
#include "widget-progress.h"

static int
private_init (liwdgProgress* self,
              liwdgManager*  manager);

static void
private_free (liwdgProgress* self);

static int
private_event (liwdgProgress* self,
               liwdgEvent*    event);

static void
private_rebuild (liwdgProgress* self);

const liwdgClass liwdgProgressType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Progress", sizeof (liwdgProgress),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liwdg_progress_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgProgressType);
}

lifntFont*
liwdg_progress_get_font (liwdgProgress* self)
{
	return self->font;
}

void
liwdg_progress_set_font (liwdgProgress* self,
                         lifntFont*     font)
{
	self->font = font;
	private_rebuild (self);
}

const char*
liwdg_progress_get_text (liwdgProgress* self)
{
	return self->string;
}

int
liwdg_progress_set_text (liwdgProgress* self,
                         const char*    text)
{
	char* tmp;

	tmp = strdup (text);
	if (tmp == NULL)
		return 0;
	free (self->string);
	self->string = tmp;
	private_rebuild (self);
	return 1;
}

float
liwdg_progress_get_value (liwdgProgress* self)
{
	return self->value;
}

void
liwdg_progress_set_value (liwdgProgress* self,
                          float          value)
{
	self->value = value;
}

/****************************************************************************/

static int
private_init (liwdgProgress*   self,
              liwdgManager* manager)
{
	self->string = calloc (1, 1);
	if (self->string == NULL)
		return 0;
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		free (self->string);
		return 0;
	}
	private_rebuild (self);
	return 1;
}

static void
private_free (liwdgProgress* self)
{
	lifnt_layout_free (self->text);
	free (self->string);
}

static int
private_event (liwdgProgress* self,
               liwdgEvent*    event)
{
	int w;
	int h;
	liwdgRect rect;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_RENDER:
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			/* Draw base. */
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "progress", &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), "progress", NULL);
			/* Draw progress. */
			glBindTexture (GL_TEXTURE_2D, 0);
			glColor3f (1.0f, 0.0f, 0.0f);
			glBegin (GL_QUADS);
			glVertex2f (rect.x, rect.y);
			glVertex2f (rect.x + self->value * rect.width, rect.y);
			glVertex2f (rect.x + self->value * rect.width, rect.y + rect.height);
			glVertex2f (rect.x, rect.y + rect.height);
			glEnd ();
			/* Draw label. */
			glColor3f (0.0f, 0.0f, 0.0f);
			lifnt_layout_render (self->text,
				rect.x + (rect.width - w) / 2,
				rect.y + (rect.height - h) / 2);
			return 1;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (liwdgProgress* self)
{
	int h = 0;

	lifnt_layout_clear (self->text);
	if (self->font != NULL)
	{
		h = lifnt_font_get_height (self->font);
		lifnt_layout_append_string (self->text, self->font, self->string);
	}
	liwdg_widget_set_style_request (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LI_MAX (
		lifnt_layout_get_height (self->text), h), "progress");
}

/** @} */
/** @} */
