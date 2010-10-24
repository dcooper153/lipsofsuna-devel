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
 * \addtogroup LIWdg Widget
 * @{
 * \addtogroup LIWdgButton Button
 * @{
 */

#include "widget-button.h"
#include "widget-manager.h"

static int
private_init (LIWdgButton*  self,
              LIWdgManager* manager);

static void
private_free (LIWdgButton* self);

static int
private_event (LIWdgButton* self,
               LIWdgEvent*  event);

static void 
private_rebuild (LIWdgButton* self);

/****************************************************************************/

const LIWdgClass*
liwdg_widget_button ()
{
	static const LIWdgClass clss =
	{
		liwdg_widget_widget, "Button", sizeof (LIWdgButton),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event,
	};
	return &clss;
}

LIWdgWidget*
liwdg_button_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, liwdg_widget_button ());
}

const char*
liwdg_button_get_text (LIWdgButton* self)
{
	return self->string;
}

int
liwdg_button_set_text (LIWdgButton* self,
                       const char*  text)
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

/****************************************************************************/

static int
private_init (LIWdgButton*  self,
              LIWdgManager* manager)
{
	liwdg_widget_set_focusable (LIWDG_WIDGET (self), 1);
	self->string = lisys_calloc (1, 1);
	if (self->string == NULL)
		return 0;
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		lisys_free (self->string);
		return 0;
	}
	liwdg_widget_set_style (LIWDG_WIDGET (self), "button");
	private_rebuild (self);

	return 1;
}

static void
private_free (LIWdgButton* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (LIWdgButton* self,
               LIWdgEvent*  event)
{
	LIWdgRect rect;
	LIWdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_KEY_PRESS:
			if (event->key.keycode != SDLK_RETURN)
				return 1;
			return lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
		case LIWDG_EVENT_TYPE_KEY_RELEASE:
			if (event->key.keycode != SDLK_RETURN)
				return 1;
			return 0;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			return lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			return 0;
		case LIWDG_EVENT_TYPE_STYLE:
			private_rebuild (self);
			break;
		case LIWDG_EVENT_TYPE_RENDER:
			style = liwdg_widget_get_style (LIWDG_WIDGET (self));
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			liwdg_style_paint_text (style, self->text, self->halign, self->valign, &rect);
			break;
		case LIWDG_EVENT_TYPE_UPDATE:
			if (liwdg_widget_get_focused (LIWDG_WIDGET (self)))
				liwdg_widget_set_state (LIWDG_WIDGET (self), "focus");
			else
				liwdg_widget_set_state (LIWDG_WIDGET (self), NULL);
			break;
	}

	return liwdg_widget_widget ()->event (LIWDG_WIDGET (self), event);
}

static void private_rebuild (
	LIWdgButton* self)
{
	int h = 0;
	int limit;
	LIFntFont* font;

	/* Set the desired label width. */
	font = liwdg_widget_get_font (LIWDG_WIDGET (self));
	limit = LIWDG_WIDGET (self)->userrequest.width;
	if (limit >= 0)
		lifnt_layout_set_width_limit (self->text, limit);
	else
		lifnt_layout_set_width_limit (self->text, 0);

	/* Rebuild the label layout. */
	lifnt_layout_clear (self->text);
	if (font != NULL)
	{
		h = lifnt_font_get_height (font);
		lifnt_layout_append_string (self->text, font, self->string);
	}

	/* Recalculate the internal size request. */
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LIMAT_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
