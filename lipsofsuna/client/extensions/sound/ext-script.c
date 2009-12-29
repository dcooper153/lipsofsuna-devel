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

#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Sound"
 * ---
 * -- Sound playback.
 * -- @name Sound
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Music track name.
 * -- @name Sound.music
 * -- @class table
 */
static void Sound_setter_music (liscrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	const char* value;
	liextModule* module;

	if (liscr_args_geti_string (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		liext_module_set_music (module, value);
	}
#endif
}

/* @luadoc
 * ---
 * -- Music volume.
 * -- @name Sound.music_volume
 * -- @class table
 */
static void Sound_setter_music_volume (liscrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;
	liextModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		liext_module_set_music_volume (module, value);
	}
#endif
}

/*****************************************************************************/

void
liextSoundScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SOUND, data);
	liscr_class_insert_cvar (self, "music", NULL, Sound_setter_music);
	liscr_class_insert_cvar (self, "music_volume", NULL, Sound_setter_music_volume);
}

/** @} */
/** @} */
/** @} */
