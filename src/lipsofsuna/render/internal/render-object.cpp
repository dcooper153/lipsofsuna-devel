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

static LIMdlPose* private_channel_animate (
	LIRenObject*    self,
	LIMdlPose*      pose,
	int             channel,
	LIMdlAnimation* anim,
	int             additive,
	int             repeat,
	int             repeat_start,
	int             keep,
	float           fade_in,
	float           fade_out,
	float           weight,
	float           weight_scale,
	float           time,
	float           time_scale,
	const char**    node_names,
	float*          node_weights,
	int             node_count);

static void private_channel_edit (
	LIRenObject*          self,
	LIMdlPose*            pose,
	int                   channel,
	int                   frame,
	const char*           node,
	const LIMatTransform* transform,
	float                 scale);

static void private_channel_fade (
	LIRenObject* self,
	LIMdlPose*   pose,
	int          channel,
	float        time);

static LIRenEntity* private_create_entity (
	LIRenObject* self,
	LIRenModel*  model);

static void private_remove_entity (
	LIRenObject* self,
	int          index);

static void private_update_entity_settings (
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

	/* Allocate self. */
	self = OGRE_NEW LIRenObject;
	if (self == NULL)
		return NULL;
	self->id = id;
	self->visible = 0;
	self->shadow_casting = 0;
	self->render_distance = -1.0f;
	self->transform = limat_transform_identity ();
	self->pose = NULL;
	self->render = render;
	self->particles = NULL;
	self->node = NULL;

	/* Choose a unique ID. */
	while (!self->id)
	{
		self->id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (render->objects, self->id))
			self->id = 0;
	}

	/* Create the scene node. */
	self->node = render->data->scene_root->createChildSceneNode ();
	self->node->setVisible (false);

	/* Add self to the object dictionary. */
	if (!lialg_u32dic_insert (render->objects, self->id, self))
	{
		OGRE_DELETE self;
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
	/* Remove from the object dictionary. */
	lialg_u32dic_remove (self->render->objects, self->id);

	/* Free models and particles. */
	liren_object_clear_models (self);

	/* Free the scene node. */
	if (self->node != NULL)
		self->render->data->scene_root->removeAndDestroyChild (self->node->getName ());

	/* Free the pose. */
	if (self->pose != NULL)
		limdl_pose_free (self->pose);
	OGRE_DELETE self;
}

/**
 * \brief Adds a model to the object.
 * \param self Object.
 * \param model Model.
 */
void liren_object_add_model (
	LIRenObject* self,
	LIRenModel*  model)
{
	/* Remove particle systems. */
	if (self->particles != NULL)
	{
		self->render->data->scene_manager->destroyParticleSystem (self->particles);
		self->particles = NULL;
	}

	/* Create the new entity. */
	self->entities.push_back (private_create_entity (self, model));
}

int liren_object_channel_animate (
	LIRenObject*    self,
	int             channel,
	LIMdlAnimation* anim,
	int             additive,
	int             repeat,
	int             repeat_start,
	int             keep,
	float           fade_in,
	float           fade_out,
	float           weight,
	float           weight_scale,
	float           time,
	float           time_scale,
	const char**    node_names,
	float*          node_weights,
	int             node_count)
{
	LIMdlPose* pose1;

	/* Update the reference pose. */
	pose1 = private_channel_animate (self, self->pose, channel, anim, additive, repeat,
		repeat_start, keep, fade_in, fade_out, weight, weight_scale, time,
		time_scale, node_names, node_weights, node_count);
	if (pose1 != NULL)
		self->pose = pose1;

	/* Update the real poses controlling skeletons of entities. */
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		LIRenEntity* entity = self->entities[i];
		pose1 = entity->get_pose ();
		if (pose1 != NULL)
		{
			private_channel_animate (self, pose1, channel, anim, additive, repeat,
				repeat_start, keep, fade_in, fade_out, weight, weight_scale, time,
				time_scale, node_names, node_weights, node_count);
		}
		else
			entity->set_pose (self->pose);
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
	private_channel_edit (self, self->pose, channel, frame, node, transform, scale);
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		LIMdlPose* pose = self->entities[i]->get_pose ();
		private_channel_edit (self, pose, channel, frame, node, transform, scale);
	}
}

