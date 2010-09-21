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
 * \addtogroup LIWdgScroll Scroll
 * @{
 */

#include "widget-manager.h"
#include "widget-scroll.h"

static int
private_init (LIWdgScroll* self,
              LIWdgManager*  manager);

static void
private_free (LIWdgScroll* self);

static int
private_event (LIWdgScroll* self,
               LIWdgEvent*    event);

static void
private_rebuild (LIWdgScroll* self);

/****************************************************************************/

const LIWdgClass*
liwdg_widget_scroll ()
{
	static const LIWdgClass clss =
	{
		liwdg_widget_widget, "Scroll", sizeof (LIWdgScroll),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

LIWdgWidget*
liwdg_scroll_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, liwdg_widget_scroll ());
}

void
liwdg_scroll_set_range (LIWdgScroll* self,
                        float        min,
                        float        max)
{
	self->min = min;
	self->max = max;
	self->value = LIMAT_MIN (self->value, self->max);
	self->value = LIMAT_MAX (self->value, self->min);
	private_rebuild (self);
}

/**
 * \brief Gets the value of the scroll widget.
 *
 * \param self Scroll widget.
 * \return Float.
 */
float
liwdg_scroll_get_value (LIWdgScroll* self)
{
	return self->value;
}

void
liwdg_scroll_set_value (LIWdgScroll* self,
                        float      value)
{
	self->value = value;
	private_rebuild (self);
}

/**
 * \brief Gets the reference value of the scroll widget.
 *
 * \param self Scroll widget.
 * \return Float.
 */
float
liwdg_scroll_get_reference (LIWdgScroll* self)
{
	return self->reference;
}

void
liwdg_scroll_set_reference (LIWdgScroll* self,
                            float      value)
{
	self->reference = value;
}

/****************************************************************************/

static int
private_init (LIWdgScroll*    self,
              LIWdgManager* manager)
{
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
private_free (LIWdgScroll* self)
{
	lifnt_layout_free (self->text);
}

static int
private_event (LIWdgScroll*  self,
               LIWdgEvent* event)
{
	int w;
	int h;
	int bar;
	int ref;
	float v;
	float r;
	LIWdgManager* manager;
	LIWdgRect alloc;
	LIWdgRect clip;
	LIWdgRect rect;
	LIWdgStyle* style0;
	LIWdgStyle* style1;
	LIWdgStyle* style2;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			self->value = self->min + (self->max - self->min) * (event->button.x - rect.x) / rect.width;
			private_rebuild (self);
			lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			v = self->max - self->min;
			if (v >= LIMAT_EPSILON)
			{
				r = (self->reference - self->min) / v;
				v = (self->value - self->min) / v;
				r = LIMAT_MAX (r, v);
			}
			else
			{
				r = 0.0f;
				v = 0.0f;
			}
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			manager = LIWDG_WIDGET (self)->manager;
			style0 = liwdg_widget_get_style (LIWDG_WIDGET (self));
			style1 = liwdg_manager_find_style (manager, LIWDG_WIDGET (self)->style_name, "ref");
			style2 = liwdg_manager_find_style (manager, LIWDG_WIDGET (self)->style_name, "max");
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &alloc);
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			/* Draw base. */
			bar = (int)(v * alloc.width);
			ref = (int)(r * alloc.width);
			clip = alloc;
			clip.width = bar;
			liwdg_style_paint_base (style2, &alloc, &clip);
			clip.x += clip.width;
			clip.width = ref - bar;
			liwdg_style_paint_base (style1, &alloc, &clip);
			clip.x += clip.width;
			clip.width = alloc.width - ref;
			liwdg_style_paint_base (style0, &alloc, &clip);
			/* Draw label. */
			liwdg_style_paint_text (style0, self->text, 0.5f, 0.5f, &rect);
			return 1;
		case LIWDG_EVENT_TYPE_STYLE:
			private_rebuild (self);
			break;
	}

	return liwdg_widget_widget ()->event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (LIWdgScroll* self)
{
	int h = 0;
	char buf[256];
	LIFntFont* font;

	lifnt_layout_clear (self->text);
	font = liwdg_widget_get_font (LIWDG_WIDGET (self));
	if (font != NULL)
	{
		h = lifnt_font_get_height (font);
		snprintf (buf, 256, "%.2f", self->value);
		lifnt_layout_append_string (self->text, font, buf);
	}
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LIMAT_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
