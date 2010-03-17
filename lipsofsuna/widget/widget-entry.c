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
 * \addtogroup LIWdgEntry Entry
 * @{
 */

#include "widget-entry.h"
#include "widget-manager.h"

static int
private_init (LIWdgEntry*   self,
              LIWdgManager* manager);

static void
private_free (LIWdgEntry*   self);

static int
private_event (LIWdgEntry* self,
               LIWdgEvent* event);

static void
private_backspace (LIWdgEntry* self);

static void
private_rebuild (LIWdgEntry* self);

/****************************************************************************/

const LIWdgClass*
liwdg_widget_entry ()
{
	static const LIWdgClass clss =
	{
		liwdg_widget_widget, "Entry", sizeof (LIWdgEntry),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

LIWdgWidget*
liwdg_entry_new (LIWdgManager* manager)
{
	return liwdg_widget_new (manager, liwdg_widget_entry ());
}

void
liwdg_entry_clear (LIWdgEntry* self)
{
	char* tmp;

	tmp = realloc (self->string, 1);
	if (tmp != NULL)
		self->string = tmp;
	self->string[0] = '\0';
	private_rebuild (self);
}

int
liwdg_entry_get_editable (LIWdgEntry* self)
{
	return self->editable;
}

void
liwdg_entry_set_editable (LIWdgEntry* self,
                          int         editable)
{
	self->editable = editable;
}

LIFntFont*
liwdg_entry_get_font (LIWdgEntry* self)
{
	return self->font;
}

void
liwdg_entry_set_font (LIWdgEntry* self,
                      LIFntFont*  font)
{
	self->font = font;
	private_rebuild (self);
}

int
liwdg_entry_get_secret (LIWdgEntry* self)
{
	return self->secret;
}

void
liwdg_entry_set_secret (LIWdgEntry* self,
                        int         secret)
{
	self->secret = secret;
	private_rebuild (self);
}

const char*
liwdg_entry_get_text (LIWdgEntry* self)
{
	return self->string;
}

int
liwdg_entry_set_text (LIWdgEntry* self,
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
private_init (LIWdgEntry*   self,
              LIWdgManager* manager)
{
	liwdg_widget_set_focusable (LIWDG_WIDGET (self), 1);
	self->editable = 1;
	self->string = lisys_calloc (1, sizeof (char));
	if (self->string == NULL)
		return 0;
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		lisys_free (self->string);
		return 0;
	}
	liwdg_widget_set_style (LIWDG_WIDGET (self), "entry");
	private_rebuild (self);

	return 1;
}

static void
private_free (LIWdgEntry* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (LIWdgEntry* self,
               LIWdgEvent* event)
{
	int len;
	int focus;
	char* str;
	char* tmp;
	LIWdgManager* manager;
	LIWdgStyle* style;
	LIWdgRect rect;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			if (!self->editable)
				return 1;
			return 0;
		case LIWDG_EVENT_TYPE_KEY_PRESS:
			if (!self->editable)
				return 1;
			switch (event->key.keycode)
			{
				case SDLK_TAB:
				case SDLK_ESCAPE:
					return 1;
				case SDLK_BACKSPACE:
					private_backspace (self);
					return 0;
				case SDLK_RETURN:
					lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "activated", lical_marshal_DATA_PTR, self);
					return 1;
				default:
					break;
			}
			if (event->key.unicode != 0)
			{
				str = listr_wchar_to_utf8 (event->key.unicode);
				if (str != NULL)
				{
					len = strlen (self->string);
					tmp = realloc (self->string, len + strlen (str) + 1);
					if (tmp != NULL)
					{
						self->string = tmp;
						strcpy (self->string + len, str);
					}
					lisys_free (str);
					private_rebuild (self);
					lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self, "edited", lical_marshal_DATA_PTR, self);
				}
			}
			return 0;
		case LIWDG_EVENT_TYPE_KEY_RELEASE:
			if (!self->editable)
				return 1;
			switch (event->key.keycode)
			{
				case SDLK_TAB:
				case SDLK_RETURN:
					return 1;
				default:
					break;
			}
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			/* Draw base. */
			liwdg_widget_get_content (LIWDG_WIDGET (self), &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), NULL);
			/* Draw text. */
			manager = LIWDG_WIDGET (self)->manager;
			style = liwdg_widget_get_style (LIWDG_WIDGET (self));
			glPushAttrib (GL_SCISSOR_BIT);
			glScissor (rect.x, manager->height - rect.y - rect.height, rect.width, rect.height);
			glEnable (GL_SCISSOR_TEST);
			glColor4fv (style->color);
			lifnt_layout_render (self->text, rect.x, rect.y);
			glPopAttrib ();
			return 1;
		case LIWDG_EVENT_TYPE_UPDATE:
			focus = liwdg_widget_get_focused (LIWDG_WIDGET (self));
			if (self->focused != focus)
			{
				self->focused = focus;
				private_rebuild (self);
			}
			return 1;
	}

	return liwdg_widget_widget ()->event (LIWDG_WIDGET (self), event);
}

static void
private_backspace (LIWdgEntry* self)
{
	int len0;
	int len1;
	char* str;
	const char* tmp;
	wchar_t* wstr;

	tmp = self->string;
	len0 = strlen (self->string);
	if (!len0)
		return;

	/* Get wide character length. */
	wstr = listr_utf8_to_wchar (tmp);
	if (wstr == NULL)
		return;
	len1 = wcslen (wstr);
	lisys_assert (len1);

	/* Get length of the last character. */
	str = listr_wchar_to_utf8 (wstr[len1 - 1]);
	lisys_free (wstr);
	if (str == NULL)
		return;
	len1 = strlen (str);
	lisys_free (str);

	/* Discard the last character. */
	self->string[len0 - len1] = '\0';
	private_rebuild (self);
	lical_callbacks_call (LIWDG_WIDGET (self)->manager->callbacks, self,
		"edited", lical_marshal_DATA_PTR, self);
}

static void
private_rebuild (LIWdgEntry* self)
{
	int i;
	int h = 0;

	/* Append text. */
	lifnt_layout_clear (self->text);
	if (self->font != NULL)
	{
		/* Append characters. */
		if (self->secret)
		{
			for (i = 0 ; self->string[i] != '\0' ; i++)
				lifnt_layout_append_string (self->text, self->font, "*");
		}
		else
			lifnt_layout_append_string (self->text, self->font, self->string);

		/* Append cursor. */
		if (self->focused)
			lifnt_layout_append_string (self->text, self->font, "|");

		h = lifnt_font_get_height (self->font);
	}

	/* Request size. */
	liwdg_widget_set_request_internal (LIWDG_WIDGET (self),
		LIMAT_MAX (32, lifnt_layout_get_width (self->text)), h);
}

/** @} */
/** @} */