void liren_object_channel_fade (
	LIRenObject* self,
	int          channel,
	float        time)
{
	private_channel_fade (self, self->pose, channel, time);
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		LIMdlPose* pose = self->entities[i]->get_pose ();
		private_channel_fade (self, pose, channel, time);
	}
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
 * \brief Clears all models from the object.
 * \param self Object.
 */
void liren_object_clear_models (
	LIRenObject* self)
{
	/* Detach everything from the scene node. */
	self->node->detachAllObjects ();

	/* Remove entities. */
	for (size_t i = 0 ; i < self->entities.size () ; i++)
		OGRE_DELETE self->entities[i];
	self->entities.clear ();

	/* Remove the particle system. */
	if (self->particles != NULL)
	{
		self->render->data->scene_manager->destroyParticleSystem (self->particles);
		self->particles = NULL;
	}
}

int liren_object_find_node (
	LIRenObject*    self,
	const char*     name,
	int             world,
	LIMatTransform* result)
{
	float scale;
	LIMatTransform transform;
	LIMdlNode* node = NULL;

	/* Find the node. */
	/* This uses the poses of the entities instead of the pose of the object
	   since the latter never has a model assigned. */
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		LIRenEntity* entity = self->entities[i];
		LIMdlPose* pose = entity->get_pose ();
		if (pose != NULL)
		{
			node = limdl_pose_find_node (pose, name);
			if (node != NULL)
				break;
		}
		if (node == NULL)
		{
			LIMdlModel* model = entity->get_model ();
			if (model != NULL)
				node = limdl_model_find_node (model, name);
		}
	}
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

/**
 * \brief Marks a model for rebuild.
 *
 * This function is called for all objects when a model is changed. Hence, the
 * model does not necessarily exist in the object.
 *
 * The old mesh is not removed immediately because it takes time for the new
 * entity to finish building. To avoid the object disappearing during the
 * build period, we keep the old model around until the build has finished.
 *
 * \param self Object.
 * \param model Model.
 */
void liren_object_model_changed (
	LIRenObject* self,
	LIRenModel*  model)
{
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		LIRenEntity* entity = self->entities[i];
		if (entity->get_render_model () == model)
		{
			/* Start loading the new entity. */
			liren_object_add_model (self, model);

			/* Mark the old entity for replacement. */
			entity->set_replacing_entity (self->entities[self->entities.size () - 1]);
			break;
		}
	}
}

/**
 * \brief Removes a model from the object.
 * \param self Object.
 * \param model Model.
 */
void liren_object_remove_model (
	LIRenObject* self,
	LIRenModel*  model)
{
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		if (self->entities[i]->get_render_model () == model)
		{
			private_remove_entity (self, i);
			break;
		}
	}
}

