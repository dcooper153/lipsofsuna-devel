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
 * \addtogroup LIWdgSpin Spin
 * @{
 */

#include "widget-manager.h"
#include "widget-spin.h"

static int
private_init (LIWdgSpin* self,
              LIWdgManager*  manager);

static void
private_free (LIWdgSpin* self);

static int
private_event (LIWdgSpin* self,
               liwdgEvent*    event);

static void
private_rebuild (LIWdgSpin* self);

const LIWdgClass liwdg_widget_spin =
{
	LIWDG_BASE_STATIC, &liwdg_widget_widget, "Spin", sizeof (LIWdgSpin),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event
};

/****************************************************************************/

LIWdgWidget*
liwdg_spin_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdg_widget_spin);
}

LIFntFont*
liwdg_spin_get_font (LIWdgSpin* self)
{
	return self->font;
}

void
liwdg_spin_set_font (LIWdgSpin* self,
                     LIFntFont* font)
{
	self->font = font;
	private_rebuild (self);
}

float
liwdg_spin_get_value (LIWdgSpin* self)
{
	return self->value;
}

void
liwdg_spin_set_value (LIWdgSpin* self,
                      float      value)
{
	self->value = value;
	private_rebuild (self);
}

/****************************************************************************/

static int
private_init (LIWdgSpin*    self,
              LIWdgManager* manager)
{
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
		return 0;
	liwdg_widget_set_style (LIWDG_WIDGET (self), "spin");
	private_rebuild (self);
	return 1;
}

static void
private_free (LIWdgSpin* self)
{
	lifnt_layout_free (self->text);
}

static int
private_event (LIWdgSpin*  self,
               liwdgEvent* event)
{
	int w;
	int h;
	LIWdgRect rect;
	LIWdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			if (event->button.x < rect.x + rect.width / 2)
				self->value -= 1.0f;
			else
				self->value += 1.0f;
			private_rebuild (self);
			lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			style = liwdg_widget_get_style (LIWDG_WIDGET (self));
			/* Draw base. */
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			/* Draw label. */
			glColor4fv (style->color);
			lifnt_layout_render (self->text,
				rect.x + (rect.width - w) / 2,
				rect.y + (rect.height - h) / 2);
			return 1;
	}

	return liwdg_widget_widget.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (LIWdgSpin* self)
{
	int h = 0;
	char buf[256];

	lifnt_layout_clear (self->text);
	if (self->font != NULL)
	{
		h = lifnt_font_get_height (self->font);
		snprintf (buf, 256, "%.0f", self->value);
		lifnt_layout_append_string (self->text, self->font, buf);
	}
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LIMAT_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
