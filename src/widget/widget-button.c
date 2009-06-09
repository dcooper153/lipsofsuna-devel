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
 * \addtogroup liwdgButton Button
 * @{
 */

#include "widget-button.h"
#include "widget-manager.h"

static int
private_init (liwdgButton*  self,
              liwdgManager* manager);

static void
private_free (liwdgButton* self);

static int
private_event (liwdgButton* self,
               liwdgEvent*  event);

static void
private_rebuild (liwdgButton* self);

const liwdgClass liwdgButtonType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Button", sizeof (liwdgButton),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event,
};

/****************************************************************************/

liwdgWidget*
liwdg_button_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgButtonType);
}

lifntFont*
liwdg_button_get_font (liwdgButton* self)
{
	return self->font;
}

void
liwdg_button_set_font (liwdgButton* self,
                       lifntFont*   font)
{
	self->font = font;
	private_rebuild (self);
}

const char*
liwdg_button_get_text (liwdgButton* self)
{
	return self->string;
}

int
liwdg_button_set_text (liwdgButton* self,
                       const char*  text)
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

/****************************************************************************/

static int
private_init (liwdgButton*  self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_PRESSED, lical_marshal_DATA))
		return 0;
	liwdg_widget_set_focusable (LIWDG_WIDGET (self), 1);
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
private_free (liwdgButton* self)
{
	lifnt_layout_free (self->text);
	free (self->string);
}

static int
private_event (liwdgButton* self,
               liwdgEvent*  event)
{
	int w;
	int h;
	liwdgRect rect;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_KEY_PRESS:
			if (event->key.keycode != SDLK_RETURN)
				return 1;
			return lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED);
		case LIWDG_EVENT_TYPE_KEY_RELEASE:
			if (event->key.keycode != SDLK_RETURN)
				return 1;
			return 0;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			return lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED);
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			w = lifnt_layout_get_width (self->text);
			h = lifnt_layout_get_height (self->text);
			if (liwdg_widget_get_focus_mouse (LIWDG_WIDGET (self)) ||
				liwdg_widget_get_focus_keyboard (LIWDG_WIDGET (self)))
			{
				liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "button-focus", &rect);
				liwdg_widget_paint (LIWDG_WIDGET (self), "button-focus", NULL);
				glColor3f (1.0f, 1.0f, 1.0f);
				lifnt_layout_render (self->text,
					rect.x + (rect.width - w) / 2 + 2,
					rect.y + (rect.height - h) / 2 - 2);
			}
			else
			{
				liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "button", &rect);
				liwdg_widget_paint (LIWDG_WIDGET (self), "button", NULL);
				glColor3f (1.0f, 1.0f, 1.0f);
				lifnt_layout_render (self->text,
					rect.x + (rect.width - w) / 2,
					rect.y + (rect.height - h) / 2);
			}
			break;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (liwdgButton* self)
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
		lifnt_layout_get_height (self->text), h), "button");
}

/** @} */
/** @} */
