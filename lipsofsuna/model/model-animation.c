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
 * \addtogroup limdl Model
 * @{
 * \addtogroup LIMdlAnimation Animation
 * @{
 */

#include <lipsofsuna/string.h>
#include "model-animation.h"

#define TIMESCALE 0.02f

static LIMatTransform*
private_frame_transform (LIMdlAnimation* self,
                         int             chan,
                         int             frame);

/*****************************************************************************/

/**
 * \brief Creates a soft copy of the animation.
 * \param anim Animation.
 * \return New animation or NULL.
 */
LIMdlAnimation* limdl_animation_new_copy (
	LIMdlAnimation* anim)
{
	int i;
	LIMdlAnimation* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlAnimation));
	if (self == NULL)
		return NULL;
	self->length = anim->length;
	self->blendin = anim->blendin;
	self->blendout = anim->blendout;
	if (anim->name != NULL)
		self->name = listr_dup (anim->name);

	/* Copy channels. */
	if (anim->channels.count)
	{
		self->channels.array = lisys_calloc (anim->channels.count, sizeof (char*));
		if (self->channels.array == NULL)
		{
			limdl_animation_free (self);
			return NULL;
		}
		self->channels.count = anim->channels.count;
		for (i = 0 ; i < self->channels.count ; i++)
		{
			self->channels.array[i] = listr_dup (anim->channels.array[i]);
			if (self->channels.array[i] == NULL)
			{
				limdl_animation_free (self);
				return NULL;
			}
		}
	}

	/* Copy frames. */
	if (anim->buffer.count)
	{
		self->buffer.array = lisys_calloc (anim->buffer.count, sizeof (LIMdlFrame));
		if (self->buffer.array == NULL)
		{
			limdl_animation_free (self);
			return NULL;
		}
		self->buffer.count = anim->buffer.count;
		memcpy (self->buffer.array, anim->buffer.array,
			self->buffer.count * sizeof (LIMdlFrame));
	}

	return self;
}

/**
 * \brief Frees the animation.
 * \param self Animation.
 */
void limdl_animation_free (
	LIMdlAnimation* self)
{
	limdl_animation_clear (self);
	lisys_free (self);
}

/**
 * \brief Adds a channel to the animation.
 *
 * If the channel exists already, nothing is done.
 *
 * \param self Animation.
 * \param name Node name.
 * \return Nonzero on success.
 */
int limdl_animation_insert_channel (
	LIMdlAnimation* self,
	const char*     name)
{
	int chan;
	int frame;
	char* str;
	char** tmp;
	LIMdlFrame* buffer;

	/* Check for existence. */
	if (limdl_animation_get_channel (self, name) != -1)
		return 1;

	/* Duplicate the name. */
	str = listr_dup (name);
	if (str == NULL)
		return 0;

	/* Rebuild the transformation buffer. */
	if (self->length)
	{
		buffer = lisys_calloc ((self->channels.count + 1) * self->length, sizeof (LIMdlFrame));
		if (buffer == NULL)
		{
			lisys_free (str);
			return 0;
		}
		for (frame = 0 ; frame < self->length ; frame++)
		{
			for (chan = 0 ; chan < self->channels.count ; chan++)
			{
				buffer[(self->channels.count + 1) * frame + chan].transform =
					self->buffer.array[self->channels.count * frame + chan].transform;
			}
			buffer[(self->channels.count + 1) * frame + chan].transform = limat_transform_identity ();
		}
	}

	/* Allocate a new channel. */
	tmp = lisys_realloc (self->channels.array, (self->channels.count + 1) * sizeof (char*));
	if (tmp == NULL)
	{
		lisys_free (buffer);
		lisys_free (str);
		return 0;
	}
	self->channels.array = tmp;
	self->channels.array[self->channels.count] = str;
	self->channels.count++;

	/* Use the new transformation buffer. */
	if (self->length)
	{
		lisys_free (self->buffer.array);
		self->buffer.array = buffer;
		self->buffer.count = self->channels.count * self->length;
	}

	return 1;
}

