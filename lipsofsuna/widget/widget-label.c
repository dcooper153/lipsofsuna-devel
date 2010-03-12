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
 * \addtogroup LIWdgLabel Label
 * @{
 */

#include "widget-label.h"
#include "widget-manager.h"

static int
private_init (LIWdgLabel*   self,
              LIWdgManager* manager);

static void
private_free (LIWdgLabel* self);

static int
private_event (LIWdgLabel* self,
               LIWdgEvent* event);

static void
private_rebuild (LIWdgLabel* self);

/****************************************************************************/

const LIWdgClass*
liwdg_widget_label ()
{
	static const LIWdgClass clss =
	{
		liwdg_widget_widget, "Label", sizeof (LIWdgLabel),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

LIWdgWidget*
liwdg_label_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, liwdg_widget_label ());
}

LIWdgWidget*
liwdg_label_new_with_text (LIWdgManager* manager,
                           const char*   text)
{
	LIWdgWidget* self;

	self = liwdg_label_new (manager);
	if (self == NULL)
		return NULL;
	if (!liwdg_label_set_text (LIWDG_LABEL (self), text))
	{
		liwdg_widget_free (self);
		return NULL;
	}

	return self;
}

LIFntFont*
liwdg_label_get_font (LIWdgLabel* self)
{
	return self->font;
}

void
liwdg_label_set_font (LIWdgLabel* self,
                      LIFntFont*  font)
{
	self->font = font;
	private_rebuild (self);
}

float
liwdg_label_get_halign (const LIWdgLabel* self)
{
	return self->halign;
}

void
liwdg_label_set_halign (LIWdgLabel* self,
                        float       value)
{
	self->halign = LIMAT_CLAMP (value, 0.0f, 1.0f);
}

int
liwdg_label_get_highlight (const LIWdgLabel* self)
{
	return self->highlight;
}

void
liwdg_label_set_highlight (LIWdgLabel* self,
                           int         value)
{
	self->highlight = value;
}

const char*
liwdg_label_get_text (LIWdgLabel* self)
{
	return self->string;
}

int
liwdg_label_set_text (LIWdgLabel* self,
                      const char* text)
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
liwdg_label_get_valign (const LIWdgLabel* self)
{
	return self->valign;
}

void
liwdg_label_set_valign (LIWdgLabel* self,
                        float       value)
{
	self->valign = LIMAT_CLAMP (value, 0.0f, 1.0f);
}

/****************************************************************************/

static int
private_init (LIWdgLabel*   self,
              LIWdgManager* manager)
{
	self->string = lisys_calloc (1, 1);
	if (self->string == NULL)
		return 0;
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		lisys_free (self->string);
		return 0;
	}
	liwdg_widget_set_style (LIWDG_WIDGET (self), "label");
	private_rebuild (self);
	return 1;
}

static void
private_free (LIWdgLabel* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (LIWdgLabel* self,
               LIWdgEvent* event)
{
	int w;
	int h;
	LIWdgRect rect;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			return lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			if (self->highlight)
			{
				glColor4fv (LIWDG_WIDGET (self)->style->selection);
				glBindTexture (GL_TEXTURE_2D, 0);
				glBegin (GL_TRIANGLE_STRIP);
				glVertex2i (rect.x, rect.y);
				glVertex2i (rect.x + rect.width, rect.y);
				glVertex2i (rect.x, rect.y + rect.height);
				glVertex2i (rect.x + rect.width, rect.y + rect.height);
				glEnd ();
			}
			glColor4fv (LIWDG_WIDGET (self)->style->color);
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			lifnt_layout_render (self->text,
				rect.x + (int)(self->halign * (rect.width - w)),
				rect.y + (int)(self->valign * (rect.height - h)));
			return 1;
	}

	return liwdg_widget_widget ()->event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (LIWdgLabel* self)
{
	int h = 0;
	int limit;

	lifnt_layout_clear (self->text);
	limit = LIWDG_WIDGET (self)->userrequest.width;
	if (limit >= 0)
		lifnt_layout_set_width_limit (self->text, limit);
	else
		lifnt_layout_set_width_limit (self->text, 0);
	if (self->font != NULL)
	{
		h = lifnt_font_get_height (self->font);
		lifnt_layout_append_string (self->text, self->font, self->string);
	}
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LIMAT_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
