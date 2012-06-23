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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlPose Pose
 * @{
 */

#include "lipsofsuna/system.h"
#include "model-pose.h"

static void private_clear_pose (
	LIMdlPose* self);

static LIMdlPoseChannel* private_create_channel (
	LIMdlPose* self,
	int        channel);

static void private_fade_free (
	LIMdlPoseFade* fade);

static void private_fade_remove (
	LIMdlPose*     self,
	LIMdlPoseFade* fade);

static LIMdlPoseChannel* private_find_channel (
	const LIMdlPose* self,
	int              channel);

static float private_smooth_fade (
	float channel_weight,
	float fade_offset,
	float fade_length);

static LIMdlAnimation private_empty_anim =
{
	0, "empty", 0.0f, 0.0f, { 0, NULL }, { 0, NULL }
};

/*****************************************************************************/

/**
 * \brief Creates a new model pose.
 * \return New model pose or NULL.
 */
LIMdlPose* limdl_pose_new ()
{
	LIMdlPose* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlPose));
	if (self == NULL)
		return NULL;

	/* Allocate channel tree. */
	self->channels = lialg_u32dic_new ();
	if (self->channels == NULL)
	{
		limdl_pose_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a copy of the pose.
 *
 * The copied pose only inherits the channels. The model of the pose is set as
 * NULL so all animations are considered as missing. They can be reinstantiated
 * by assigning a model to the pose.
 *
 * \param pose Model pose.
 * \return Copied model pose.
 */
LIMdlPose* limdl_pose_new_copy (
	LIMdlPose* pose)
{
	LIAlgU32dicIter iter;
	LIMdlPose* self;
	LIMdlPoseChannel* chan;
	LIMdlPoseChannel* chan1;

	/* Allocate the copy. */
	self = limdl_pose_new ();
	if (self == NULL)
		return NULL;

	/* Copy channels. */
	LIALG_U32DIC_FOREACH (iter, pose->channels)
	{
		chan = iter.value;
		chan1 = limdl_pose_channel_new_copy (chan);
		if (chan1)
		{
			if (!lialg_u32dic_insert (self->channels, iter.key, chan1))
				limdl_pose_channel_free (chan1);
		}
	}

	/* TODO: Copy fades. */

	return self;
}

/**
 * \brief Frees the model pose.
 * \param self Model pose.
 */
void limdl_pose_free (
	LIMdlPose* self)
{
	private_clear_pose (self);
	if (self->channels != NULL)
		lialg_u32dic_free (self->channels);
	lisys_free (self);
}

/**
 * \brief Calculates the local transformation of a node.
 * \param self Pose.
 * \param node Node name.
 * \param result_transform Return location for the transformation.
 * \param result_scale Return location for the scale.
 */
void limdl_pose_calculate_node_tranformation (
	LIMdlPose*      self,
	const char*     node,
	LIMatTransform* result_transform,
	float*          result_scale)
{
	int channels;
	float scale;
	float scale1;
	float total_scale;
	float total_transform;
	float weight;
	float weight1;
	LIAlgU32dicIter iter;
	LIMatQuaternion bonerot;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector bonepos;
	LIMatVector position;
	LIMdlPoseFade* fade;
	LIMdlPoseChannel* chan;

	channels = 0;
	scale = 0.0f;
	total_scale = 0.0f;
	total_transform = 0.0f;
	position = limat_vector_init (0.0f, 0.0f, 0.0f);
	rotation = limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f);

	/* Sum channel weights. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		if (chan->additive)
			continue;
		if (limdl_animation_get_channel (chan->animation, node) != -1)
		{
			limdl_pose_channel_get_weight (chan, node, &weight, &weight1);
			total_scale += weight;
			total_transform += weight1;
			channels++;
		}
	}

	/* Sum fade weights. */
	for (fade = self->fades ; fade != NULL ; fade = fade->next)
	{
		if (limdl_animation_get_channel (fade->animation, node) != -1)
		{
			total_transform += fade->current_weight_transform;
			total_scale += fade->current_weight_scale;
			channels++;
		}
	}

	/* Apply valid transformation influences. */
	if (channels && total_transform >= LIMAT_EPSILON)
	{
		/* Apply channel influences. */
		LIALG_U32DIC_FOREACH (iter, self->channels)
		{
			chan = iter.value;
			if (chan->additive)
				continue;
			if (limdl_animation_get_transform (chan->animation, node, chan->time, &scale1, &transform))
			{
				bonepos = transform.position;
				bonerot = transform.rotation;
				limdl_pose_channel_get_weight (chan, node, &weight1, &weight);
				rotation = limat_quaternion_nlerp (bonerot, rotation, weight / total_transform);
				position = limat_vector_lerp (bonepos, position, weight / total_transform);
			}
		}

		/* Apply fade influences. */
		for (fade = self->fades ; fade != NULL ; fade = fade->next)
		{
			if (limdl_animation_get_transform (fade->animation, node, fade->time, &scale1, &transform))
			{
				bonepos = transform.position;
				bonerot = transform.rotation;
				weight = fade->current_weight_transform;
				rotation = limat_quaternion_nlerp (bonerot, rotation, weight / total_transform);
				position = limat_vector_lerp (bonepos, position, weight / total_transform);
			}
		}
	}

	/* Apply valid scale influences. */
	if (channels && total_scale >= LIMAT_EPSILON)
	{
		/* Apply channel influences. */
		LIALG_U32DIC_FOREACH (iter, self->channels)
		{
			chan = iter.value;
			if (limdl_animation_get_transform (chan->animation, node, chan->time, &scale1, &transform))
			{
				limdl_pose_channel_get_weight (chan, node, &weight, &weight1);
				scale += scale1 * weight / total_scale;
			}
		}

		/* Apply fade influences. */
		for (fade = self->fades ; fade != NULL ; fade = fade->next)
		{
			if (limdl_animation_get_transform (fade->animation, node, fade->time, &scale1, &transform))
			{
				weight = fade->current_weight_scale;
				scale += scale1 * weight / total_scale;
			}
		}
	}
	else
		scale = 1.0f;

	/* Apply additive transformations and scaling. */
	/* Additive channels aren't normalized against the total weight but applied as
	   is on top of other transformations. If the weight of an additive channel is
	   1, the blended transformation of other channels is multiplied by its full
	   rotation and scaling. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		if (!chan->additive)
			continue;
		if (limdl_animation_get_transform (chan->animation, node, chan->time, &scale1, &transform))
		{
			bonepos = transform.position;
			bonerot = transform.rotation;
			limdl_pose_channel_get_weight (chan, node, &weight1, &weight);
			rotation = limat_quaternion_nlerp (bonerot, rotation, weight);
			position = limat_vector_lerp (bonepos, position, weight);
			scale += scale1 * weight1;
		}
	}

	/* Return the transformation. */
	*result_transform = limat_transform_init (position, rotation);
	*result_scale = scale;
}

