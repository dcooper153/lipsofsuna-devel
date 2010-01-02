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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSpeech Speech
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <client/lips-client.h>
#include <script/lips-script.h>

#define LIEXT_SCRIPT_SPEECH "Lips.Speech"

typedef struct _liextModule liextModule;
struct _liextModule
{
	licliClient* client;
	lialgU32dic* objects;
	licalHandle calls[2];
};

liextModule*
liext_module_new (licliClient* client);

void
liext_module_free (liextModule* self);

int
liext_module_set_speech (liextModule* self,
                         uint32_t     object,
                         const char*  message);

/*****************************************************************************/

typedef struct _liextObject liextObject;
struct _liextObject
{
	lialgList* speech;
	limatVector position;
};

liextObject*
liext_object_new ();

void
liext_object_free (liextObject* self);

/*****************************************************************************/

void
liextSpeechScript (liscrClass* self,
                   void*       data);

#endif

/** @} */
/** @} */
/** @} */
