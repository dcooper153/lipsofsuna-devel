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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenObject Object
 * @{
 */

#include "lipsofsuna/model.h"
#include "lipsofsuna/network.h"
#include "lipsofsuna/system.h"
#include "render.h"
#include "render-model.h"
#include "render-object.h"

static Ogre::String private_unique_id (
	LIRenObject* self);

/*****************************************************************************/

/**
 * \brief Creates a new render object and adds it to the scene.
 * \param render Renderer.
 * \param id Unique identifier.
 * \return Object or NULL on failure.
 */
LIRenObject* liren_object_new (
	LIRenRender* render,
	int          id)
{
	LIRenObject* self;

	self = (LIRenObject*) lisys_calloc (1, sizeof (LIRenObject));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->shadow_casting = 0;

	/* Initialize the private data. */
	self->data = (LIRenObjectData*) lisys_calloc (1, sizeof (LIRenObjectData));
	if (self->data == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Choose a unique ID. */
	while (!id)
	{
		id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (render->objects, id))
			id = 0;
	}
	self->id = id;

	/* Initialize the backend. */
	self->data->node = render->data->scene_manager->getRootSceneNode ()->createChildSceneNode ();
	self->data->node->setVisible (false);

	/* Add to dictionary. */
	if (!lialg_u32dic_insert (render->objects, id, self))
	{
		liren_object_free (self);
		return 0;
	}

	return self;
}

/**
 * \brief Frees the render object.
 * \param self Object.
 */
void liren_object_free (
	LIRenObject* self)
{
	lialg_u32dic_remove (self->render->objects, self->id);

	/* Free the private data. */
	if (self->data != NULL)
	{
		if (self->data->entity != NULL)
		{
			self->data->node->detachAllObjects ();
			self->render->data->scene_manager->destroyEntity (self->data->entity);
		}
		if (self->data->node != NULL)
			self->render->data->scene_root->removeAndDestroyChild (self->data->node->getName ());
		lisys_free (self->data);
	}

	if (self->pose != NULL)
		limdl_pose_free (self->pose);
	lisys_free (self);
}

int liren_object_channel_animate (
	LIRenObject* self,
	int          channel,
	const char*  name,
	int          additive,
	int          repeat,
	int          repeat_start,
	int          keep,
	float        fade_in,
	float        fade_out,
	float        weight,
	float        weight_scale,
	float        time,
	float        time_scale,
	const char** node_names,
	float*       node_weights,
	int          node_count)
{
	int i;
	const char* name1;

	/* Create the pose if it doesn't exist. */
	if (self->pose == NULL)
	{
		self->pose = limdl_pose_new ();
		if (self->pose == NULL)
			return 0;
		if (self->model != NULL)
			limdl_pose_set_model (self->pose, self->model->model);
		/* TODO */
	}

	/* Avoid restarts in simple cases. */
	/* The position is kept if the animation is repeating and being replaced with
	   the same one but parameters such as fading and weights still need to be reset. */
	if (repeat && channel != -1 && name != NULL)
	{
		if (limdl_pose_get_channel_state (self->pose, channel) == LIMDL_POSE_CHANNEL_STATE_PLAYING &&
		    limdl_pose_get_channel_repeats (self->pose, channel) == -1)
		{
			name1 = limdl_pose_get_channel_name (self->pose, channel);
			if (!strcmp (name, name1))
				keep = 1;
		}
	}

	/* Automatic channel assignment. */
	if (channel == -1)
	{
		for (channel = 254 ; channel > 0 ; channel--)
		{
			if (limdl_pose_get_channel_state (self->pose, channel) == LIMDL_POSE_CHANNEL_STATE_INVALID)
				break;
		}
	}

	/* Update or initialize the channel. */
	if (!keep)
	{
		limdl_pose_fade_channel (self->pose, channel, LIMDL_POSE_FADE_AUTOMATIC);
		if (name != NULL)
		{
			limdl_pose_set_channel_animation (self->pose, channel, name);
			limdl_pose_set_channel_repeats (self->pose, channel, repeat? -1 : 1);
			limdl_pose_set_channel_position (self->pose, channel, time);
			limdl_pose_set_channel_state (self->pose, channel, LIMDL_POSE_CHANNEL_STATE_PLAYING);
		}
	}
	limdl_pose_set_channel_additive (self->pose, channel, additive);
	limdl_pose_set_channel_repeat_start (self->pose, channel, repeat_start);
	limdl_pose_set_channel_priority_scale (self->pose, channel, weight_scale);
	limdl_pose_set_channel_priority_transform (self->pose, channel, weight);
	limdl_pose_set_channel_time_scale (self->pose, channel, time_scale);
	limdl_pose_set_channel_fade_in (self->pose, channel, fade_in);
	limdl_pose_set_channel_fade_out (self->pose, channel, fade_out);

	/* Handle optional per-node weights. */
	if (name != NULL && node_count)
	{
		limdl_pose_clear_channel_node_priorities (self->pose, channel);
		for (i = 0 ; i < node_count ; i++)
		{
			limdl_pose_set_channel_priority_node (self->pose, channel,
				node_names[i], node_weights[i]);
		}
	}

	return 1;
}

void liren_object_channel_edit (
	LIRenObject*          self,
	int                   channel,
	int                   frame,
	const char*           node,
	const LIMatTransform* transform,
	float                 scale)
{
	if (self->pose != NULL)
		limdl_pose_set_channel_transform (self->pose, channel, frame, node, scale, transform);
}

void liren_object_channel_fade (
	LIRenObject* self,
	int          channel,
	float        time)
{
	if (self->pose != NULL)
		limdl_pose_fade_channel (self->pose, channel, time);
}

LIMdlPoseChannel* liren_object_channel_get_state (
	LIRenObject* self,
	int          channel)
{
	LIMdlPoseChannel* chan;

	if (self->pose == NULL)
		return NULL;
	chan = (LIMdlPoseChannel*) lialg_u32dic_find (self->pose->channels, channel);
	if (chan == NULL)
		return NULL;
	chan = limdl_pose_channel_new_copy (chan);

	return chan;
}

/**
 * \brief Deforms the object.
 * \param self Object.
 */
void liren_object_deform (
	LIRenObject* self)
{
	/* TODO */
}

int liren_object_find_node (
	LIRenObject*    self,
	const char*     name,
	int             world,
	LIMatTransform* result)
{
	float scale;
	LIMatTransform transform;
	LIMdlNode* node;

	/* Find the node. */
	if (self->model == NULL || self->model->model == NULL)
		return 0;
	if (self->pose == NULL)
		node = limdl_model_find_node (self->model->model, name);
	else
		node = limdl_pose_find_node (self->pose, name);
	if (node == NULL)
		return 0;

	/* Get the transformation. */
	limdl_node_get_world_transform (node, &scale, &transform);
	if (world)
		transform = limat_transform_multiply (self->transform, transform);
	*result = transform;

	return 1;
}

/**
 * \brief Sets the particle animation state of the object.
 * \param self Object.
 * \param start Animation offset in seconds.
 * \param loop Nonzero to enable looping.
 */
void liren_object_particle_animation (
	LIRenObject* self,
	float        start,
	int          loop)
{
	/* TODO */
}

void liren_object_model_changed (
	LIRenObject* self)
{
	liren_object_set_model (self, self->model);
}

void liren_object_update_pose (
	LIRenObject* self)
{
	/* Do nothing if the object isn't initialized. */
	if (self->data->entity == NULL)
		return;

	/* Get the skeleton. */
	/* If the model doesn't have one, we don't need to do anything. */
	Ogre::SkeletonInstance* skeleton = self->data->entity->getSkeleton ();
	if (skeleton == NULL)
		return;

	/* Update weight group bones. */
	/* The hierarchy doesn't matter because LIMdlPose already calculated the
	   global transformations of the bones. We just need to copy the
	   transformations of the bones used by weight groups. */
	for (int i = 0 ; i < self->model->model->weight_groups.count ; i++)
	{
		LIMdlWeightGroup* group = self->model->model->weight_groups.array + i;
		if (group->node != NULL)
		{
			LIMdlNode* node = limdl_pose_find_node (self->pose, group->node->name);
			if (node != NULL)
			{
				Ogre::Bone* bone = skeleton->getBone (i + 1);
				LIMatTransform t = node->pose_transform.global;
				float s = node->pose_transform.global_scale;
				bone->setScale (s, s, s);
				bone->setPosition (t.position.x, t.position.y, t.position.z);
				bone->setOrientation (t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z);
			}
		}
	}

	skeleton->_notifyManualBonesDirty ();
}

/**
 * \brief Sets the effect layer of the object.
 * \param self Object.
 * \param shader Shader name or NULL to disable the effect.
 * \param params Effect parameters or NULL.
 * \return Nonzero on success.
 */
int liren_object_set_effect (
	LIRenObject* self,
	const char*  shader,
	const float* params)
{
	/* TODO */

	return 1;
}

/**
 * \brief Sets the model of the object.
 * \param self Object.
 * \param model Model.
 * \return Nonzero on success.
 */
int liren_object_set_model (
	LIRenObject* self,
	LIRenModel*  model)
{
	/* Update the pose if the model has one. */
	if (self->pose != NULL)
	{
		if (model != NULL)
			limdl_pose_set_model (self->pose, model->model);
		else
		{
			limdl_pose_free (self->pose);
			self->pose = NULL;
		}
	}

	self->model = model;

	/* Remove the old entity or particle system. */
	self->data->node->detachAllObjects ();
	if (self->data->entity != NULL)
	{
		self->render->data->scene_manager->destroyEntity (self->data->entity);
		self->data->entity = NULL;
	}
	if (self->data->particles != NULL)
	{
		self->render->data->scene_manager->destroyParticleSystem (self->data->particles);
		self->data->particles = NULL;
	}

	/* Attach a new entity to the scene node. */
	if (model != NULL && !model->data->mesh.isNull ())
	{
		Ogre::String e_name = private_unique_id (self);
		Ogre::String m_name = model->data->mesh->getName ();
		self->data->entity = self->render->data->scene_manager->createEntity (e_name, m_name);
		self->data->node->attachObject (self->data->entity);
	}

	/* Set entity flags. */
	if (self->data->entity != NULL)
		self->data->entity->setCastShadows (self->shadow_casting);

	/* Mark all bones as manually controlled. */
	if (self->data->entity != NULL)
	{
		Ogre::SkeletonInstance* skeleton = self->data->entity->getSkeleton ();
		if (skeleton != NULL)
		{
			for (int i = 0 ; i < skeleton->getNumBones () ; i++)
			{
				Ogre::Bone* bone = skeleton->getBone (i);
				bone->setManuallyControlled (true);
			}
		}
	}

	return 1;
}

/**
 * \brief Sets the particle effect of the object.
 * \param self Object.
 * \param name Particle effect name.
 * \return Nonzero on success.
 */
int liren_object_set_particle (
	LIRenObject* self,
	const char*  name)
{
	/* Remove the pose. */
	if (self->pose != NULL)
	{
		limdl_pose_free (self->pose);
		self->pose = NULL;
	}

	/* Remove the existing model or particle system. */
	self->data->node->detachAllObjects ();
	if (self->data->entity != NULL)
	{
		self->render->data->scene_manager->destroyEntity (self->data->entity);
		self->data->entity = NULL;
	}
	if (self->data->particles != NULL)
	{
		self->render->data->scene_manager->destroyParticleSystem (self->data->particles);
		self->data->particles = NULL;
	}
	self->model = NULL;

	/* Attach a new particle system to the scene node. */
	try
	{
		Ogre::String e_name = private_unique_id (self);
		self->data->particles = self->render->data->scene_manager->createParticleSystem (e_name, name);
		lisys_assert (self->data->particles != NULL);
		self->data->node->attachObject (self->data->particles);
	}
	catch (...)
	{
	}

	return 1;
}

/**
 * \brief Sets the realized flag of the object.
 * \param self Object.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int liren_object_set_realized (
	LIRenObject* self,
	int          value)
{
	self->data->node->setVisible (value);
	return 1;
}

/**
 * \brief Sets the shadow casting mode of the object.
 * \param self Object.
 * \param value Nonzero to allow shadow casting, zero to disable.
 */
void liren_object_set_shadow (
	LIRenObject* self,
	int          value)
{
	self->shadow_casting = value;
	if (self->data->entity != NULL)
		self->data->entity->setCastShadows (self->shadow_casting);
}

/**
 * \brief Sets the transformation of the object.
 * \param self Object.
 * \param value Transformation.
 */
void liren_object_set_transform (
	LIRenObject*          self,
	const LIMatTransform* value)
{
	self->transform = *value;
	self->data->node->setPosition (value->position.x, value->position.y, value->position.z);
	self->data->node->setOrientation (value->rotation.w, value->rotation.x, value->rotation.y, value->rotation.z);
}

/*****************************************************************************/

static Ogre::String private_unique_id (
	LIRenObject* self)
{
	return Ogre::StringConverter::toString (self->id);
}

/** @} */
/** @} */
/** @} */
