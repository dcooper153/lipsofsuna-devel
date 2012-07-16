/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#include "sound-module.h"

static void SoundSource_new (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	int tmp;
	int stereo = 0;
	int looping = 0;
	const char* effect;
	LIExtModule* module;
	LIScrData* data;
	LISndSample* sample;
	LISndSource* self;

	/* Get arguments. */
	if (!liscr_args_geti_string (args, 0, &effect))
		return;
	if (liscr_args_geti_bool (args, 1, &tmp) && !tmp)
		stereo = 1;
	if (liscr_args_geti_bool (args, 2, &tmp) && tmp)
		looping = 1;

	/* Find the sample. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_SOUND_SOURCE);
	if (module->sound == NULL)
		return;
	sample = liext_sound_find_sample (module, effect);
	if (sample == NULL)
		return;

	/* Allocate the sound source. */
	self = lisnd_source_new_with_sample (module->sound, sample, stereo);
	if (self == NULL)
		return;
	if (looping)
		lisnd_source_set_looping (self, 1);

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_SOUND_SOURCE, lisnd_source_free);
	if (data == NULL)
	{
		lisnd_source_free (self);
		return;
	}
	liscr_args_seti_stack (args);
#endif
}

static void SoundSource_get_ended (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	liscr_args_seti_bool (args, lisnd_source_get_ended (args->self));
#endif
}

static void SoundSource_set_looping (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		lisnd_source_set_looping (args->self, value);
#endif
}

static void SoundSource_set_pitch (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		lisnd_source_set_pitch (args->self, value);
#endif
}

static void SoundSource_get_playing (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	liscr_args_seti_bool (args, lisnd_source_get_playing (args->self));
#endif
}
static void SoundSource_set_playing (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		lisnd_source_set_playing (args->self, value);
#endif
}

static void SoundSource_set_position (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	LIMatVector value;

	if (liscr_args_geti_vector (args, 0, &value))
		lisnd_source_set_position (args->self, &value);
#endif
}

static void SoundSource_set_velocity (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	LIMatVector value;

	if (liscr_args_geti_vector (args, 0, &value))
		lisnd_source_set_velocity (args->self, &value);
#endif
}

static void SoundSource_set_volume (LIScrArgs* args)
{
#ifndef LI_DISABLE_SOUND
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		lisnd_source_set_volume (args->self, value);
#endif
}

/*****************************************************************************/

void liext_script_sound_source (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_new", SoundSource_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_get_ended", SoundSource_get_ended);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_set_looping", SoundSource_set_looping);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_set_pitch", SoundSource_set_pitch);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_get_playing", SoundSource_get_playing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_set_playing", SoundSource_set_playing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_set_position", SoundSource_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_set_velocity", SoundSource_set_velocity);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_SOUND_SOURCE, "sound_source_set_volume", SoundSource_set_volume);
}

/** @} */
/** @} */