void limdl_pose_clear_channel_node_priorities (
	LIMdlPose*  self,
	int         channel)
{
	LIAlgStrdicIter iter;
	LIMdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL || chan->weights == NULL)
		return;
	LIALG_STRDIC_FOREACH (iter, chan->weights)
		lisys_free (iter.value);
	lialg_strdic_free (chan->weights);
	chan->weights = NULL;
}

/**
 * \brief Destroys an animation channel.
 *
 * Channels are destroyed automatically with the pose but it's also possible to
 * destroy a channel explicitly with this function.
 *
 * \param self Model pose.
 * \param channel Channel number.
 */
void limdl_pose_destroy_channel (
	LIMdlPose* self,
	int        channel)
{
	LIMdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL)
		return;
	lialg_u32dic_remove (self->channels, channel);
	limdl_pose_channel_free (chan);
}

/**
 * \brief Fades out the channel.
 *
 * If an animation is playing or paused on the channel, a new fade sequence
 * is created and played seamlessly in the background. The channel cleared
 * and immediately ready to accept a new animation.
 *
 * If the channel is empty, no fading takes place, so it is safe to call this
 * function every time you set a new animation and want it to blend.
 *
 * \param self Model pose.
 * \param channel Channel number.
 * \param secs Fade out time in seconds or LIMDL_POSE_FADE_AUTOMATIC.
 */
