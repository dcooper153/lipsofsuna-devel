/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlPoseChannel PoseChannel
 * @{
 */

#include "lipsofsuna/system.h"
#include "model-pose-channel.h"

/**
 * \brief Creates a new pose channel.
 * \param animation Animation.
 * \return New pose channel.
 */
LIMdlPoseChannel* limdl_pose_channel_new (
	LIMdlAnimation* animation)
{
	LIMdlAnimation* anim;
	LIMdlPoseChannel* self;

	/* Create an empty animation. */
	anim = limdl_animation_new_copy (animation);
	if (anim == NULL)
		return NULL;

	/* Create a new channel. */
	self = lisys_calloc (1, sizeof (LIMdlPoseChannel));
	if (self == NULL)
	{
		limdl_animation_free (anim);
		return NULL;
	}
	self->state = LIMDL_POSE_CHANNEL_STATE_PLAYING;
	self->animation = anim;
	self->priority_scale = 0;
	self->priority_transform = 0;
	self->weight_scale = 1.0f;
	self->weight_transform = 1.0f;
	self->time_scale = 1.0f;
	self->repeat_end = -1.0f;
	self->fade_in.mode = LIMDL_POSE_FADE_IN_AFTER_START;
	self->fade_out.mode = LIMDL_POSE_FADE_OUT_AFTER_END;

	return self;
}

/**
 * \brief Creates a deep copy of the pose channel.
 * \param channel Pose channel.
 * \return Pose channel.
 */
LIMdlPoseChannel* limdl_pose_channel_new_copy (
	LIMdlPoseChannel* channel)
{
	float* weight_src;
	float* weight_dst;
	LIAlgStrdicIter iter;
	LIMdlAnimation* anim;
	LIMdlPoseChannel* self;

	/* Create an empty animation. */
	anim = limdl_animation_new_copy (channel->animation);
	if (anim == NULL)
		return NULL;

	/* Create a new channel. */
	self = lisys_calloc (1, sizeof (LIMdlPoseChannel));
	if (self == NULL)
	{
		limdl_animation_free (anim);
		return NULL;
	}
	self->additive = channel->additive;
	self->state = channel->state;
	self->repeat = channel->repeat;
	self->repeats = channel->repeats;
	self->repeat_end = channel->repeat_end;
	self->repeat_start = channel->repeat_start;
	self->time = channel->time;
	self->time_scale = channel->time_scale;
	self->priority_scale = channel->priority_scale;
	self->priority_transform = channel->priority_transform;
	self->weight_scale = channel->weight_scale;
	self->weight_transform = channel->weight_transform;
	self->fade_in = channel->fade_in;
	self->fade_out = channel->fade_out;
	self->animation = anim;

	/* Copy node weights. */
	if (channel->weights != NULL)
	{
		self->weights = lialg_strdic_new ();
		if (self->weights == NULL)
		{
			limdl_pose_channel_free (self);
			return NULL;
		}
		LIALG_STRDIC_FOREACH (iter, channel->weights)
		{
			weight_src = iter.value;
			weight_dst = lisys_calloc (1, sizeof (float));
			if (weight_dst == NULL)
			{
				limdl_pose_channel_free (self);
				return NULL;
			}
			*weight_dst = *weight_src;
			if (!lialg_strdic_insert (self->weights, iter.key, weight_dst))
			{
				lisys_free (weight_dst);
				limdl_pose_channel_free (self);
				return NULL;
			}
		}
	}

	return self;
}

/**
 * \brief Frees the pose channel.
 * \param self Pose channel.
 */
void limdl_pose_channel_free (
	LIMdlPoseChannel* self)
{
	LIAlgStrdicIter iter;

	limdl_animation_free (self->animation);
	if (self->weights)
	{
		LIALG_STRDIC_FOREACH (iter, self->weights)
			lisys_free (iter.value);
		lialg_strdic_free (self->weights);
	}
	lisys_free (self);
}

/**
 * \brief Advances the playback of the pose channel.
 * \param self Pose channel.
 * \param secs Seconds since the last update.
 * \return One if the channel is active. Zero if playback has finished.
 */
int limdl_pose_channel_play (
	LIMdlPoseChannel* self,
	float             secs)
{
	int cycles;
	float duration;
	float start;

	/* Update fading. */
	if (self->fade_out.active)
	{
		self->time += secs;
		self->fade_timer += secs;
		if (self->fade_timer >= self->fade_out.duration)
			return 0;
		return 1;
	}

	/* Skip empty. */
	duration = limdl_animation_get_duration (self->animation);
	if (self->repeat_end >= 0.0f)
		duration = LIMAT_MIN (self->repeat_end, duration);
	if (duration < LIMAT_EPSILON)
	{
		self->time = 0.0f;
		if (self->repeats == -1)
			return 1;
		return 0;
	}

	/* Advance time. */
	self->time += self->time_scale * secs;

	/* Handle looping. */
	if (self->time > duration)
	{
		start = LIMAT_CLAMP (self->repeat_start, 0.0f, duration);
		if (start < duration)
		{
			cycles = (int) floor ((self->time - start) / (duration - start));
			self->time = self->time - (duration - start) * cycles;
			self->repeat += cycles;
		}
		else
		{
			self->time = duration;
			self->repeat++;
		}
	}

	/* Handle ending. */
	if (self->repeats != -1 && self->repeat >= self->repeats)
		return 0;

	return 1;
}

/**
 * \brief Gets the fading weight of the channel.
 * \param self Pose channel.
 * \return Fading weight.
 */
float limdl_pose_channel_get_fading (
	const LIMdlPoseChannel* self)
{
	float duration;
	float time;
	float end;
	float weight1;
	float weight2;

	/* Calculate the channel offset. */
	duration = limdl_animation_get_duration (self->animation);
	time = self->repeat * duration + self->time;
	end = self->repeats * duration;

	/* Handle fade in. */
	weight1 = 1.0f;
	switch (self->fade_in.mode)
	{
		case LIMDL_POSE_FADE_IN_AFTER_START:
			if (time < self->fade_in.duration)
				weight1 = time / self->fade_in.duration;
			break;
		case LIMDL_POSE_FADE_IN_BEFORE_START:
			if (self->fade_in.active)
				weight1 = self->fade_timer / self->fade_in.duration;
			break;
		case LIMDL_POSE_FADE_IN_INSTANT:
				weight1 = 1.0f;
			break;
	}

	/* Handle fade out. */
	weight2 = 1.0f;
	switch (self->fade_out.mode)
	{
		case LIMDL_POSE_FADE_OUT_AFTER_END:
		case LIMDL_POSE_FADE_OUT_AFTER_END_REPEAT:
			if (self->fade_out.active)
				weight2 = 1.0f - self->fade_timer / self->fade_out.duration;
			break;
		case LIMDL_POSE_FADE_OUT_BEFORE_END:
			if (time > end - self->fade_out.duration)
				weight2 = time - (end - self->fade_out.duration) / self->fade_out.duration;
			break;
		case LIMDL_POSE_FADE_OUT_INSTANT:
			if (self->fade_out.active)
				weight2 = 0.0f;
			break;
	}

	return LIMAT_MIN (weight1, weight2);
}

/**
 * \brief Gets the priority of the given node.
 * \param self Pose channel.
 * \param node Node name.
 * \param scale Return location for the scale priority.
 * \param transform Return location for the transform priority.
 */
void limdl_pose_channel_get_node_priority (
	LIMdlPoseChannel* self,
	const char*       node,
	int*              scale,
	int*              transform)
{
	int* value;

	if (self->priorities != NULL)
	{
		value = lialg_strdic_find (self->priorities, node);
		if (value != NULL)
		{
			*scale = *value;
			*transform = *value;
			return;
		}
	}
	*scale = self->priority_scale;
	*transform = self->priority_transform;
}

/**
 * \brief Sets the priority of the given node.
 * \param self Pose channel.
 * \param node Node name.
 * \param value Priority.
 * \return One on success. Zero otherwise.
 */
int limdl_pose_channel_set_node_priority (
	LIMdlPoseChannel* self,
	const char*       node,
	int               value)
{
	int* ptr;

	/* Make sure the node priority dictionary exists. */
	if (self->priorities == NULL)
	{
		self->priorities = lialg_strdic_new ();
		if (self->priorities == NULL)
			return 0;
	}

	/* Replace or add a node priority. */
	ptr = lialg_strdic_find (self->priorities, node);
	if (ptr == NULL)
	{
		ptr = lisys_calloc (1, sizeof (int));
		if (ptr == NULL)
			return 0;
		if (!lialg_strdic_insert (self->priorities, node, ptr))
		{
			lisys_free (ptr);
			return 0;
		}
	}
	*ptr = value;

	return 1;
}

int limdl_pose_channel_set_node_weight (
	LIMdlPoseChannel* self,
	const char*       node,
	float             value)
{
	float* ptr;

	/* Make sure the node weight dictionary exists. */
	if (self->weights == NULL)
	{
		self->weights = lialg_strdic_new ();
		if (self->weights == NULL)
			return 0;
	}

	/* Replace or add a node weight. */
	ptr = lialg_strdic_find (self->weights, node);
	if (ptr == NULL)
	{
		ptr = lisys_calloc (1, sizeof (float));
		if (ptr == NULL)
			return 0;
		if (!lialg_strdic_insert (self->weights, node, ptr))
		{
			lisys_free (ptr);
			return 0;
		}
	}
	*ptr = value;

	return 1;
}

void limdl_pose_channel_get_weight (
	const LIMdlPoseChannel* self,
	const char*             node,
	float*                  scale,
	float*                  transform)
{
	float weight_fading;
	float weight_scale;
	float weight_transform;
	float* weight_ptr;

	/* Calculate the base weights. */
	weight_scale = self->weight_scale;
	weight_transform = self->weight_transform;
	if (node != NULL && self->weights)
	{
		weight_ptr = lialg_strdic_find (self->weights, node);
		if (weight_ptr != NULL)
			weight_transform = *weight_ptr;
	}

	/* Calculate the fading weight. */
	weight_fading = limdl_pose_channel_get_fading (self);

	/* Calculate channel weight. */
	*scale = weight_scale * weight_fading;
	*transform = weight_transform * weight_fading;
}

/** @} */
/** @} */