/**
 * \brief Clears the name and channels of the animation.
 * \param self Animation.
 */
void limdl_animation_clear (
	LIMdlAnimation* self)
{
	int i;

	if (self->channels.array != NULL)
	{
		for (i = 0 ; i < self->channels.count ; i++)
			lisys_free (self->channels.array[i]);
		lisys_free (self->channels.array);
	}
	lisys_free (self->buffer.array);
	lisys_free (self->name);
}

/**
 * \brief Reads the animation from a stream.
 * \param self Animation.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int limdl_animation_read (
	LIMdlAnimation* self,
	LIArcReader*    reader)
{
	int i;
	uint32_t count0;
	uint32_t count1;
	LIMatTransform* transform;

	/* Read the header. */
	if (!liarc_reader_get_text (reader, "", &self->name) ||
	    !liarc_reader_get_uint32 (reader, &count0) ||
	    !liarc_reader_get_uint32 (reader, &count1))
		return 0;

	/* Allocate channels. */
	self->channels.count = count0;
	if (count0)
	{
		self->channels.array = lisys_calloc (count0, sizeof (char*));
		if (self->channels.array == NULL)
			return 0;
	}

	/* Read channels. */
	for (i = 0 ; i < self->channels.count ; i++)
	{
		if (!liarc_reader_get_text (reader, "", self->channels.array + i))
			return 0;
	}

	/* Allocate frames. */
	self->length = count1;
	self->buffer.count = count0 * count1;
	if (self->buffer.count)
	{
		self->buffer.array = lisys_calloc (self->buffer.count, sizeof (LIMdlFrame));
		if (self->buffer.array == NULL)
			return 0;
	}

	/* Read frames. */
	for (i = 0 ; i < self->buffer.count ; i++)
	{
		transform = &self->buffer.array[i].transform;
		if (!liarc_reader_get_float (reader, &transform->position.x) ||
			!liarc_reader_get_float (reader, &transform->position.y) ||
			!liarc_reader_get_float (reader, &transform->position.z) ||
			!liarc_reader_get_float (reader, &transform->rotation.x) ||
			!liarc_reader_get_float (reader, &transform->rotation.y) ||
			!liarc_reader_get_float (reader, &transform->rotation.z) ||
			!liarc_reader_get_float (reader, &transform->rotation.w))
			return 0;
	}

	return 1;
}

/**
 * \brief Gets the index of a channel.
 *
 * \param self Animation.
 * \param name Channel name.
 * \return Channel index or -1.
 */
int limdl_animation_get_channel (
	LIMdlAnimation* self,
	const char*     name)
{
	int i;

	for (i = 0 ; i < self->channels.count ; i++)
	{
		if (!strcmp (self->channels.array[i], name))
			return i;
	}

	return -1;
}

/**
 * \brief Gets the duration of the animation in seconds.
 *
 * \param self Animation.
 * \return Duration in seconds.
 */
float limdl_animation_get_duration (
	const LIMdlAnimation* self)
{
	return (self->length - 1) * TIMESCALE;
}

/**
 * \brief Sets the number of frames in the animation.
 * \param self Animation.
 * \param value Frame count.
 * \return Nonzero on success.
 */
