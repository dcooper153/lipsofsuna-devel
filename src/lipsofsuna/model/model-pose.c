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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlPose Pose
 * @{
 */

#include <lipsofsuna/system.h>
#include "model-pose.h"

static void private_channel_free (
	LIMdlPoseChannel* chan);

static void private_clear_node (
	LIMdlPose*       self,
	LIMdlNode*       node,
	const LIMdlNode* rest);

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

static int private_init_pose (
	LIMdlPose*  self,
	LIMdlModel* model);

static int private_play_channel (
	const LIMdlPose*  self,
	LIMdlPoseChannel* channel,
	float             secs);

static void private_transform_node (
	LIMdlPose* self,
	LIMdlNode* node);

static float private_get_channel_weight (
	const LIMdlPose*        self,
	const LIMdlPoseChannel* channel);

static LIMdlAnimation private_empty_anim =
{
	0, "empty", 0.0f, 0.0f, { 0, NULL }, { 0, NULL }
};

/*****************************************************************************/

/**
 * \brief Creates a new model pose.
 * \param model The model this pose is for.
 * \return New model pose or NULL.
 */
LIMdlPose* limdl_pose_new (
	LIMdlModel* model)
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
 * \brief Frees the model pose.
 * \param self A model pose.
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
	private_channel_free (chan);
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
void limdl_pose_fade_channel (
	LIMdlPose* self,
	int        channel,
	float      rate)
{
	LIMdlPoseFade* fade;
	LIMdlPoseChannel* chan;

	chan = lialg_u32dic_find (self->channels, channel);
	if (chan == NULL)
		return;

	/* Handle auto rate. */
	if (rate == LIMDL_POSE_FADE_AUTOMATIC)
	{
		if (chan->fade_out < LIMAT_EPSILON)
			return;
		rate = 1.0f / chan->fade_out;
	}

	/* Create a fade sequence. */
	fade = lisys_calloc (1, sizeof (LIMdlPoseFade));
	if (fade == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		return;
	}
	fade->weight = chan->priority * private_get_channel_weight (self, chan);
	fade->rate = fade->weight * rate;
	fade->time = chan->time;
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
 * \brief Finds a node by name.
 *
 * \param self Model pose.
 * \param name Name of the node to find.
 * \return Node or NULL.
 */
LIMdlNode* limdl_pose_find_node (
	const LIMdlPose* self,
	const char*      name)
{
	int i;
	LIMdlNode* node;

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
void limdl_pose_update (
	LIMdlPose* self,
	float      secs)
{
	int i;
	LIAlgU32dicIter iter;
	LIMatQuaternion quat0;
	LIMatQuaternion quat1;
	LIMdlPoseFade* fade;
	LIMdlPoseFade* fade_next;
	LIMdlPoseChannel* chan;
	LIMdlPoseGroup* group;
	LIMdlNode* node0;
	LIMdlNode* node1;

	if (self->model == NULL)
		return;

	/* Update channels. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
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
			private_fade_remove (self, fade);
			private_fade_free (fade);
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

	/* Update pose group transformations. */
	for (i = 0 ; i < self->groups.count ; i++)
	{
		group = self->groups.array + i;
		if (group->enabled)
		{
			quat0 = group->rest_node->transform.global.rotation;
			quat1 = group->pose_node->transform.global.rotation;
			quat0 = limat_quaternion_conjugate (quat0);
			group->rotation = limat_quaternion_multiply (quat1, quat0);
			group->head_pose = group->pose_node->transform.global.position;
		}
	}
}

void limdl_pose_transform (
	LIMdlPose*   self,
	LIMdlVertex* vertices)
{
	int i;
	int j;
	LIMatVector tmp;
	LIMatVector rest_vertex;
	LIMatVector pose_vertex;
	LIMatVector rest_normal;
	LIMatVector pose_normal;
	LIMdlPoseGroup* group;
	LIMdlPoseVertex* vertex;
	LIMdlWeight* weight;
	LIMdlWeights* weights;

	if (self->model == NULL)
		return;

	/* Transform each vertex. */
	for (i = 0 ; i < self->model->vertices.count ; i++)
	{
		/* Get the rest pose state. */
		vertex = self->vertices.array + i;
		rest_vertex = self->model->vertices.array[i].coord;
		rest_normal = self->model->vertices.array[i].normal;
		pose_vertex = limat_vector_init (0.0f, 0.0f, 0.0f);
		pose_normal = limat_vector_init (0.0f, 0.0f, 0.0f);

		/* Transform by each weight group. */
		if (vertex->weight_count && vertex->weight_total != 0.0f)
		{
			weights = self->model->weights.array + i;
			for (j = 0 ; j < weights->count ; j++)
			{
				/* Transform by a weight group. */
				weight = weights->weights + j;
				group = self->groups.array + weight->group;
				if (weight->weight != 0.0f && group->enabled)
				{
					/* Transform the vertex. */
					tmp = limat_vector_subtract (rest_vertex, group->head_rest);
					tmp = limat_quaternion_transform (group->rotation, tmp);
					tmp = limat_vector_add (tmp, group->head_pose);
					pose_vertex = limat_vector_add (pose_vertex,
						limat_vector_multiply (tmp, weight->weight / vertex->weight_total));

					/* Transform the normal. */
					tmp = limat_quaternion_transform (group->rotation, rest_normal);
					pose_normal = limat_vector_add (pose_normal,
						limat_vector_multiply (tmp, weight->weight / vertex->weight_total));
				}
			}
		}
		else
		{
			/* Default to the rest pose. */
			pose_vertex = rest_vertex;
			pose_normal = rest_normal;
		}

		/* Set the transformed state. */
		vertices[i].coord = pose_vertex;
		vertices[i].normal = limat_vector_normalize (pose_normal);
	}
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
	LIMdlPose*  self,
	int         channel,
	const char* animation)
{
	LIMdlAnimation* anim;
	LIMdlPoseChannel* chan;

	if (self->model == NULL)
		return;

	/* Create an animation. */
	anim = limdl_model_find_animation (self->model, animation);
	if (anim == NULL)
	{
		limdl_pose_destroy_channel (self, channel);
		return;
	}
	anim = limdl_animation_new_copy (anim);
	if (anim == NULL)
		return;

	/* Create a channel. */
	chan = private_create_channel (self, channel);
	if (chan == NULL)
	{
		limdl_animation_free (anim);
		return;
	}
	chan->time = 0.0f;
	chan->fade_in = 0.0f;
	chan->fade_out = 0.0f;
	chan->animation = anim;
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

float limdl_pose_get_channel_priority (
	const LIMdlPose* self,
	int              channel)
{
	LIMdlPoseChannel* chan;

	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0.0f;
	return chan->priority;
}

void limdl_pose_set_channel_priority (
	LIMdlPose* self,
	int        channel,
	float      value)
{
	LIMdlPoseChannel* chan;

	chan = private_create_channel (self, channel);
	if (chan == NULL)
		return;
	chan->priority = value;
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
 *
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

/**
 * \brief Sets the transformation of a node for the given frame.
 * \param self Pose.
 * \param channel Channel number.
 * \param frame Frame number.
 * \param node Node name.
 * \param transform Node transformation.
 * \return Nonzero on success.
 */
int limdl_pose_set_channel_transform (
	LIMdlPose*            self,
	int                   channel,
	int                   frame,
	const char*           node,
	const LIMatTransform* transform)
{
	LIMdlNode* node_;
	LIMdlPoseChannel* chan;

	/* Find the node. */
	node_ = limdl_pose_find_node (self, node);
	if (node_ == NULL)
		return 0;

	/* Find the channel. */
	chan = private_find_channel (self, channel);
	if (chan == NULL)
		return 0;

	/* Make sure the channel and the frame exist. */
	if (!limdl_animation_insert_channel (chan->animation, node))
		return 0;
	if (chan->animation->length <= frame)
	{
		if (!limdl_animation_set_length (chan->animation, frame + 1))
			return 0;
	}

	/* Set the node transformation of the frame. */
	limdl_animation_set_transform (chan->animation, node, frame, transform);

	return 1;
}

/**
 * \brief Sets the posed model.
 *
 * \param self Model pose.
 * \param model Model to pose.
 * \return Nonzero on success.
 */
int limdl_pose_set_model (
	LIMdlPose*  self,
	LIMdlModel* model)
{
	LIAlgU32dicIter iter;
	LIMdlAnimation* anim;
	LIMdlPose backup;
	LIMdlPoseChannel* chan;
	LIMdlPoseFade* fade;
	LIMdlPoseFade* fade_next;

	/* Backup old data. */
	memcpy (&backup, self, sizeof (LIMdlPose));
	self->channels = NULL;
	self->fades = NULL;
	self->model = NULL;
	self->groups.count = 0;
	self->groups.array = NULL;
	self->nodes.count = 0;
	self->nodes.array = NULL;
	self->vertices.count = 0;
	self->vertices.array = NULL;

	/* Initialize new pose. */
	if (model != NULL)
	{
		if (!private_init_pose (self, model))
		{
			private_clear_pose (self);
			memcpy (self, &backup, sizeof (LIMdlPose));
			return 0;
		}
	}

	/* Clear invalid animations. */
	self->channels = backup.channels;
	backup.channels = NULL;
	LIALG_U32DIC_FOREACH (iter, self->channels)
	{
		chan = iter.value;
		if (model != NULL)
		{
			anim = limdl_model_find_animation (model, chan->animation->name);
			if (anim != NULL)
				anim = limdl_animation_new_copy (anim);
		}
		else
			anim = NULL;
		if (anim != NULL)
		{
			limdl_animation_free (chan->animation);
			chan->animation = anim;
		}
		else
		{
			lialg_u32dic_remove (self->channels, iter.key);
			private_channel_free (chan);
		}
	}

	/* Clear invalid fades. */
	self->fades = backup.fades;
	backup.fades = NULL;
	for (fade = self->fades ; fade != NULL ; fade = fade_next)
	{
		fade_next = fade->next;
		if (model != NULL)
			anim = limdl_model_find_animation (model, fade->animation->name);
		else
			anim = NULL;
		if (anim == NULL)
		{
			private_fade_remove (self, fade);
			private_fade_free (fade);
		}
	}

	/* Clear old data. */
	private_clear_pose (&backup);

	return 1;
}

/*****************************************************************************/

static void private_channel_free (
	LIMdlPoseChannel* chan)
{
	limdl_animation_free (chan->animation);
	lisys_free (chan);
}

static void private_clear_node (
	LIMdlPose*       self,
	LIMdlNode*       node,
	const LIMdlNode* rest)
{
	int i;
	LIMdlNode* node0;
	LIMdlNode* node1;

	node->transform.global = rest->transform.global;
	for (i = 0 ; i < node->nodes.count ; i++)
	{
		node0 = node->nodes.array[i];
		node1 = rest->nodes.array[i];
		private_clear_node (self, node0, node1);
	}
}

static void private_clear_pose (
	LIMdlPose* self)
{
	int i;
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

	/* Clear channel tree. */
	if (self->channels != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->channels)
			private_channel_free (iter.value);
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

	lisys_free (self->groups.array);
	lisys_free (self->vertices.array);
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

static int private_init_pose (
	LIMdlPose*  self,
	LIMdlModel* model)
{
	int i;
	int j;
	LIMdlPoseGroup* pose_group;
	LIMdlPoseVertex* pose_vertex;
	LIMdlWeight* weight;
	LIMdlWeights* weights;
	LIMdlWeightGroup* weight_group;

	/* Set model. */
	self->model = model;
	self->groups.count = model->weightgroups.count;
	self->nodes.count = model->nodes.count;
	if (model->weights.count)
		self->vertices.count = model->vertices.count;

	/* Copy nodes. */
	if (self->nodes.count)
	{
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (LIMdlNode*));
		if (self->nodes.array == NULL)
			return 0;
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_copy (model->nodes.array[i]);
			if (self->nodes.array[i] == NULL)
				return 0;
		}
	}

	/* Precalculate weight group information. */
	if (self->groups.count)
	{
		self->groups.array = lisys_calloc (self->groups.count, sizeof (LIMdlPoseGroup));
		if (self->groups.array == NULL)
			return 0;
		for (i = 0 ; i < self->groups.count ; i++)
		{
			weight_group = model->weightgroups.array + i;
			pose_group = self->groups.array + i;
			pose_group->weight_group = weight_group;
			pose_group->rest_node = weight_group->node;
			pose_group->pose_node = limdl_pose_find_node (self, weight_group->bone);
			pose_group->rotation = limat_quaternion_identity ();
			if (pose_group->rest_node != NULL)
				pose_group->head_rest = pose_group->rest_node->transform.global.position;
			if (pose_group->rest_node != NULL && pose_group->pose_node != NULL)
				pose_group->enabled = 1;
		}
	}

	/* Precalculate vertex weight information. */
	if (self->vertices.count)
	{
		self->vertices.array = lisys_calloc (self->vertices.count, sizeof (LIMdlPoseVertex));
		if (self->vertices.array == NULL)
			return 0;
		for (i = 0 ; i < self->vertices.count ; i++)
		{
			pose_vertex = self->vertices.array + i;
			pose_vertex->weight_count = 0;
			pose_vertex->weight_total = 0.0f;
			weights = model->weights.array + i;
			for (j = 0 ; j < weights->count ; j++)
			{
				weight = weights->weights + j;
				pose_group = self->groups.array + weight->group;
				if (weight->weight != 0.0f && pose_group->enabled)
				{
					pose_vertex->weight_total += weight->weight;
					pose_vertex->weight_count++;
				}
			}
		}
	}

	return 1;
}

static int private_play_channel (
	const LIMdlPose*  self,
	LIMdlPoseChannel* channel,
	float             secs)
{
	int cycles;
	float duration;

	/* Skip empty. */
	duration = limdl_animation_get_duration (channel->animation);
	if (duration < LIMAT_EPSILON)
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
	channel->repeat += cycles;
	if (channel->repeats != -1 && channel->repeat >= channel->repeats)
		return 0;

	return 1;
}

static void private_transform_node (
	LIMdlPose* self,
	LIMdlNode* node)
{
	int i;
	int channels;
	float total;
	float weight;
	LIAlgU32dicIter iter;
	LIMatQuaternion bonerot;
	LIMatQuaternion rotation;
	LIMatTransform transform;
	LIMatVector bonepos;
	LIMatVector position;
	LIMdlPoseFade* fade;
	LIMdlPoseChannel* chan;

	channels = 0;
	total = 0.0f;
	position = limat_vector_init (0.0f, 0.0f, 0.0f);
	rotation = limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f);

	/* Sum channel weights. */
	LIALG_U32DIC_FOREACH (iter, self->channels)
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
	if (channels && total >= LIMAT_EPSILON)
	{
		/* Apply channel influences. */
		LIALG_U32DIC_FOREACH (iter, self->channels)
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
			if (limdl_animation_get_transform (fade->animation, node->name, fade->time, &transform))
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

static float private_get_channel_weight (
	const LIMdlPose*        self,
	const LIMdlPoseChannel* channel)
{
	float end;
	float time;
	float duration;

	duration = limdl_animation_get_duration (channel->animation);
	time = channel->repeat * duration + channel->time;
	end = channel->repeats * duration;

	/* Fade in period. */
	if (time < channel->fade_in)
		return time / channel->fade_in;

	/* No fade period. */
	if (channel->repeats == -1 || time < end - channel->fade_out)
		return 1.0f;

	/* Fade out period. */
	return 1.0f - (end - time) / channel->fade_out;
}

/** @} */
/** @} */
