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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtSound Sound
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/sound"
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
		object = liscr_data_get_data (data);
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		source = liext_sound_set_effect (module, object->id, effect, flags);
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
 * --- Position of the listener.
 * -- @name Sound.listener_position
 * -- @class table
 */
static void Sound_get_listener_position (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
	liscr_args_seti_vector (args, &module->listener_position);
}
static void Sound_set_listener_position (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector value;

	if (liscr_args_geti_vector (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		module->listener_position = value;
	}
}

/* @luadoc
 * --- Rotation of the listener.
 * -- @name Sound.listener_rotation
 * -- @class table
 */
static void Sound_get_listener_rotation (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
	liscr_args_seti_quaternion (args, &module->listener_rotation);
}
static void Sound_set_listener_rotation (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatQuaternion value;

	if (liscr_args_geti_quaternion (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		module->listener_rotation = value;
	}
}

/* @luadoc
 * --- Velocity of the listener.
 * -- @name Sound.listener_velocity
 * -- @class table
 */
static void Sound_get_listener_velocity (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
	liscr_args_seti_vector (args, &module->listener_velocity);
}
static void Sound_set_listener_velocity (LIScrArgs* args)
{
	LIExtModule* module;
	LIMatVector value;

	if (liscr_args_geti_vector (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		module->listener_velocity = value;
	}
}

/* @luadoc
 * --- Music track name.
 * -- @name Sound.music
 * -- @class table
 */
static void Sound_set_music (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	const char* value;
	LIExtModule* module;

	if (liscr_args_geti_string (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		liext_sound_set_music (module, value);
	}
#endif
}

/* @luadoc
 * --- Music fading time.
 * -- @name Sound.music_fading
 * -- @class table
 */
static void Sound_set_music_fading (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		value = LIMAT_MAX (0.0f, value);
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		liext_sound_set_music_fading (module, value);
	}
#endif
}

/* @luadoc
 * --- Music volume.
 * -- @name Sound.music_volume
 * -- @class table
 */
static void Sound_set_music_volume (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;
	LIExtModule* module;

	if (liscr_args_geti_float (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SOUND);
		liext_sound_set_music_volume (module, value);
	}
#endif
}

/*****************************************************************************/

void liext_script_sound (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SOUND, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "effect", Sound_effect);
	liscr_class_insert_cfunc (self, "get_listener_position", Sound_get_listener_position);
	liscr_class_insert_cfunc (self, "set_listener_position", Sound_set_listener_position);
	liscr_class_insert_cfunc (self, "get_listener_rotation", Sound_get_listener_rotation);
	liscr_class_insert_cfunc (self, "set_listener_rotation", Sound_set_listener_rotation);
	liscr_class_insert_cfunc (self, "get_listener_velocity", Sound_get_listener_velocity);
	liscr_class_insert_cfunc (self, "set_listener_velocity", Sound_set_listener_velocity);
	liscr_class_insert_cfunc (self, "set_music", Sound_set_music);
	liscr_class_insert_cfunc (self, "set_music_fading", Sound_set_music_fading);
	liscr_class_insert_cfunc (self, "set_music_volume", Sound_set_music_volume);
}

/** @} */
/** @} */
