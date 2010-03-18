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

#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Sound"
 * ---
 * -- Sound playback.
 * -- @name Sound
 * -- @class table
 */

/* @luadoc
 * --- Plays a sound effect.
 * --
 * -- @param clss Sound class.
 * -- @param args Arguments.<ul>
 * --   <li>effect: Sample string. (required)</li>
 * --   <li>object: Object. (required)</li>
 * --   <li>pitch: Pitch shift multiplier.</li>
 * --   <li>volume: Volume multiplier.</li></ul>
 * function Sound.effect(clss, args)
 */
static void Sound_effect (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	int flags = 0;
	float pitch;
	float volume;
	const char* effect;
	LIEngObject* object;
	LIExtModule* module;
	LIScrData* data;
	LISndSource* source;

	if (liscr_args_gets_string (args, "effect", &effect) &&
	    liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
	{
		object = data->data;
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		source = liext_module_set_effect (module, object->id, effect, flags);
		if (source != NULL)
		{
			if (liscr_args_gets_float (args, "pitch", &pitch))
				lisnd_source_set_pitch (source, pitch);
			if (liscr_args_gets_float (args, "volume", &volume))
				lisnd_source_set_volume (source, volume);
		}
	}
#endif
}

/* @luadoc
 * --- Music track name.
 * -- @name Sound.music
 * -- @class table
 */
static void Sound_setter_music (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	const char* value;
	LIExtModule* module;

	if (liscr_args_geti_string (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		liext_module_set_music (module, value);
	}
#endif
}

/* @luadoc
 * --- Music volume.
 * -- @name Sound.music_volume
 * -- @class table
 */
static void Sound_setter_music_volume (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		liext_module_set_music_volume (module, value);
	}
#endif
}

/*****************************************************************************/

void
liext_script_sound (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SOUND, data);
	liscr_class_insert_cfunc (self, "effect", Sound_effect);
	liscr_class_insert_cvar (self, "music", NULL, Sound_setter_music);
	liscr_class_insert_cvar (self, "music_volume", NULL, Sound_setter_music_volume);
}

/** @} */
/** @} */
/** @} */
