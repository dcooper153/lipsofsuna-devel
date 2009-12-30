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
 * \addtogroup liwdgLabel Label
 * @{
 */

#include "widget-label.h"
#include "widget-manager.h"

static int
private_init (liwdgLabel*   self,
              liwdgManager* manager);

static void
private_free (liwdgLabel* self);

static int
private_event (liwdgLabel* self,
               liwdgEvent* event);

static void
private_rebuild (liwdgLabel* self);

const liwdgClass liwdgLabelType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Label", sizeof (liwdgLabel),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liwdg_label_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgLabelType);
}

liwdgWidget*
liwdg_label_new_with_text (liwdgManager* manager,
                           const char*   text)
{
	liwdgWidget* self;

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

lifntFont*
liwdg_label_get_font (liwdgLabel* self)
{
	return self->font;
}

void
liwdg_label_set_font (liwdgLabel* self,
                      lifntFont*  font)
{
	self->font = font;
	private_rebuild (self);
}

float
liwdg_label_get_halign (const liwdgLabel* self)
{
	return self->halign;
}

void
liwdg_label_set_halign (liwdgLabel* self,
                        float       value)
{
	self->halign = LI_CLAMP (value, 0.0f, 1.0f);
}

int
liwdg_label_get_highlight (const liwdgLabel* self)
{
	return self->highlight;
}

void
liwdg_label_set_highlight (liwdgLabel* self,
                           int         value)
{
	self->highlight = value;
}

const char*
liwdg_label_get_text (liwdgLabel* self)
{
	return self->string;
}

int
liwdg_label_set_text (liwdgLabel* self,
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
liwdg_label_get_valign (const liwdgLabel* self)
{
	return self->valign;
}

void
liwdg_label_set_valign (liwdgLabel* self,
                        float       value)
{
	self->valign = LI_CLAMP (value, 0.0f, 1.0f);
}

/****************************************************************************/

static int
private_init (liwdgLabel*   self,
              liwdgManager* manager)
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
private_free (liwdgLabel* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (liwdgLabel* self,
               liwdgEvent* event)
{
	int w;
	int h;
	liwdgRect rect;

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

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (liwdgLabel* self)
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
		lifnt_layout_get_width (self->text), LI_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
