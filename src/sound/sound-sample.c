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
 * \addtogroup lisnd Sound 
 * @{
 * \addtogroup lisndSample Sample
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <system/lips-system.h>
#include "sound-sample.h"

/**
 * \brief Creates a new sample.
 *
 * The sample is loaded from an Ogg Vorbis file.
 *
 * \param system Sound system.
 * \param file File name.
 * \return New sample or NULL.
 */
lisndSample*
lisnd_sample_new (lisndSystem* system,
                  const char*  file)
{
	int bs = -1;
	int freq;
	long num;
	void* buffer;
	ogg_int64_t pos;
	ogg_int64_t len;
	vorbis_info* info;
	lisndSample* self;
	OggVorbis_File vorbis;

	/* Allocate self. */
	self = calloc (1, sizeof (lisndSample));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Allocate a buffer. */
	alGenBuffers (1, &self->buffer);
	if (alGetError() != AL_NO_ERROR)
	{
		free (self);
		return NULL;
	}

	/* Initialize the decoder. */
	num = ov_fopen ((char*) file, &vorbis);
	if (num != 0)
	{
		if (num == OV_EREAD)
			lisys_error_set (EIO, "cannot open `%s'", file);
		else
			lisys_error_set (EINVAL, "cannot read `%s'", file);
		lisnd_sample_free (self);
		return NULL;
	}

	/* Get stream information. */
	info = ov_info (&vorbis, -1);
	freq = info->rate;
	len = 2 * info->channels * ov_pcm_total (&vorbis, -1);
	pos = 0;

	/* Allocate the output buffer. */
	buffer = malloc (len);
	if (buffer == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lisnd_sample_free (self);
		ov_clear (&vorbis);
		return NULL;
	}

	/* Decode the stream. */
	while (1)
	{
		assert (pos <= len);
		num = ov_read (&vorbis, buffer + pos, len - pos, 0, 2, 1, &bs);
		if (num <= 0)
			break;
		pos += num;
	}
	assert (pos == len);

	/* Upload to OpenAL. */
	if (info->channels == 1)
		alBufferData (self->buffer, AL_FORMAT_MONO16, buffer, len, freq);
	else
		alBufferData (self->buffer, AL_FORMAT_STEREO16, buffer, len, freq);
	ov_clear (&vorbis);
	free (buffer);

	return self;
}

/**
 * \brief Frees the sample.
 *
 * \param self Sample.
 */
void
lisnd_sample_free (lisndSample* self)
{
	alDeleteBuffers (1, &self->buffer);
	free (self);
}

/** @} */
/** @} */

