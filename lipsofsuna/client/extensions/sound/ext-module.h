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
 * \addtogroup liextcliSound Sound
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <client/lips-client.h>
#include <script/lips-script.h>
#ifndef LI_DISABLE_SOUND
#include <sound/lips-sound.h>
#endif

#define LIEXT_SCRIPT_SOUND "Lips.Sound"

typedef struct _liextModule liextModule;
struct _liextModule
{
	licliModule* module;
#ifndef LI_DISABLE_SOUND
	lialgU32dic* objects;
	licalHandle calls[2];
	lisndSystem* system;
	lisndManager* sound;
	lisndSource* music;
#endif
};

liextModule*
liext_module_new (licliModule* module);

void
liext_module_free (liextModule* self);

#ifndef LI_DISABLE_SOUND
lisndSample*
liext_module_find_sample_by_id (liextModule* self,
                                int          id);

lisndSample*
liext_module_find_sample_by_name (liextModule* self,
                                  const char*  name);

int
liext_module_set_effect (liextModule* self,
                         uint32_t     object,
                         uint32_t     effect,
                         int          flags);

int
liext_module_set_music (liextModule* self,
                        const char*  value);

void
liext_module_set_music_volume (liextModule* self,
                               float        value);
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
typedef struct _liextObject liextObject;
struct _liextObject
{
	lialgList* sounds;
};

liextObject*
liext_object_new ();

void
liext_object_free (liextObject* self);
#endif

/*****************************************************************************/

void
liextSoundScript (liscrClass* self,
                  void*       data);

#endif

/** @} */
/** @} */
/** @} */
