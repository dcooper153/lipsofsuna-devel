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
 * \addtogroup liwdgScroll Scroll
 * @{
 */

#include "widget-manager.h"
#include "widget-scroll.h"

static int
private_init (liwdgScroll* self,
              liwdgManager*  manager);

static void
private_free (liwdgScroll* self);

static int
private_event (liwdgScroll* self,
               liwdgEvent*    event);

static void
private_rebuild (liwdgScroll* self);

const liwdgClass liwdgScrollType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Scroll", sizeof (liwdgScroll),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liwdg_scroll_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgScrollType);
}

lifntFont*
liwdg_scroll_get_font (liwdgScroll* self)
{
	return self->font;
}

void
liwdg_scroll_set_font (liwdgScroll* self,
                       lifntFont* font)
{
	self->font = font;
	private_rebuild (self);
}

void
liwdg_scroll_set_range (liwdgScroll* self,
                        float        min,
                        float        max)
{
	self->min = min;
	self->max = max;
	self->value = LI_MIN (self->value, self->max);
	self->value = LI_MAX (self->value, self->min);
	private_rebuild (self);
}

float
liwdg_scroll_get_value (liwdgScroll* self)
{
	return self->value;
}

void
liwdg_scroll_set_value (liwdgScroll* self,
                        float      value)
{
	self->value = value;
	private_rebuild (self);
}

/****************************************************************************/

static int
private_init (liwdgScroll*    self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_PRESSED, lical_marshal_DATA_PTR))
		return 0;
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	self->min = 0.0f;
	self->max = 1.0f;
	if (self->text == NULL)
		return 0;
	liwdg_widget_set_style (LIWDG_WIDGET (self), "scroll-horz");
	private_rebuild (self);
	return 1;
}

static void
private_free (liwdgScroll* self)
{
	lifnt_layout_free (self->text);
}

static int
private_event (liwdgScroll*  self,
               liwdgEvent* event)
{
	int w;
	int h;
	float v;
	liwdgRect rect;
	liwdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			self->value = self->min + (self->max - self->min) * (event->button.x - rect.x) / rect.width;
			private_rebuild (self);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED, self);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			v = self->max - self->min;
			if (v >= LI_MATH_EPSILON)
				v = (self->value - self->min) / v;
			else
				v = 0.0f;
			style = liwdg_widget_get_style (LIWDG_WIDGET (self));
			/* Draw base. */
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			/* Draw bar. */
			glBindTexture (GL_TEXTURE_2D, 0);
			glColor4fv (style->selection);
			glBegin (GL_QUADS);
			glVertex2f (rect.x, rect.y);
			glVertex2f (rect.x + v * rect.width, rect.y);
			glVertex2f (rect.x + v * rect.width, rect.y + rect.height);
			glVertex2f (rect.x, rect.y + rect.height);
			glEnd ();
			/* Draw label. */
			glColor4fv (style->color);
			lifnt_layout_render (self->text,
				rect.x + (rect.width - w) / 2,
				rect.y + (rect.height - h) / 2);
			return 1;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (liwdgScroll* self)
{
	int h = 0;
	char buf[256];

	lifnt_layout_clear (self->text);
	if (self->font != NULL)
	{
		h = lifnt_font_get_height (self->font);
		snprintf (buf, 256, "%.2f", self->value);
		lifnt_layout_append_string (self->text, self->font, buf);
	}
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LI_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
