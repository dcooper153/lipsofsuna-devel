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
 * \addtogroup LIWdgProgress Progress
 * @{
 */

#include "widget-manager.h"
#include "widget-progress.h"

static int
private_init (LIWdgProgress* self,
              LIWdgManager*  manager);

static void
private_free (LIWdgProgress* self);

static int
private_event (LIWdgProgress* self,
               LIWdgEvent*    event);

static void
private_rebuild (LIWdgProgress* self);

/****************************************************************************/

const LIWdgClass*
liwdg_widget_progress ()
{
	static const LIWdgClass clss =
	{
		liwdg_widget_widget, "Progress", sizeof (LIWdgProgress),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

LIWdgWidget*
liwdg_progress_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, liwdg_widget_progress ());
}

const char*
liwdg_progress_get_text (LIWdgProgress* self)
{
	return self->string;
}

int
liwdg_progress_set_text (LIWdgProgress* self,
                         const char*    text)
{
	char* tmp;

	tmp = listr_dup (text);
	if (tmp == NULL)
		return 0;
	lisys_free (self->string);
	self->string = tmp;
	private_rebuild (self);

	return 1;
}

float
liwdg_progress_get_value (LIWdgProgress* self)
{
	return self->value;
}

void
liwdg_progress_set_value (LIWdgProgress* self,
                          float          value)
{
	self->value = value;
}

/****************************************************************************/

static int
private_init (LIWdgProgress* self,
              LIWdgManager*  manager)
{
	self->string = lisys_calloc (1, 1);
	if (self->string == NULL)
		return 0;
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		lisys_free (self->string);
		return 0;
	}
	liwdg_widget_set_style (LIWDG_WIDGET (self), "progress");
	private_rebuild (self);

	return 1;
}

static void
private_free (LIWdgProgress* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (LIWdgProgress* self,
               LIWdgEvent*    event)
{
	int w;
	int h;
	LIWdgRect alloc;
	LIWdgRect rect;
	LIWdgStyle* style0;
	LIWdgStyle* style1;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_RENDER:
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			style0 = liwdg_widget_get_style (LIWDG_WIDGET (self));
			style1 = liwdg_manager_find_style (LIWDG_WIDGET (self)->manager, LIWDG_WIDGET (self)->style_name, "max");
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &alloc);
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			/* Draw base. */
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			/* Draw progress. */
			if (style1 != NULL && style0 != style1)
			{
				glPushAttrib (GL_SCISSOR_BIT);
				glEnable (GL_SCISSOR_TEST);
				glScissor (alloc.x, alloc.y, (int)(self->value * alloc.width), alloc.height);
				liwdg_style_paint (style1, &rect);
				glPopAttrib ();
			}
			else
			{
				glBindTexture (GL_TEXTURE_2D, 0);
				glColor4fv (style0->selection);
				glBegin (GL_QUADS);
				glVertex2f (rect.x, rect.y);
				glVertex2f (rect.x + self->value * rect.width, rect.y);
				glVertex2f (rect.x + self->value * rect.width, rect.y + rect.height);
				glVertex2f (rect.x, rect.y + rect.height);
				glEnd ();
			}
			/* Draw label. */
			glColor4fv (style0->color);
			lifnt_layout_render (self->text,
				rect.x + (rect.width - w) / 2,
				rect.y + (rect.height - h) / 2);
			return 1;
		case LIWDG_EVENT_TYPE_STYLE:
			private_rebuild (self);
			break;
	}

	return liwdg_widget_widget ()->event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (LIWdgProgress* self)
{
	int h = 0;
	LIFntFont* font;

	lifnt_layout_clear (self->text);
	font = liwdg_widget_get_font (LIWDG_WIDGET (self));
	if (font != NULL)
	{
		h = lifnt_font_get_height (font);
		lifnt_layout_append_string (self->text, font, self->string);
	}
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LIMAT_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
