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

#include <stdio.h>
#include "model-animation.h"

#define TIMESCALE 0.02f

static LIMatTransform*
private_frame_transform (LIMdlAnimation* self,
                         int             chan,
                         int             frame);

/*****************************************************************************/

/**
 * \brief Gets the index of a channel.
 *
 * \param self Animation.
 * \param name Channel name.
 * \return Channel index or -1.
 */
int
limdl_animation_get_channel (LIMdlAnimation* self,
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
float
limdl_animation_get_duration (const LIMdlAnimation* self)
{
	return (self->length - 1) * TIMESCALE;
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
int
limdl_animation_get_transform (LIMdlAnimation* self,
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
float
limdl_animation_get_weight (const LIMdlAnimation* self,
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
