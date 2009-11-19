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

#ifndef __EXT_SPEECH_H__
#define __EXT_SPEECH_H__

#include <client/lips-client.h>
#include <font/lips-font.h>

#define LIEXT_SPEECH_TIMEOUT 10.0f

typedef struct _liextSpeech liextSpeech;
struct _liextSpeech
{
	float timer;
	float alpha;
	lifntLayout* text;
};

liextSpeech*
liext_speech_new (licliModule* module,
                  const char*  text);

void
liext_speech_free (liextSpeech* self);

#endif

/** @} */
/** @} */
/** @} */
