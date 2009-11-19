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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSpeech Speech
 * @{
 */

#include "ext-speech.h"

liextSpeech*
liext_speech_new (licliModule* module,
                  const char*  text)
{
	liextSpeech* self;
	lifntFont* font;

	self = lisys_malloc (sizeof (liextSpeech));
	if (self == NULL)
		return NULL;
	self->alpha = 1.0f;
	self->timer = 0.0f;
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	lifnt_layout_set_width_limit (self->text, 150);
	font = liwdg_manager_find_font (module->widgets, "default");
	if (font != NULL)
		lifnt_layout_append_string (self->text, font, text);

	return self;
}

void
liext_speech_free (liextSpeech* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