void liren_object_update (
	LIRenObject* self,
	float        secs)
{
	/* Replace old entities with built ones. */
	/* Removing an entity is a potentially recursive operation so there is no
	   guarantee of the list index being in any given position afthe the removal.
	   Because of that, the operation needs to be restarted from scratch after
	   each removal. */
	while (true)
	{
		bool found = false;
		for (size_t i = 0 ; i < self->entities.size () ; i++)
		{
			LIRenEntity* repl = self->entities[i]->get_replacing_entity ();
			if (repl != NULL && repl->get_loaded ())
			{
				private_remove_entity (self, i);
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}

	/* Hide objects too far away. */
	/* Frustum culling cannot necessarily eliminate all desirable objects if the
	   view distance is very low. Because of that, we allow the user to specify
	   the render distance for the object. */
	if (self->render_distance > 0)
	{
		float dist2 = self->node->getSquaredViewDepth (self->render->data->camera);
		if (dist2 > self->render_distance * self->render_distance)
			self->node->setVisible (false);
		else
			self->node->setVisible (self->visible);
	}
	else
		self->node->setVisible (self->visible);

	/* Update poses. */
	if (self->pose != NULL)
	{
		limdl_pose_update (self->pose, secs);
		for (size_t i = 0 ; i < self->entities.size () ; i++)
			self->entities[i]->update_pose (secs);
	}
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
 * \brief Returns non-zero if the object has finished background loading.
 * \param self Object.
 * \return Non-zero if finished background loading.
 */
int liren_object_get_loaded (
	LIRenObject* self)
{
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		if (!self->entities[i]->get_loaded ())
			return 0;
	}
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
	/* Simply clear everything if setting to NULL. */
	if (model == NULL)
	{
		liren_object_clear_models (self);
		return 1;
	}

	/* Remove the particle system. */
	if (self->particles != NULL)
	{
		self->render->data->scene_manager->destroyParticleSystem (self->particles);
		self->particles = NULL;
	}

	/* Add the new model. */
	liren_object_add_model (self, model);
	LIRenEntity* successor = self->entities[self->entities.size () - 1];

	/* Mark all the old entities for replacement. */
	for (size_t i = 0 ; i < self->entities.size () - 1 ; i++)
		self->entities[i]->set_replacing_entity (successor);

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
	/* Remove the existing model or particle system. */
	liren_object_clear_models (self);

	/* Remove the pose. */
	if (self->pose != NULL)
	{
		limdl_pose_free (self->pose);
		self->pose = NULL;
	}

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
 * \brief Enables or disables particle emission.
 * \param self Object.
 * \param value Nonzero to enable, zero to disable.
 */
void liren_object_set_particle_emitting (
	LIRenObject* self,
	int          value)
{
	if (self->particles != NULL)
		self->particles->setEmitting (value);
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
 * \brief Sets the render distance of the object.
 * \param self Object.
 * \param value Render distance.
 */
void liren_object_set_render_distance (
	LIRenObject* self,
	float        value)
{
	self->render_distance = value;
	private_update_entity_settings (self);
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
	private_update_entity_settings (self);
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

/*****************************************************************************/

static LIMdlPose* private_channel_animate (
	LIRenObject*    self,
	LIMdlPose*      pose,
	int             channel,
	LIMdlAnimation* anim,
	int             additive,
	int             repeat,
	int             repeat_start,
	int             keep,
	float           fade_in,
	float           fade_out,
	float           weight,
	float           weight_scale,
	float           time,
	float           time_scale,
	const char**    node_names,
	float*          node_weights,
	int             node_count)
{
	int i;
	const char* name1;

	/* Create the pose if it doesn't exist. */
	if (pose == NULL)
	{
		pose = limdl_pose_new ();
		if (pose == NULL)
			return NULL;
	}

	/* Avoid restarts in simple cases. */
	/* The position is kept if the animation is repeating and being replaced with
	   the same one but parameters such as fading and weights still need to be reset. */
	if (repeat && channel != -1 && anim != NULL)
	{
		if (keep)
		{
			keep = 0;
			if (limdl_pose_get_channel_state (pose, channel) == LIMDL_POSE_CHANNEL_STATE_PLAYING &&
				limdl_pose_get_channel_repeats (pose, channel) == -1)
			{
				name1 = limdl_pose_get_channel_name (pose, channel);
				if (!strcmp (anim->name, name1))
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
			if (limdl_pose_get_channel_state (pose, channel) == LIMDL_POSE_CHANNEL_STATE_INVALID)
				break;
		}
	}

	/* Clear the channel if NULL was given as the animation. */
	if (anim == NULL)
	{
		if (keep)
			limdl_pose_fade_channel (pose, channel, LIMDL_POSE_FADE_AUTOMATIC);
		else
			limdl_pose_destroy_channel (pose, channel);
		return pose;
	}

	/* Update or initialize the channel. */
	if (!keep)
	{
		limdl_pose_fade_channel (pose, channel, LIMDL_POSE_FADE_AUTOMATIC);
		limdl_pose_set_channel_animation (pose, channel, anim);
		limdl_pose_set_channel_repeats (pose, channel, repeat? -1 : 1);
		limdl_pose_set_channel_position (pose, channel, time);
		limdl_pose_set_channel_state (pose, channel, LIMDL_POSE_CHANNEL_STATE_PLAYING);
	}
	limdl_pose_set_channel_additive (pose, channel, additive);
	limdl_pose_set_channel_repeat_start (pose, channel, repeat_start);
	limdl_pose_set_channel_priority_scale (pose, channel, weight_scale);
	limdl_pose_set_channel_priority_transform (pose, channel, weight);
	limdl_pose_set_channel_time_scale (pose, channel, time_scale);
	limdl_pose_set_channel_fade_in (pose, channel, fade_in);
	limdl_pose_set_channel_fade_out (pose, channel, fade_out);

	/* Handle optional per-node weights. */
	if (anim != NULL && node_count)
	{
		limdl_pose_clear_channel_node_priorities (pose, channel);
		for (i = 0 ; i < node_count ; i++)
		{
			limdl_pose_set_channel_priority_node (pose, channel,
				node_names[i], node_weights[i]);
		}
	}

	return pose;
}

static void private_channel_edit (
	LIRenObject*          self,
	LIMdlPose*            pose,
	int                   channel,
	int                   frame,
	const char*           node,
	const LIMatTransform* transform,
	float                 scale)
{
	if (pose != NULL)
		limdl_pose_set_channel_transform (pose, channel, frame, node, scale, transform);
}

static void private_channel_fade (
	LIRenObject* self,
	LIMdlPose*   pose,
	int          channel,
	float        time)
{
	if (pose != NULL)
		limdl_pose_fade_channel (pose, channel, time);
}

static LIRenEntity* private_create_entity (
	LIRenObject* self,
	LIRenModel*  model)
{
	/* Make sure that a model was given. */
	if (model == NULL || model->mesh.isNull ())
		return NULL;

	/* Create a new entity. */
	Ogre::String e_name = self->render->data->id.next ();
	LIRenEntity* entity = OGRE_NEW LIRenEntity (e_name, model);
	self->node->attachObject (entity);

	/* Update the pose for the new model data. */
	/* The model data might be NULL currently, but that can be because the model
	   is is still loading in the background. We cannot remove the pose because
	   of that since the caller might want to transfer it to the new model once
	   it has loaded. */
	if (self->pose != NULL)
		entity->set_pose (self->pose);

	/* Set the entity flags. */
	entity->setCastShadows (self->shadow_casting);

	/* Set entity visibility. */
	/* If a visible entity is added to a hidden scene node, the entity is
	   still rendered. Hence, newly added entities needs to be explicitly
	   hidden or Ogre will render our invisible objects. */
	entity->setVisible (self->visible);

	return entity;
}

static void private_remove_entity (
	LIRenObject* self,
	int          index)
{
	/* Remove from the list and the scene node. */
	LIRenEntity* entity = self->entities[index];
	self->node->detachObject (entity);
	self->entities.erase(self->entities.begin () + index);

	/* Free entities waiting for the removal of this entity. */
	/* This is a potentially recursive operation so the list indices may change
	   wildly. To avoid out of bounds errors for sure, we restart the loop from
	   scratch immediately after removing something. */
	while (true)
	{
		bool found = false;
		for (size_t i = 0 ; i < self->entities.size () ; i++)
		{
			if (entity == self->entities[i]->get_replacing_entity ())
			{
				private_remove_entity (self, i);
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}

	/* Free this entity. */
	OGRE_DELETE entity;
}

static void private_update_entity_settings (
	LIRenObject* self)
{
	for (size_t i = 0 ; i < self->entities.size () ; i++)
	{
		LIRenEntity* entity = self->entities[i];
		entity->setCastShadows (self->shadow_casting);
	}
}

/** @} */
/** @} */
/** @} */
