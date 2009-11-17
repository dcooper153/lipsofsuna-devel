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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlPose Pose
 * @{
 */

#include <system/lips-system.h>
#include "model-pose.h"

static void
private_clear_node (limdlPose*       self,
                    limdlNode*       node,
                    const limdlNode* rest);

static void
private_clear_pose (limdlPose* self);

static limdlPoseChannel*
private_create_channel (limdlPose* self,
                        int        channel);

static limdlPoseChannel*
private_find_channel (const limdlPose* self,
                      int              channel);

static int
private_init_pose (limdlPose*  self,
                   limdlModel* model);

static int
private_play_channel (const limdlPose*  self,
                      limdlPoseChannel* channel,
                      float             secs);

static void
private_transform_node (limdlPose* self,
                        limdlNode* node);

static float
private_get_channel_weight (const limdlPose*        self,
                            const limdlPoseChannel* channel);

static limdlAnimation
private_empty_anim =
{
	0, "empty", 0.0f, 0.0f, { 0, NULL }, { 0, NULL }
};

/*****************************************************************************/

/**
 * \brief Creates a new model pose.
 *
 * \param model The model this pose is for.
 * \return New model pose or NULL.
 */
limdlPose*
limdl_pose_new (limdlModel* model)
{
	limdlPose* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (limdlPose));
	if (self == NULL)
		return NULL;

	/* Allocate channel tree. */
	self->channels = lialg_u32dic_new ();
	if (self->channels == NULL)
		goto error;

	return self;

error:
	limdl_pose_free (self);
	return NULL;
}

/**
 * \brief Frees the model pose.
 *
 * \param self A model pose.
 */
void
limdl_pose_free (limdlPose* self)
{
	int i;
	lialgU32dicIter iter;
	limdlPoseFade* fade;
	limdlPoseFade* fade_next;
	limdlPoseChannel* chan;

	/* Free fades. */
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		fade_next = fade->next;
		lisys_free (fade);
	}

	/* Free channel tree. */
	if (self->channels != NULL)
	{
		LI_FOREACH_U32DIC (iter, self->channels)
		{
			chan = iter.value;
			lisys_free (chan);
		}
		lialg_u32dic_free (self->channels);
	}

	/* Free nodes. */
	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		lisys_free (self->nodes.array);
	}

	/* Free weight groups. */
	if (self->groups.array != NULL)
	{
		for (i = 0 ; i < self->groups.count ; i++)
		{
			lisys_free (self->groups.array[i].name);
			lisys_free (self->groups.array[i].bone);
		}
		lisys_free (self->groups.array);
	}

	lisys_free (self);
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
void
limdl_pose_destroy_channel (limdlPose* self,
                            int        channel)
{
	limdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL)
		return;
	lialg_u32dic_remove (self->channels, channel);
	lisys_free (chan);
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
 * \param rate Amount of fade out per second or LIMDL_POSE_FADE_AUTOMATIC.
 */
void
limdl_pose_fade_channel (limdlPose* self,
                         int        channel,
                         float      rate)
{
	limdlPoseFade* fade;
	limdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL)
		return;

	/* Handle auto rate. */
	if (rate == LIMDL_POSE_FADE_AUTOMATIC)
		rate = 1.0f / chan->animation->blendout;

	/* Create a fade sequence. */
	fade = lisys_calloc (1, sizeof (limdlPoseFade));
	if (fade == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		return;
	}
	fade->rate = rate;
	fade->time = chan->time;
	fade->weight = chan->priority * private_get_channel_weight (self, chan);
	fade->animation = chan->animation;

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
 * \brief Finds a node by name.
 *
 * \param self Model pose.
 * \param name Name of the node to find.
 * \return Node or NULL.
 */
limdlNode*
limdl_pose_find_node (const limdlPose* self,
                      const char*      name)
{
	int i;
	limdlNode* node;

	if (self->model == NULL)
		return NULL;

	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		node = limdl_node_find_node (node, name);
		if (node != NULL)
			return node;
	}

	return NULL;
}

/**
 * \brief Progresses the animation.
 *
 * \param self Model pose.
 * \param secs Number of seconds to progress.
 */
