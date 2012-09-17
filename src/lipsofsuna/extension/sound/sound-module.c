/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef LI_DISABLE_SOUND
static int private_tick (
	LIExtModule* self,
	float        secs);
#endif

/*****************************************************************************/

LIMaiExtensionInfo liext_sound_info =
{
	LIMAI_EXTENSION_VERSION, "Sound",
	liext_sound_new,
	liext_sound_free
};

LIExtModule* liext_sound_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->music_looping = 0;
	self->music_volume = 1.0f;
	self->music_fading = 1.0f;
	self->listener_rotation = limat_quaternion_identity ();

#ifndef LI_DISABLE_SOUND
	/* Initialize sound. */
	self->system = lisnd_system_new ();
	if (self->system != NULL)
		self->sound = lisnd_manager_new (self->system);
	else
		printf ("WARNING: cannot initialize sound: %s\n", lisys_error_get_string ());

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "tick", 1, private_tick, self, self->calls + 0))
	{
		liext_sound_free (self);
		return NULL;
	}
#endif

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SOUND, self);
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_SOUND_SOURCE, self);
	liext_script_sound (program->script);
	liext_script_sound_source (program->script);

	return self;
}

void liext_sound_free (
	LIExtModule* self)
{
#ifndef LI_DISABLE_SOUND
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Free music. */
	if (self->music != NULL)
		lisnd_source_free (self->music);
	if (self->music_fade != NULL)
		lisnd_source_free (self->music_fade);

	/* Disable sound. */
	if (self->sound != NULL)
		lisnd_manager_free (self->sound);
	if (self->sound != NULL)
		lisnd_system_free (self->system);
#endif

	lisys_free (self);
}

#ifndef LI_DISABLE_SOUND
/**
 * \brief Finds a sound sample by name.
 *
 * \param self Module.
 * \param name Name of the sample.
 * \return Sample owned by the module or NULL.
 */
LISndSample* liext_sound_find_sample (
	LIExtModule* self,
	const char*  name)
{
	int ret;
	char* file;
	const char* path;
	LISndSample* sample;

	/* Check for existing. */
	sample = lisnd_manager_get_sample (self->sound, name);
	if (sample != NULL)
		return sample;

	/* Try to load FLAC. */
	file = lisys_string_concat (name, ".flac");
	if (file == NULL)
		return NULL;
	path = lipth_paths_find_file (self->program->paths, file);
	lisys_free (file);
	if (path != NULL)
	{
		ret = lisnd_manager_set_sample (self->sound, name, path);
		if (ret)
			return lisnd_manager_get_sample (self->sound, name);
	}

	/* Try to load OGG. */
	file = lisys_string_concat (name, ".ogg");
	if (file == NULL)
		return NULL;
	path = lipth_paths_find_file (self->program->paths, file);
	lisys_free (file);
	if (path != NULL)
	{
		ret = lisnd_manager_set_sample (self->sound, name, path);
		if (ret)
			return lisnd_manager_get_sample (self->sound, name);
	}

	return 0;
}

int liext_sound_set_music (
	LIExtModule* self,
	const char*  value)
{
	LISndSource* music;
	LISndSample* sample;

	/* Find sample. */
	if (self->sound == NULL)
		return 1;
	sample = liext_sound_find_sample (self, value);
	if (sample == NULL)
		return 0;

	/* Fade in a new music track. */
	music = lisnd_source_new (self->sound, 1);
	if (music == NULL)
		return 0;
	lisnd_source_queue_sample (music, sample);
	lisnd_source_set_fading (music, 0.0f, 1.0f / self->music_fading);
	lisnd_source_set_volume (music, self->music_volume);
	lisnd_source_set_looping (music, self->music_looping);
	lisnd_source_set_playing (music, 1);

	/* Fade out the old music track. */
	if (self->music_fade != NULL)
		lisnd_source_free (self->music_fade);
	if (self->music != NULL)
		lisnd_source_set_fading (self->music, 1.0f, -1.0f / self->music_fading);
	self->music_fade = self->music;
	self->music = music;

	return 1;
}

void liext_sound_set_music_fading (
	LIExtModule* self,
	float        value)
{
	self->music_fading = value;
}

void liext_sound_set_music_looping (
	LIExtModule* self,
	int          value)
{
	if (self->music_looping != value)
	{
		self->music_looping = value;
		if (self->music != NULL)
			lisnd_source_set_looping (self->music, self->music_looping);
	}
}

void liext_sound_set_music_volume (
	LIExtModule* self,
	float        value)
{
	if (self->sound == NULL)
		return;
	if (value < 0.0f)
		value = 0.0f;
	if (value > 1.0f)
		value = 1.0f;
	self->music_volume = value;

	if (self->music != NULL)
		lisnd_source_set_volume (self->music, value);
	if (self->music_fade != NULL)
		lisnd_source_set_volume (self->music_fade, value);
}
#endif

/*****************************************************************************/

#ifndef LI_DISABLE_SOUND
static int private_tick (
	LIExtModule* self,
	float        secs)
{
	LIMatVector direction;
	LIMatVector velocity;
	LIMatVector up;

	/* Update listener position. */
	velocity = self->listener_velocity;
	direction = limat_quaternion_get_basis (self->listener_rotation, 2);
	up = limat_quaternion_get_basis (self->listener_rotation, 1);
	if (self->system != NULL)
		lisnd_system_set_listener (self->system, &self->listener_position, &velocity, &direction, &up);

	/* Update music. */
	if (self->music_fade != NULL)
	{
		if (!lisnd_source_update (self->music_fade, secs))
		{
			limai_program_event (self->program, "music-fade-ended", NULL);
			lisnd_source_free (self->music_fade);
			self->music_fade = NULL;
		}
	}
	if (self->music != NULL)
	{
		lisnd_source_set_position (self->music, &self->listener_position);
		if (!lisnd_source_update (self->music, secs))
		{
			limai_program_event (self->program, "music-ended", NULL);
			lisnd_source_free (self->music);
			self->music = NULL;
		}
	}

	/* Update sound sources. */
	if (self->sound != NULL)
		lisnd_manager_update (self->sound, secs);

	return 1;
}
#endif

/** @} */
/** @} */