int limdl_animation_set_length (
	LIMdlAnimation* self,
	int             value)
{
	int i;
	LIMdlFrame* tmp;

	if (value == self->length)
		return 1;
	if (!self->channels.count)
	{
		self->length = value;
		return 1;
	}
	if (!value)
	{
		/* Set to empty. */
		lisys_free (self->buffer.array);
		self->buffer.array = NULL;
	}
	else if (value < self->length)
	{
		/* Shrink. */
		tmp = lisys_realloc (self->buffer.array, self->channels.count * value * sizeof (LIMdlFrame));
		if (tmp != NULL)
			self->buffer.array = tmp;
	}
	else
	{
		/* Expand. */
		tmp = lisys_realloc (self->buffer.array, self->channels.count * value * sizeof (LIMdlFrame));
		if (tmp == NULL)
			return 0;
		self->buffer.array = tmp;
		for (i = self->channels.count * self->length ; i < self->channels.count * value ; i++)
			self->buffer.array[i].transform = limat_transform_identity ();
	}
	self->length = value;
	self->buffer.count = self->channels.count * self->length;

	return 1;
}

/**
 * \brief Sets the node transformation.
 *
 * \param self Animation.
 * \param name Channel name.
 * \param frame Frame number.
 * \param value Node transformation.
 * \return Nonzero on success.
 */
int limdl_animation_set_transform (
	LIMdlAnimation*       self,
	const char*           name,
	int                   frame,
	const LIMatTransform* value)
{
	int chan;

	lisys_assert (frame >= 0);
	lisys_assert (frame < self->length);

	chan = limdl_animation_get_channel (self, name);
	if (chan == -1)
		return 0;
	self->buffer.array[self->channels.count * frame + chan].transform = *value;

	return 1;
}

/**
 * \brief Gets the node transformation.
 *
 * \param self Animation.
 * \param name Channel name.
 * \param secs Animation position.
 * \param value Return location for the transformation.
 * \return Nonzero on success.
 */
int limdl_animation_get_transform (
	LIMdlAnimation* self,
	const char*     name,
	float           secs,
	LIMatTransform* value)
{
	int chan;
	int frame;
	float blend;
	float frames;
	LIMatTransform* t0;
	LIMatTransform* t1;

	chan = limdl_animation_get_channel (self, name);
	if (chan == -1)
		return 0;

	frames = secs / TIMESCALE;
	frame = (int) frames;
	if (frame <= 0)
		*value = *private_frame_transform (self, chan, 0);
	else if (frame >= self->length - 1)
		*value = *private_frame_transform (self, chan, self->length - 1);
	else
	{
		blend = frames - frame;
		t0 = private_frame_transform (self, chan, frame);
		t1 = private_frame_transform (self, chan, frame + 1);
		value->position = limat_vector_lerp (t1->position, t0->position, blend);
		value->rotation = limat_quaternion_nlerp (t1->rotation, t0->rotation, blend);
	}

	return 1;
}

/**
 * \brief Gets the weight of the animation at certain point.
 *
 * The three weights passed allow different blending patters,
 * mostly needed for looping:
 *
 * - Loop once: [0,1,0]
 * - Loop twice: [0,1,1] [1,1,0]
 * - Loop thrice: [0,1,1] [1,1,1] [1,1,0]
 * - Loop forever: [1,1,1]
 *
 * \param self Animation.
 * \param time Time into the animation in seconds.
 * \param sweight Starting weight.
 * \param mweight Middle weight.
 * \param eweight Ending weight.
 * \return Value within range from 0.0 to 1.0.
 */
float limdl_animation_get_weight (
	const LIMdlAnimation* self,
	float                 time,
	float                 sweight,
	float                 mweight,
	float                 eweight)
{
#warning No animation weights
#if 0
	if (time < 0.0f)
		return sweight;
	if (time < self->blendin)
		return sweight + (mweight - sweight) * (time / self->blendin);
	if (time < self->duration - self->blendout)
		return mweight;
	if (time < self->duration)
		return mweight + (eweight - mweight) * (1.0f - (self->duration - time) / self->blendout);
	return eweight;
#endif
	return 1.0f;
}

/*****************************************************************************/

static LIMatTransform*
private_frame_transform (LIMdlAnimation* self,
                         int             chan,
                         int             frame)
{
	return &self->buffer.array[self->channels.count * frame + chan].transform;
}

/** @} */
/** @} */