void
limdl_pose_update (limdlPose* self,
                   float      secs)
{
	int i;
	lialgU32dicIter iter;
	limdlPoseFade* fade;
	limdlPoseFade* fade_next;
	limdlPoseChannel* chan;
	limdlNode* node0;
	limdlNode* node1;

	if (self->model == NULL)
		return;

	/* Update channels. */
	LI_FOREACH_U32DIC (iter, self->channels)
	{
		chan = iter.value;
		switch (chan->state)
		{
			case LIMDL_POSE_CHANNEL_STATE_PAUSED:
				break;
			case LIMDL_POSE_CHANNEL_STATE_PLAYING:
				if (!private_play_channel (self, chan, secs))
					lialg_u32dic_remove (self->channels, iter.key);
				break;
		}
	}

	/* Update fade sequences. */
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		fade_next = fade->next;
		fade->time += secs;
		fade->weight -= secs * fade->rate;
		if (fade->weight <= 0.0f)
		{
			if (fade->next != NULL)
				fade->next->prev = fade->prev;
			if (fade->prev != NULL)
				fade->prev->next = fade->next;
			else
				self->fades = fade->next;
			lisys_free (fade);
		}
	}

	/* Clear each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node0 = self->nodes.array[i];
		node1 = self->model->nodes.array[i];
		private_clear_node (self, node0, node1);
	}

	/* Transform each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node0 = self->nodes.array[i];
		private_transform_node (self, node0);
	}
}

void
limdl_pose_transform_group (limdlPose*   self,
                            int          group,
                            limdlVertex* vertices)
{
	int i;
	int j;
	int count;
	limatQuaternion quat0;
	limatQuaternion quat1;
	limatVector tmp;
	limatVector rest_vertex;
	limatVector pose_vertex;
	limatVector rest_normal;
	limatVector pose_normal;
	limdlFaces* group_;
	limdlNode* restbone;
	limdlNode* posebone;
	limdlWeight* weight;
	limdlWeights* weights;
	limdlModel* model = self->model;

	if (model == NULL)
		return;

	assert (group >= 0);
	assert (group < model->facegroups.count);
	group_ = model->facegroups.array + group;

	/* Transform each vertex. */
	for (i = 0 ; i < group_->vertices.count ; i++)
	{
		count = 0;
		weights = group_->weights.array + i;

		/* Get the rest pose state. */
		rest_vertex = group_->vertices.array[i].coord;
		rest_normal = group_->vertices.array[i].normal;
		rest_normal = limat_vector_add (rest_normal, rest_vertex);
		pose_vertex = limat_vector_init (0.0f, 0.0f, 0.0f);
		pose_normal = limat_vector_init (0.0f, 0.0f, 0.0f);

		/* Transform by each weight group. */
		for (j = 0 ; j < weights->count ; j++)
		{
			/* Get transformation weight. */
			weight = weights->weights + j;
			if (weight->weight == 0.0f)
				continue;

			/* Get transformed bone. */
			restbone = model->weightgroups.weightgroups[weight->group].node;
			posebone = self->groups.array[weight->group].node;
			if (restbone == NULL || posebone == NULL)
				continue;
			count++;

#warning Possible to use transforms here?
			/* Get the rotations. */
			quat0 = restbone->transform.global.rotation;
			quat1 = posebone->transform.global.rotation;
			quat0 = limat_quaternion_conjugate (quat0);

			/* Transform the vertex. */
			tmp = limat_vector_subtract (rest_vertex, restbone->transform.global.position);
			tmp = limat_quaternion_transform (quat0, tmp);
			tmp = limat_quaternion_transform (quat1, tmp);
			tmp = limat_vector_add (tmp, posebone->transform.global.position);
			pose_vertex = limat_vector_add (pose_vertex,
				limat_vector_multiply (tmp, weight->weight));

			/* Transform the normal. */
			tmp = limat_vector_subtract (rest_normal, restbone->transform.global.position);
			tmp = limat_quaternion_transform (quat0, tmp);
			tmp = limat_quaternion_transform (quat1, tmp);
			tmp = limat_vector_add (tmp, posebone->transform.global.position);
			pose_normal = limat_vector_add (pose_normal,
				limat_vector_multiply (tmp, weight->weight));
		}

		/* Default to the rest pose. */
		if (!count)
		{
			pose_vertex = rest_vertex;
			pose_normal = rest_normal;
		}

		/* Set the transformed state. */
		pose_normal = limat_vector_subtract (pose_normal, pose_vertex);
		pose_normal = limat_vector_normalize (pose_normal);
		vertices[i].coord = pose_vertex;
		vertices[i].normal = pose_normal;
	}
}

limdlAnimation*
limdl_pose_get_channel_animation (const limdlPose* self,
                                  int              channel)
{
	limdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return NULL;
	return chan->animation;
}

void
limdl_pose_set_channel_animation (limdlPose*  self,
                                  int         channel,
                                  const char* animation)
{
	limdlAnimation* anim;
	limdlPoseChannel* chan;

	if (self->model == NULL)
		return;
	anim = limdl_model_get_animation (self->model, animation);
	if (anim == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		return;
	}
	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->time = 0.0f;
	chan->animation = anim;
}

const char*
limdl_pose_get_channel_name (const limdlPose* self,
                             int              channel)
{
	limdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL || chan->animation == NULL)
		return "";
	return chan->animation->name;
}

