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
 * \addtogroup liwdgSpin Spin
 * @{
 */

#include "widget-manager.h"
#include "widget-spin.h"

static int
private_init (liwdgSpin* self,
              liwdgManager*  manager);

static void
private_free (liwdgSpin* self);

static int
private_event (liwdgSpin* self,
               liwdgEvent*    event);

static void
private_rebuild (liwdgSpin* self);

const liwdgClass liwdgSpinType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Spin", sizeof (liwdgSpin),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liwdg_spin_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgSpinType);
}

lifntFont*
liwdg_spin_get_font (liwdgSpin* self)
{
	return self->font;
}

void
liwdg_spin_set_font (liwdgSpin* self,
                     lifntFont* font)
{
	self->font = font;
	private_rebuild (self);
}

float
liwdg_spin_get_value (liwdgSpin* self)
{
	return self->value;
}

void
liwdg_spin_set_value (liwdgSpin* self,
                      float      value)
{
	self->value = value;
	private_rebuild (self);
}

/****************************************************************************/

static int
private_init (liwdgSpin*    self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_PRESSED, lical_marshal_DATA_PTR))
		return 0;
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
		return 0;
	private_rebuild (self);
	return 1;
}

static void
private_free (liwdgSpin* self)
{
	lifnt_layout_free (self->text);
}

static int
private_event (liwdgSpin*  self,
               liwdgEvent* event)
{
	int w;
	int h;
	liwdgRect rect;
	liwdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			if (event->button.x < rect.x + rect.width / 2)
				self->value -= 1.0f;
			else
				self->value += 1.0f;
			private_rebuild (self);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED, self);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			style = liwdg_widget_get_style (LIWDG_WIDGET (self), "spin");
			/* Draw base. */
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "spin", &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), "spin", NULL);
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
private_rebuild (liwdgSpin* self)
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
	liwdg_widget_set_style_request (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LI_MAX (
		lifnt_layout_get_height (self->text), h), "spin");
}

/** @} */
/** @} */