void limdl_pose_fade_channel (
	LIMdlPose* self,
	int        channel,
	float      secs)
{
	LIMdlPoseFade* fade;
	LIMdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL)
		return;

	/* Handle auto rate. */
	if (secs == LIMDL_POSE_FADE_AUTOMATIC)
	{
		if (chan->fade_out < LIMAT_EPSILON)
			return;
		secs = chan->fade_out;
	}

	/* Create a fade sequence. */
	fade = lisys_calloc (1, sizeof (LIMdlPoseFade));
	if (fade == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		return;
	}
	limdl_pose_channel_get_weight (chan, NULL, &fade->priority_scale, &fade->priority_transform);
	fade->fade_out = secs;
	fade->time = chan->time;
	fade->time_fade = 0.0f;
	fade->current_weight_transform = fade->priority_transform;
	fade->current_weight_scale = fade->priority_scale;
	fade->animation = limdl_animation_new_copy (chan->animation);
	if (fade->animation == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		lisys_free (fade);
		return;
	}

	/* Link to fade list. */
	fade->prev = NULL;
	fade->next = self->fades;
	if (self->fades != NULL)
		self->fades->prev = fade;
	self->fades = fade;

	/* Destroy the channel. */
	limdl_pose_destroy_channel (self, channel);
}

/**
 * \brief Merges or adds a pose channel.
 * \param self Model pose.
 * \param channel Channel number, or -1 for automatic.
 * \param keep Nonzero to keep appropriate data from old animations, zero to overwrite all data.
 * \param info Pose channel data.
 */
void limdl_pose_merge_channel (
	LIMdlPose*              self,
	int                     channel,
	int                     keep,
	const LIMdlPoseChannel* info)
{
	const char* name1;
	LIAlgStrdicIter iter;

	lisys_assert (info != NULL);
	lisys_assert (info->animation != NULL);

	/* Avoid restarts in simple cases. */
	/* The position is kept if the animation is repeating and being replaced with
	   the same one but parameters such as fading and weights still need to be reset. */
	if (info->repeats > 0 && channel != -1)
	{
		if (keep)
		{
			keep = 0;
			if (limdl_pose_get_channel_state (self, channel) == LIMDL_POSE_CHANNEL_STATE_PLAYING &&
				limdl_pose_get_channel_repeats (self, channel) == -1)
			{
				name1 = limdl_pose_get_channel_name (self, channel);
				if (!strcmp (info->animation->name, name1))
					keep = 1;
			}
		}
	}
	else
		keep = 0;

	/* Automatic channel assignment. */
	if (channel == -1)
	{
		for (channel = 254 ; channel > 0 ; channel--)
		{
			if (limdl_pose_get_channel_state (self, channel) == LIMDL_POSE_CHANNEL_STATE_INVALID)
				break;
		}
	}

	/* Update or initialize the channel. */
	if (!keep)
	{
		limdl_pose_fade_channel (self, channel, LIMDL_POSE_FADE_AUTOMATIC);
		limdl_pose_set_channel_animation (self, channel, info->animation);
		limdl_pose_set_channel_repeats (self, channel, info->repeats);
		limdl_pose_set_channel_position (self, channel, info->time);
		limdl_pose_set_channel_state (self, channel, LIMDL_POSE_CHANNEL_STATE_PLAYING);
	}
	limdl_pose_set_channel_additive (self, channel, info->additive);
	limdl_pose_set_channel_repeat_end (self, channel, info->repeat_end);
	limdl_pose_set_channel_repeat_start (self, channel, info->repeat_start);
	limdl_pose_set_channel_priority_scale (self, channel, info->priority_scale);
	limdl_pose_set_channel_priority_transform (self, channel, info->priority_transform);
	limdl_pose_set_channel_time_scale (self, channel, info->time_scale);
	limdl_pose_set_channel_fade_in (self, channel, info->fade_in);
	limdl_pose_set_channel_fade_out (self, channel, info->fade_out);

	/* Handle optional per-node weights. */
	if (info->weights != NULL)
	{
		limdl_pose_clear_channel_node_priorities (self, channel);
		LIALG_STRDIC_FOREACH (iter, info->weights)
		{
			limdl_pose_set_channel_priority_node (self, channel,
				iter.key, *((float*) iter.value));
		}
	}
}

/**
 * \brief Progresses the animation.
 *
 * \param self Model pose.
 * \param secs Number of seconds to progress.
 */