float
limdl_pose_get_channel_position (const limdlPose* self,
                                 int              channel)
{
	limdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->time;
}

float
limdl_pose_get_channel_priority (const limdlPose* self,
                                 int              channel)
{
	limdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->priority;
}

void
limdl_pose_set_channel_priority (limdlPose* self,
                                 int        channel,
                                 float      value)
{
	limdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->priority = value;
}

int
limdl_pose_get_channel_repeats (const limdlPose* self,
                                int              channel)
{
	limdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;
	return chan->repeats;
}

void
limdl_pose_set_channel_repeats (limdlPose* self,
                                int        channel,
                                int        value)
{
	limdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->repeats = value;
}

/**
 * \brief Gets the state of a channel.
 *
 * \param self Model pose.
 * \param channel Channel number.
 * \return Current state.
 */
limdlPoseChannelState
limdl_pose_get_channel_state (const limdlPose* self,
                              int              channel)
{
	limdlPoseChannel* chan;

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
void
limdl_pose_set_channel_state (limdlPose*            self,
                              int                   channel,
                              limdlPoseChannelState value)
{
	limdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL || chan->state == value)
		return;
	switch (value)
	{
		case LIMDL_POSE_CHANNEL_STATE_PLAYING:
			chan->time = 0.0f;
			chan->state = value;
			break;
		case LIMDL_POSE_CHANNEL_STATE_PAUSED:
			chan->state = value;
			break;
		default:
			assert (0);
			break;
	}
}

/**
 * \brief Sets the posed model.
 *
 * \param self Model pose.
 * \param model Model to pose.
 * \return Nonzero on success.
 */
int
limdl_pose_set_model (limdlPose*  self,
                      limdlModel* model)
{
	limdlPose backup;

	/* Backup old data. */
	memcpy (&backup, self, sizeof (limdlPose));
	self->channels = NULL;
	self->fades = NULL;
	self->model = NULL;
	self->groups.count = 0;
	self->groups.array = NULL;
	self->nodes.count = 0;
	self->nodes.array = NULL;

	/* Create new pose data. */
	if (model != NULL)
	{
		if (!private_init_pose (self, model))
		{
			private_clear_pose (self);
			memcpy (self, &backup, sizeof (limdlPose));
			return 0;
		}
	}

	/* Replace old data. */
	/* This also clears the incompatible channels and fade sequences. */
	self->channels = backup.channels;
	private_clear_pose (&backup);

	return 1;
}

/*****************************************************************************/

static void
private_clear_node (limdlPose*       self,
                    limdlNode*       node,
                    const limdlNode* rest)
{
	int i;
	limdlNode* node0;
	limdlNode* node1;

	node->transform.global = rest->transform.global;
	for (i = 0 ; i < node->nodes.count ; i++)
	{
		node0 = node->nodes.array[i];
		node1 = rest->nodes.array[i];
		private_clear_node (self, node0, node1);
	}
}

static void
private_clear_pose (limdlPose* self)
{
	int i;
	lialgU32dicIter iter;
	limdlPoseFade* fade;
	limdlPoseFade* fade_next;
	limdlPoseChannel* chan;

	/* Free fades. */
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		fade_next = fade->next;
		lisys_free (fade);
	}
	self->fades = NULL;

	/* Clear channel tree. */
	if (self->channels != NULL)
	{
		LI_FOREACH_U32DIC (iter, self->channels)
		{
			chan = iter.value;
			lisys_free (chan);
		}
		lialg_u32dic_clear (self->channels);
	}

	/* Free nodes. */
	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		lisys_free (self->nodes.array);
	}

	/* Free weight groups. */
	if (self->groups.array != NULL)
	{
		for (i = 0 ; i < self->groups.count ; i++)
		{
			lisys_free (self->groups.array[i].name);
			lisys_free (self->groups.array[i].bone);
		}
		lisys_free (self->groups.array);
	}
}

static limdlPoseChannel*
private_create_channel (limdlPose* self,
                        int        channel)
{
	limdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan != NULL)
		return chan;
	chan = lisys_calloc (1, sizeof (limdlPoseChannel));
	if (chan == NULL)
		return 0;
	if (!lialg_u32dic_insert (self->channels, channel, chan))
		return 0;
	chan->state = LIMDL_POSE_CHANNEL_STATE_PLAYING;
	chan->animation = &private_empty_anim;

	return chan;
}

static limdlPoseChannel*
private_find_channel (const limdlPose* self,
                      int              channel)
{
	return lialg_u32dic_find (self->channels, channel);
}

