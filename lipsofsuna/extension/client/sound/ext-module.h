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
 * \addtogroup liextcliSound Sound
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/script.h>
#ifndef LI_DISABLE_SOUND
#include <lipsofsuna/sound.h>
#endif

#define LIEXT_SCRIPT_SOUND "Sound"

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LICliClient* client;
#ifndef LI_DISABLE_SOUND
	LIAlgU32dic* objects;
	LICalHandle calls[1];
	LISndSystem* system;
	LISndManager* sound;
	LISndSource* music;
#endif
};

LIExtModule*
liext_module_new (LIMaiProgram* program);

void
liext_module_free (LIExtModule* self);

#ifndef LI_DISABLE_SOUND
LISndSample*
liext_module_find_sample_by_id (LIExtModule* self,
                                int          id);

LISndSample*
liext_module_find_sample_by_name (LIExtModule* self,
                                  const char*  name);

int
liext_module_set_effect (LIExtModule* self,
                         uint32_t     object,
                         uint32_t     effect,
                         int          flags);

int
liext_module_set_music (LIExtModule* self,
                        const char*  value);

void
liext_module_set_music_volume (LIExtModule* self,
                               float        value);
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
typedef struct _LIExtObject LIExtObject;
struct _LIExtObject
{
	LIAlgList* sounds;
};

LIExtObject*
liext_object_new ();

void
liext_object_free (LIExtObject* self);
#endif

/*****************************************************************************/

void
liext_script_sound (LIScrClass* self,
                  void*       data);

#endif

/** @} */
/** @} */
/** @} */