void limdl_pose_update (
	LIMdlPose* self,
	float      secs)
{
	LIAlgU32dicIter iter;
	LIMdlPoseFade* fade;
	LIMdlPoseFade* fade_next;
	LIMdlPoseChannel* chan;

	/* Update channels. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		switch (chan->state)
		{
			case LIMDL_POSE_CHANNEL_STATE_PAUSED:
				break;
			case LIMDL_POSE_CHANNEL_STATE_PLAYING:
				if (!limdl_pose_channel_play (chan, secs))
				{
					lialg_u32dic_remove (self->channels, iter.key);
					limdl_pose_channel_free (chan);
				}
				break;
		}
	}

	/* Update fade sequences. */
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		/* Calculate smooth fading. */
		fade->current_weight_scale = private_smooth_fade (
			fade->priority_scale, fade->time_fade, fade->fade_out);
		fade->current_weight_transform = private_smooth_fade (
			fade->priority_transform, fade->time_fade, fade->fade_out);

		/* Update time and weights. */
		fade_next = fade->next;
		fade->time += secs;
		fade->time_fade += secs;

		/* Remove the fade when its influence reaches zero. */
		if (fade->time_fade >= fade->fade_out)
		{
			private_fade_remove (self, fade);
			private_fade_free (fade);
		}
	}
}

int limdl_pose_get_channel_additive (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->additive;
}

void limdl_pose_set_channel_additive (
	LIMdlPose* self,
	int        channel,
	int        value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->additive = value;
}

LIMdlAnimation* limdl_pose_get_channel_animation (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return NULL;
	return chan->animation;
}

void limdl_pose_set_channel_animation (
	LIMdlPose*      self,
	int             channel,
	LIMdlAnimation* animation)
{
	LIMdlAnimation* anim;
	LIMdlPoseChannel* chan;

	/* Copy the animation. */
	lisys_assert (animation != NULL);
	anim = limdl_animation_new_copy (animation);
	if (anim == NULL)
		return;

	/* Create the channel. */
	chan = private_create_channel (self, channel);
	if (chan == NULL)
	{
		limdl_animation_free (anim);
		return;
	}
	chan->time = 0.0f;
	chan->fade_in = 0.0f;
	chan->fade_out = 0.0f;
	limdl_animation_free (chan->animation);
	chan->animation = anim;
}

/**
 * \brief Gets the repeat range end offset, in seconds.
 * \param self Model pose.
 * \param channel Channel number.
 * \return Repeat range end, in seconds.
 */
float limdl_pose_get_channel_repeat_end (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->repeat_end;
}

/**
 * \brief Sets the repeat range end offset, in seconds.
 * \param self Model pose.
 * \param channel Channel number.
 * \param value Repeat range end, in seconds.
 */
void limdl_pose_set_channel_repeat_end (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->repeat_end = value;
}

/**
 * \brief Gets the repeat range start offset, in seconds.
 * \param self Model pose.
 * \param channel Channel number.
 * \return Repeat range start, in seconds.
 */
float limdl_pose_get_channel_repeat_start (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->repeat_start;
}

/**
 * \brief Sets the repeat range start offset, in seconds.
 * \param self Model pose.
 * \param channel Channel number.
 * \param value Repeat range start, in seconds.
 */
void limdl_pose_set_channel_repeat_start (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->repeat_start = value;
}

float limdl_pose_get_channel_fade_in (
	LIMdlPose* self,
	int        channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->fade_in;
}

void limdl_pose_set_channel_fade_in (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan != NULL)
		chan->fade_in = value;
}

float limdl_pose_get_channel_fade_out (
	LIMdlPose* self,
	int        channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->fade_out;
}

void limdl_pose_set_channel_fade_out (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan != NULL)
		chan->fade_out = value;
}

const char* limdl_pose_get_channel_name (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return "";
	return chan->animation->name;
}

float limdl_pose_get_channel_position (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->time;
}

void limdl_pose_set_channel_position (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->time = value;
	if (chan->time > limdl_animation_get_duration (chan->animation))
		chan->time = limdl_animation_get_duration (chan->animation);
}

float* limdl_pose_get_channel_priority_node (
	const LIMdlPose* self,
	int              channel,
	const char*      node)
{
	float* ptr;
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL || chan->weights == NULL)
		return NULL;
	ptr = lialg_strdic_find (chan->weights, node);

	return ptr;
}

int limdl_pose_set_channel_priority_node (
	LIMdlPose*  self,
	int         channel,
	const char* node,
	float       value)
{
	LIMdlPoseChannel* chan;

	/* Find the channel. */
	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;

	/* Replace or add a node weight. */
	return limdl_pose_channel_set_node_priority (chan, node, value);
}

LIAlgStrdic* limdl_pose_get_channel_priority_nodes (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return NULL;

	return chan->weights;
}

float limdl_pose_get_channel_priority_scale (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->priority_scale;
}

void limdl_pose_set_channel_priority_scale (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->priority_scale = value;
}

float limdl_pose_get_channel_priority_transform (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->priority_transform;
}

void limdl_pose_set_channel_priority_transform (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->priority_transform = value;
}

