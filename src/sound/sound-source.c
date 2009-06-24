/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lisnd Sound
 * @{
 * \addtogroup lisndSource Source
 * @{
 */

#ifndef LI_DISABLE_SOUND

#include <stdio.h>
#include <stdlib.h>
#include "sound-source.h"

/**
 * \brief Creates an empty sound source.
 *
 * \param system Sound system.
 * \return New sound source or NULL.
 */
lisndSource*
lisnd_source_new (lisndSystem* system)
{
	lisndSource* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lisndSource));
	if (self == NULL)
		return NULL;

	/* Allocate a source. */
	alGenSources (1, &self->source);
	if (alGetError() != AL_NO_ERROR)
	{
		free (self);
		return NULL;
	}
	alSourcef (self->source, AL_REFERENCE_DISTANCE, 5.0f);
	alSourcef (self->source, AL_MAX_DISTANCE, 100.0f);

	return self;
}

/**
 * \brief Creates an sound source and queues a sample.
 *
 * \param system Sound system.
 * \param sample Sample to be queued.
 * \return New sound source or NULL.
 */
lisndSource*
lisnd_source_new_with_sample (lisndSystem* system,
                              lisndSample* sample)
{
	lisndSource* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lisndSource));
	if (self == NULL)
		return NULL;

	/* Allocate a source. */
	alGenSources (1, &self->source);
	if (alGetError() != AL_NO_ERROR)
	{
		free (self);
		return NULL;
	}
	alSourcef (self->source, AL_REFERENCE_DISTANCE, 5.0f);
	alSourcef (self->source, AL_MAX_DISTANCE, 100.0f);

	/* Queue the sample. */
	lisnd_source_queue_sample (self, sample);
	return self;
}

/**
 * \brief Frees the sound source.
 *
 * \param self Sound source.
 */
void
lisnd_source_free (lisndSource* self)
{
	alSourceStop (self->source);
	alDeleteSources (1, &self->source);
	free (self);
}

/**
 * \brief Unqueues samples already processed.
 *
 * You should call this function periodically, though not necessarily every
 * frame, for sources that make use of streaming to avoid the memory usage
 * growing too huge. If you don't queue samples or your sources are short
 * lived, you can omit calling this.
 *
 * \param self Sound source.
 * \return The number of samples in the queue.
 */
int
lisnd_source_update (lisndSource* self)
{
	ALint num;
	ALuint buffer;

	alGetSourcei (self->source, AL_BUFFERS_PROCESSED, &num);
	self->queued -= num;
	while (num--)
		alSourceUnqueueBuffers (self->source, 1, &buffer);
	return self->queued;
}

/**
 * \brief Queues a sample.
 *
 * \param self Sound source.
 * \param sample Sample.
 */
void
lisnd_source_queue_sample (lisndSource* self,
                           lisndSample* sample)
{
	alSourceQueueBuffers (self->source, 1, &sample->buffer);
	self->queued++;
}

/**
 * \brief Sets the looping setting of the source.
 *
 * \param self Sound source.
 * \param looping Nonzero if should loop.
 */
void
lisnd_source_set_looping (lisndSource* self,
                          int          looping)
{
	alSourcei (self->source, AL_LOOPING, looping);
}

/**
 * \brief Checks if the source is in playing state.
 *
 * \param self Sound source.
 */
int
lisnd_source_get_playing (lisndSource* self)
{
	ALint state;

	alGetSourcei (self->source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}

/**
 * \brief Plays or stops the source.
 *
 * \param self Sound source.
 * \param playing Nonzero for playing, zero for stopped.
 */
void
lisnd_source_set_playing (lisndSource* self,
                          int          playing)
{
	if (playing)
		alSourcePlay (self->source);
	else
		alSourcePause (self->source);
}

/**
 * \brief Sets the position of the source.
 *
 * \param self Sound source.
 * \param value Vector.
 */
void
lisnd_source_set_position (lisndSource*       self,
                           const limatVector* value)
{
	alSource3f (self->source, AL_POSITION, -value->x, value->y, -value->z);
}

/**
 * \brief Sets the velocity of the source.
 *
 * \param self Sound source.
 * \param value Vector.
 */
void
lisnd_source_set_velocity (lisndSource*       self,
                           const limatVector* value)
{
	alSource3f (self->source, AL_VELOCITY, value->x, value->y, value->z);
}

#endif

/** @} */
/** @} */

