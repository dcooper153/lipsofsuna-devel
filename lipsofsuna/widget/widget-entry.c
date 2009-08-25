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
 * \addtogroup liwdgEntry Entry
 * @{
 */

#include "widget-entry.h"
#include "widget-manager.h"

static int
private_init (liwdgEntry*   self,
              liwdgManager* manager);

static void
private_free (liwdgEntry*   self);

static int
private_event (liwdgEntry* self,
               liwdgEvent* event);

static void
private_backspace (liwdgEntry* self);

static void
private_rebuild (liwdgEntry* self);

const liwdgClass liwdgEntryType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Entry", sizeof (liwdgEntry),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liwdg_entry_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgEntryType);
}

void
liwdg_entry_clear (liwdgEntry* self)
{
	char* tmp;

	tmp = realloc (self->string, 1);
	if (tmp != NULL)
		self->string = tmp;
	self->string[0] = '\0';
	private_rebuild (self);
}

int
liwdg_entry_get_editable (liwdgEntry* self)
{
	return self->editable;
}

void
liwdg_entry_set_editable (liwdgEntry* self,
                          int         editable)
{
	self->editable = editable;
}

lifntFont*
liwdg_entry_get_font (liwdgEntry* self)
{
	return self->font;
}

void
liwdg_entry_set_font (liwdgEntry* self,
                      lifntFont*  font)
{
	self->font = font;
	private_rebuild (self);
}

int
liwdg_entry_get_secret (liwdgEntry* self)
{
	return self->secret;
}

void
liwdg_entry_set_secret (liwdgEntry* self,
                        int         secret)
{
	self->secret = secret;
	private_rebuild (self);
}

const char*
liwdg_entry_get_text (liwdgEntry* self)
{
	return self->string;
}

int
liwdg_entry_set_text (liwdgEntry* self,
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
private_init (liwdgEntry*   self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_ACTIVATED, lical_marshal_DATA) ||
	    !liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_EDITED, lical_marshal_DATA))
		return 0;
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
	private_rebuild (self);

	return 1;
}

static void
private_free (liwdgEntry* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (liwdgEntry* self,
               liwdgEvent* event)
{
	int len;
	char* str;
	char* tmp;
	liwdgStyle* style;
	liwdgRect rect;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_FOCUS_GAIN:
		case LIWDG_EVENT_TYPE_FOCUS_LOSE:
			if (!event->focus.mouse)
				private_rebuild (self);
			return 0;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
		case LIWDG_EVENT_TYPE_BUTTON_RELEASE:
			if (!self->editable)
				return 1;
			liwdg_widget_set_focus_keyboard (LIWDG_WIDGET (self));
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
					lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_ACTIVATED);
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
					lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_EDITED);
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
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "entry", &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), "entry", NULL);
			/* Draw text. */
			style = liwdg_widget_get_style (LIWDG_WIDGET (self), "entry");
			glScissor (rect.x, rect.y, rect.width, rect.height);
			glEnable (GL_SCISSOR_TEST);
			glColor4fv (style->color);
			lifnt_layout_render (self->text, rect.x, rect.y);
			glDisable (GL_SCISSOR_TEST);
			return 1;
		case LIWDG_EVENT_TYPE_UPDATE:
			return 1;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static void
private_backspace (liwdgEntry* self)
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
	assert (len1);

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
	lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_EDITED);
}

static void
private_rebuild (liwdgEntry* self)
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
		if (liwdg_widget_get_focus_keyboard (LIWDG_WIDGET (self)))
			lifnt_layout_append_string (self->text, self->font, "|");

		h = lifnt_font_get_height (self->font);
	}

	/* Request size. */
	liwdg_widget_set_style_request (LIWDG_WIDGET (self),
		LI_MAX (32, lifnt_layout_get_width (self->text)), h, "entry");
}

/** @} */
/** @} */
