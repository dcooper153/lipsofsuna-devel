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
 * \addtogroup LIWdgCheck Check
 * @{
 */

#include "widget-check.h"
#include "widget-manager.h"

static int
private_init (LIWdgCheck*   self,
              LIWdgManager* manager);

static void
private_free (LIWdgCheck* self);

static int
private_event (LIWdgCheck* self,
               liwdgEvent* event);

static void
private_rebuild (LIWdgCheck* self);

const LIWdgClass liwdg_widget_check =
{
	LIWDG_BASE_STATIC, &liwdg_widget_widget, "Check", sizeof (LIWdgCheck),
	(LIWdgWidgetInitFunc) private_init,
	(LIWdgWidgetFreeFunc) private_free,
	(LIWdgWidgetEventFunc) private_event
};

/****************************************************************************/

LIWdgWidget*
liwdg_check_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdg_widget_check);
}

int
liwdg_check_get_active (LIWdgCheck* self)
{
	return self->active;
}

void
liwdg_check_set_active (LIWdgCheck* self,
                        int         active)
{
	if (self->active != active)
	{
		self->active = active;
		private_rebuild (self);
	}
}

LIFntFont*
liwdg_check_get_font (LIWdgCheck* self)
{
	return self->font;
}

void
liwdg_check_set_font (LIWdgCheck* self,
                      LIFntFont*  font)
{
	self->font = font;
	private_rebuild (self);
}

const char*
liwdg_check_get_text (LIWdgCheck* self)
{
	return self->string;
}

int
liwdg_check_set_text (LIWdgCheck* self,
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

/****************************************************************************/

static int
private_init (LIWdgCheck*   self,
              LIWdgManager* manager)
{
	liwdg_widget_set_focusable (LIWDG_WIDGET (self), 1);
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
	liwdg_widget_set_style (LIWDG_WIDGET (self), "check");
	private_rebuild (self);

	return 1;
}

static void
private_free (LIWdgCheck* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (LIWdgCheck* self,
               liwdgEvent* event)
{
	int h;
	LIWdgRect rect;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_KEY_PRESS:
			if (event->key.keycode != SDLK_RETURN)
				return 1;
			self->active = !self->active;
			private_rebuild (self);
			lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
			return 0;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			self->active = !self->active;
			private_rebuild (self);
			lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "pressed", lical_marshal_DATA_PTR, self);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			glColor4fv (LIWDG_WIDGET (self)->style->color);
			h = lifnt_layout_get_height (self->text);
			lifnt_layout_render (self->text, rect.x, rect.y + (rect.height - h) / 2 - 2);
			return 1;
	}

	return liwdg_widget_widget.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (LIWdgCheck* self)
{
	int h = 0;

	/* Rebuild layout. */
	lifnt_layout_clear (self->text);
	if (self->font != NULL)
	{
		h = lifnt_font_get_height (self->font);
		lifnt_layout_append_string (self->text, self->font, self->string);
	}

	/* Select state graphics. */
	liwdg_widget_set_state (LIWDG_WIDGET (self), self->active? "active" : NULL);

	/* Rebuild request. */
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LIMAT_MAX (
		lifnt_layout_get_height (self->text), h));
}

/** @} */
/** @} */