static int
private_init_pose (limdlPose*  self,
                   limdlModel* model)
{
	int i;

	/* Set model. */
	self->model = model;
	self->groups.count = model->weightgroups.count;
	self->nodes.count = model->nodes.count;

	/* Copy nodes. */
	if (self->nodes.count)
	{
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (limdlNode*));
		if (self->nodes.array == NULL)
			return 0;
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_copy (model->nodes.array[i]);
			if (self->nodes.array[i] == NULL)
				return 0;
		}
	}

	/* Copy weight groups. */
	if (self->groups.count)
	{
		self->groups.array = lisys_calloc (self->groups.count, sizeof (limdlWeightGroup));
		if (self->groups.array == NULL)
			return 0;
		for (i = 0 ; i < self->groups.count ; i++)
		{
			self->groups.array[i].name = listr_dup (model->weightgroups.weightgroups[i].name);
			self->groups.array[i].bone = listr_dup (model->weightgroups.weightgroups[i].bone);
			self->groups.array[i].node = limdl_pose_find_node (self, self->groups.array[i].bone);
			if (self->groups.array[i].name == NULL ||
				self->groups.array[i].bone == NULL)
				return 0;
		}
	}

	return 1;
}

static int
private_play_channel (const limdlPose*  self,
                      limdlPoseChannel* channel,
                      float             secs)
{
	int cycles;
	float duration;

	/* Skip empty. */
	duration = limdl_animation_get_duration (channel->animation);
	if (duration < LI_MATH_EPSILON)
	{
		channel->time = 0.0f;
		if (channel->repeats == -1)
			return 1;
		return 0;
	}

	/* Advance time. */
	channel->time += secs;
	cycles = (int) floor (channel->time / duration);
	channel->time = channel->time - duration * cycles;

	/* Handle looping. */
	if (channel->repeats != -1)
	{
		channel->repeat += cycles;
		if (channel->repeat >= channel->repeats)
			return 0;
	}

	return 1;
}

static void
private_transform_node (limdlPose* self,
                        limdlNode* node)
{
	int i;
	int channels;
	float total;
	float weight;
	lialgU32dicIter iter;
	limatQuaternion bonerot;
	limatQuaternion rotation;
	limatTransform transform;
	limatVector bonepos;
	limatVector position;
	limdlPoseFade* fade;
	limdlPoseChannel* chan;

	channels = 0;
	total = 0.0f;
	position = limat_vector_init (0.0f, 0.0f, 0.0f);
	rotation = limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f);

	/* Sum channel weights. */
	LI_FOREACH_U32DIC (iter, self->channels)
	{
		chan = iter.value;
		if (limdl_animation_get_channel (chan->animation, node->name) != -1)
		{
			weight = private_get_channel_weight (self, chan);
			total += chan->priority * weight;
			channels++;
		}
	}

	/* Sum fade weights. */
	for (fade = self->fades ; fade != NULL ; fade = fade->next)
	{
		if (limdl_animation_get_channel (fade->animation, node->name) != -1)
		{
			total += fade->weight;
			channels++;
		}
	}

	/* Apply valid influences. */
	if (channels && total >= LI_MATH_EPSILON)
	{
		/* Apply channel influences. */
		LI_FOREACH_U32DIC (iter, self->channels)
		{
			chan = iter.value;
			if (limdl_animation_get_transform (chan->animation, node->name, chan->time, &transform))
			{
				bonepos = transform.position;
				bonerot = transform.rotation;
				weight = chan->priority * private_get_channel_weight (self, chan);
				rotation = limat_quaternion_nlerp (bonerot, rotation, weight / total);
				position = limat_vector_lerp (bonepos, position, weight / total);
			}
		}

		/* Apply fade influences. */
		for (fade = self->fades ; fade != NULL ; fade = fade->next)
		{
			if (limdl_animation_get_transform (fade->animation, node->name, chan->time, &transform))
			{
				bonepos = transform.position;
				bonerot = transform.rotation;
				weight = fade->weight;
				rotation = limat_quaternion_nlerp (bonerot, rotation, weight / total);
				position = limat_vector_lerp (bonepos, position, weight / total);
			}
		}
	}

	/* Update node transformation. */
	transform = limat_transform_init (position, rotation);
	limdl_node_set_local_transform (node, &transform);
	limdl_node_rebuild (node, 0);

	/* Update child transformations recursively. */
	for (i = 0 ; i < node->nodes.count ; i++)
		private_transform_node (self, node->nodes.array[i]);
}

static float
private_get_channel_weight (const limdlPose*        self,
                            const limdlPoseChannel* channel)
{
	return limdl_animation_get_weight (channel->animation, channel->time,
		channel->repeat == 0 && channel->repeats != -1? 0.0f : 1.0f, 1.0f,
		channel->repeat == channel->repeats - 1? 0.0f : 1.0f);
}

/** @} */
/** @} */