int limdl_pose_get_channel_repeats (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->repeats;
}

void limdl_pose_set_channel_repeats (
	LIMdlPose* self,
	int        channel,
	int        value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->repeats = value;
}

/**
 * \brief Gets the state of a channel.
 * \param self Model pose.
 * \param channel Channel number.
 * \return Current state.
 */
LIMdlPoseChannelState limdl_pose_get_channel_state (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return LIMDL_POSE_CHANNEL_STATE_INVALID;
	return chan->state;
}

/**
 * \brief Sets the state of a channel.
 *
 * \param self Model pose.
 * \param channel Channel number.
 * \param value New state.
 */
void limdl_pose_set_channel_state (
	LIMdlPose*            self,
	int                   channel,
	LIMdlPoseChannelState value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL || chan->state == value)
		return;
	switch (value)
	{
		case LIMDL_POSE_CHANNEL_STATE_PLAYING:
			chan->state = value;
			break;
		case LIMDL_POSE_CHANNEL_STATE_PAUSED:
			chan->state = value;
			break;
		default:
			lisys_assert (0);
			break;
	}
}

float limdl_pose_get_channel_time_scale (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->time_scale;
}

void limdl_pose_set_channel_time_scale (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->time_scale = value;
}

/*****************************************************************************/

static void private_clear_pose (
	LIMdlPose* self)
{
	LIAlgU32dicIter iter;
	LIMdlPoseFade* fade;
	LIMdlPoseFade* fade_next;

	/* Free fades. */
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		fade_next = fade->next;
		private_fade_free (fade);
	}
	self->fades = NULL;

	/* Clear the channel tree. */
	if (self->channels != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->channels)
			limdl_pose_channel_free (iter.value);
		lialg_u32dic_clear (self->channels);
	}
}

static LIMdlPoseChannel* private_create_channel (
	LIMdlPose* self,
	int        channel)
{
	LIMdlAnimation* anim;
	LIMdlPoseChannel* chan;

	/* Check for an existing channel. */
	chan = private_find_channel (self, channel);
	if (chan != NULL)
		return chan;

	/* Create an empty animation. */
	anim = limdl_animation_new_copy (&private_empty_anim);
	if (anim == NULL)
		return 0;

	/* Create a new channel. */
	chan = lisys_calloc (1, sizeof (LIMdlPoseChannel));
	if (chan == NULL)
	{
		limdl_animation_free (anim);
		return 0;
	}
	chan->state = LIMDL_POSE_CHANNEL_STATE_PLAYING;
	chan->animation = anim;
	chan->priority_scale = 0.0f;
	chan->priority_transform = 1.0f;
	chan->time_scale = 1.0f;

	/* Register the channel. */
	if (!lialg_u32dic_insert (self->channels, channel, chan))
	{
		limdl_animation_free (chan->animation);
		lisys_free (chan);
		return 0;
	}

	return chan;
}

static void private_fade_free (
	LIMdlPoseFade* fade)
{
	limdl_animation_free (fade->animation);
	lisys_free (fade);
}

static void private_fade_remove (
	LIMdlPose*     self,
	LIMdlPoseFade* fade)
{
	if (fade->next != NULL)
		fade->next->prev = fade->prev;
	if (fade->prev != NULL)
		fade->prev->next = fade->next;
	else
		self->fades = fade->next;
}

static LIMdlPoseChannel* private_find_channel (
	const LIMdlPose* self,
	int              channel)
{
	return lialg_u32dic_find (self->channels, channel);
}

static float private_smooth_fade (
	float channel_weight,
	float fade_offset,
	float fade_length)
{
	float weight_base;
	float weight_scaled;
	float weight_smoothing;

	/* Calculates smooth fading. */
	/* Linear fading of channels with wildly different weights doesn't
	   look nice because the channel with the largest weight dominates.
	   This is particularly problematic with cross-fading since there's
	   no cross-fading at all without compensating for the difference. */
	/* We reduce the problem by applying smoothstep() to the excess weight.
	   This compensates for the weight difference by smoothly bringing the
	   weight of the sequence closer to the [0,1] range. */
	weight_base = 1.0f - (fade_offset / fade_length);
	weight_scaled = weight_base * channel_weight;
	weight_smoothing = 1.0f - limat_smoothstep (fade_offset, 0.0f, fade_length);
	return weight_base + (weight_scaled - weight_base) * weight_smoothing;
}

/** @} */
/** @} */
