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
#include "render-entity.hpp"
#include "render-model.h"
#include "render-object.h"

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
	self->visible = 0;
	self->shadow_casting = 0;

	/* Choose a unique ID. */
	while (!id)
	{
		id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (render->objects, id))
			id = 0;
	}
	self->id = id;

	/* Initialize the backend. */
	self->node = render->data->scene_root->createChildSceneNode ();
	self->node->setVisible (false);

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
	if (self->node != NULL)
		self->node->detachAllObjects ();
	if (self->entity != NULL)
		OGRE_DELETE self->entity;
	if (self->particles != NULL)
		self->render->data->scene_manager->destroyParticleSystem (self->particles);
	if (self->node != NULL)
		self->render->data->scene_root->removeAndDestroyChild (self->node->getName ());

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
			limdl_pose_set_model (self->pose, liren_model_get_model (self->model));
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

int liren_object_find_node (
	LIRenObject*    self,
	const char*     name,
	int             world,
	LIMatTransform* result)
{
	float scale;
	LIMatTransform transform;
	LIMdlModel* model;
	LIMdlNode* node;

	/* Get the model. */
	if (self->model == NULL)
		return 0;
	model = liren_model_get_model (self->model);
	if (model == NULL)
		return 0;

	/* Find the node. */
	if (self->pose == NULL)
		node = limdl_model_find_node (model, name);
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
	LIMdlModel* model;

	/* Do nothing if the object isn't initialized. */
	if (self->entity == NULL)
		return;

	/* Get the skeleton. */
	/* If the model doesn't have one, we don't need to do anything. */
	Ogre::SkeletonInstance* skeleton = self->entity->getSkeleton ();
	if (skeleton == NULL)
		return;

	/* Get the model. */
	model = liren_model_get_model (self->model);
	lisys_assert (model != NULL);

	/* Update weight group bones. */
	/* The hierarchy doesn't matter because LIMdlPose already calculated the
	   global transformations of the bones. We just need to copy the
	   transformations of the bones used by weight groups. */
	for (int i = 0 ; i < model->weight_groups.count ; i++)
	{
		LIMdlWeightGroup* group = model->weight_groups.array + i;
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
 * \brief Gets the ID of the object.
 * \param self Object.
 * \return ID.
 */
int liren_object_get_id (
	LIRenObject* self)
{
	return self->id;
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
	LIMdlModel* model_data;

	/* Remove the old entity or particle system. */
	self->node->detachAllObjects ();
	if (self->entity != NULL)
	{
		OGRE_DELETE self->entity;
		self->entity = NULL;
	}
	if (self->particles != NULL)
	{
		self->render->data->scene_manager->destroyParticleSystem (self->particles);
		self->particles = NULL;
	}

	/* Store the new render model. */
	self->model = model;

	/* Create a new entity. */
	if (model != NULL && !model->mesh.isNull ())
	{
		Ogre::String e_name = self->render->data->id.next ();
		self->entity = OGRE_NEW LIRenEntity (e_name, model->mesh);
		self->node->attachObject (self->entity);
	}

	/* Get the model copy created by the entity. */
	if (self->entity != NULL)
		model_data = self->entity->get_model ();
	else
		model_data = NULL;

	/* Update the pose for the new model data. */
	if (self->pose != NULL)
	{
		if (model_data != NULL)
			limdl_pose_set_model (self->pose, model_data);
		else
		{
			limdl_pose_free (self->pose);
			self->pose = NULL;
		}
	}

	/* Set the entity flags. */
	if (self->entity != NULL)
		self->entity->setCastShadows (self->shadow_casting);

	/* Set entity visibility. */
	/* If a visible entity is added to a hidden scene node, the entity is
	   still rendered. Hence, newly added entities needs to be explicitly
	   hidden or Ogre will render our invisible objects. */
	if (self->entity != NULL)
		self->entity->setVisible (self->visible);

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
	self->node->detachAllObjects ();
	if (self->entity != NULL)
	{
		OGRE_DELETE self->entity;
		self->entity = NULL;
	}
	if (self->particles != NULL)
	{
		self->render->data->scene_manager->destroyParticleSystem (self->particles);
		self->particles = NULL;
	}
	self->model = NULL;

	try
	{
		/* Attach a new particle system to the scene node. */
		Ogre::String e_name = self->render->data->id.next ();
		self->particles = self->render->data->scene_manager->createParticleSystem (e_name, name);
		lisys_assert (self->particles != NULL);
		self->node->attachObject (self->particles);

		/* Set particle effect visibility. */
		self->particles->setVisible (self->visible);
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
	self->visible = value;
	self->node->setVisible (value);
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
	if (self->entity != NULL)
		self->entity->setCastShadows (self->shadow_casting);
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
	self->node->setPosition (value->position.x, value->position.y, value->position.z);
	self->node->setOrientation (value->rotation.w, value->rotation.x, value->rotation.y, value->rotation.z);
}

/** @} */
/** @} */
/** @} */
