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
#include "lipsofsuna/system.h"
#include "render.h"
#include "render-attachment-entity.hpp"
#include "render-attachment-particle.hpp"
#include "render-model.h"
#include "render-object.h"

static LIMdlPose* private_channel_animate (
	LIRenObject*            self,
	LIMdlPose*              pose,
	int                     channel,
	int                     keep,
	const LIMdlPoseChannel* info);

static void private_channel_fade (
	LIRenObject* self,
	LIMdlPose*   pose,
	int          channel,
	float        time);

static void private_rebuild_skeleton (
	LIRenObject* self);

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
	self->skeleton_rebuild_needed = 0;
	self->render_distance = -1.0f;
	self->transform = limat_transform_identity ();
	self->pose = NULL;
	self->pose_skeleton = NULL;
	self->render = render;
	self->node = NULL;

	/* Choose a unique ID. */
	while (!self->id)
	{
		self->id = lialg_random_range (&render->random, 0x00000000, 0x7FFFFFFF);
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
	if (self->pose_skeleton != NULL)
		limdl_pose_skeleton_free (self->pose_skeleton);
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
	/* Create the new entity. */
	self->attachments.push_back (OGRE_NEW LIRenAttachmentEntity (self, model));
	self->skeleton_rebuild_needed = 1;
}

int liren_object_channel_animate (
	LIRenObject*            self,
	int                     channel,
	int                     keep,
	const LIMdlPoseChannel* info)
{
	LIMdlPose* pose1;

	/* Update the reference pose. */
	pose1 = private_channel_animate (self, self->pose, channel, keep, info);
	if (pose1 != NULL)
	{
		self->pose = pose1;
		if (self->pose_skeleton == NULL)
		{
			self->pose_skeleton = limdl_pose_skeleton_new (NULL, 0);
			private_rebuild_skeleton (self);
		}
	}

	return 1;
}

void liren_object_channel_fade (
	LIRenObject* self,
	int          channel,
	float        time)
{
	private_channel_fade (self, self->pose, channel, time);
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
 * \brief Clears all animations from the object.
 * \param self Object.
 */
void liren_object_clear_animations (
	LIRenObject* self)
{
	if (self->pose == NULL)
		return;
	limdl_pose_destroy_all (self->pose);
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
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
		OGRE_DELETE self->attachments[i];
	self->attachments.clear ();

	/* Remove the skeleton. */
	if (self->pose == NULL && self->pose_skeleton != NULL)
	{
		limdl_pose_skeleton_free (self->pose_skeleton);
		self->pose_skeleton = NULL;
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

	/* Search from the skeleton. */
	if (self->pose_skeleton != NULL)
		node = limdl_pose_skeleton_find_node (self->pose_skeleton, name);

	/* Search from attachments. */
	if (node == NULL)
	{
		for (size_t i = 0 ; i < self->attachments.size () ; i++)
		{
			node = self->attachments[i]->find_node (name);
			if (node != NULL)
				break;
		}
		if (node == NULL)
			return 0;
	}

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
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
	{
		if (self->attachments[i]->has_model (model))
		{
			LIRenAttachmentEntity* attachment = OGRE_NEW LIRenAttachmentEntity (self, model);
			self->attachments[i]->set_replacer (attachment);
			self->attachments.insert (self->attachments.begin () + i, attachment);
			i++;
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
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
	{
		if (self->attachments[i]->has_model (model))
		{
			private_remove_entity (self, i);
			self->skeleton_rebuild_needed = 1;
			break;
		}
	}
}

/**
 * \brief Replace a model with another.
 *
 * This performs a delayed replacement in such a way that the old model
 * won't be removed until the new model has been background loaded.
 *
 * \param self Object.
 * \param model_old Model to be removed.
 * \param model_new Model to be added.
 */
void liren_object_replace_model (
	LIRenObject* self,
	LIRenModel*  model_old,
	LIRenModel*  model_new)
{
	/* Try to replace. */
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
	{
		if (self->attachments[i]->has_model (model_old))
		{
			LIRenAttachmentEntity* attachment = OGRE_NEW LIRenAttachmentEntity (self, model_new);
			self->attachments[i]->set_replacer (attachment);
			self->attachments.insert (self->attachments.begin () + i, attachment);
			self->skeleton_rebuild_needed = 1;
			return;
		}
	}

	/* Just add if there was nothing to replace. */
	liren_object_add_model (self, model_new);
}

/**
 * \brief Replaces a texture.
 * \param self Object.
 * \param name Name of the replaced texture.
 * \param width Width of the new texture.
 * \param height Height of the new texture.
 * \param pixels Pixels in the RGBA format.
 */
void liren_object_replace_texture (
	LIRenObject* self,
	const char*  name,
	int          width,
	int          height,
	const void*  pixels)
{
	// Create the new texture.
	// FIXME: Why does the Ogre::PF_R8G8B8A8 format not work?
	Ogre::Image img;
	img.loadDynamicImage ((Ogre::uchar*) pixels, width, height, 1, Ogre::PF_A8B8G8R8);
	Ogre::String unique_name = self->render->data->id.next ();
	Ogre::TexturePtr texture = self->render->data->texture_manager->loadImage (unique_name, LIREN_RESOURCES_TEMPORARY, img);

	// Replace in all non-deprecated entities.
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
	{
		if (!self->attachments[i]->get_replacer ())
			self->attachments[i]->replace_texture (name, texture);
	}
}

void liren_object_update (
	LIRenObject* self,
	float        secs)
{
	/* Update attachments. */
	/* This needs to be done before replacing attachments since an
	   attachment may finish loading here. If replacing were done first,
	   both the replaced attachment and the replacement would be shown
	   during the same frame. */
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
		self->attachments[i]->update (secs);

	/* Replace old attachments with built ones. */
	/* Removing an attachment is a potentially recursive operation so there is no
	   guarantee of the list index being in any given position after the removal.
	   Because of that, the operation needs to be restarted from scratch after
	   each removal. */
	while (true)
	{
		bool found = false;
		for (size_t i = 0 ; i < self->attachments.size () ; i++)
		{
			LIRenAttachment* repl = self->attachments[i]->get_replacer ();
			if (repl != NULL && repl->is_loaded ())
			{
				self->skeleton_rebuild_needed = 1;
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

	/* Update attachment poses. */
	if (self->pose_skeleton != NULL)
	{
		limdl_pose_update (self->pose, secs);
		if (self->skeleton_rebuild_needed)
			private_rebuild_skeleton (self);
		limdl_pose_skeleton_update (self->pose_skeleton, self->pose);
		for (size_t i = 0 ; i < self->attachments.size () ; i++)
			self->attachments[i]->update_pose (self->pose_skeleton);
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
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
	{
		if (!self->attachments[i]->is_loaded ())
			return 0;
		if (self->attachments[i]->get_replacer () != NULL)
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

	/* Add the new model. */
	LIRenAttachmentEntity* attachment = OGRE_NEW LIRenAttachmentEntity (self, model);
	self->skeleton_rebuild_needed = 1;

	/* Mark all the old entities for replacement. */
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
		self->attachments[i]->set_replacer (attachment);
	self->attachments.push_back (attachment);

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

	/* Add the new attachment. */
	self->attachments.push_back (OGRE_NEW LIRenAttachmentParticle (self, name));

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
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
		self->attachments[i]->set_emitting (value);
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
	LIRenObject*            self,
	LIMdlPose*              pose,
	int                     channel,
	int                     keep,
	const LIMdlPoseChannel* info)
{
	/* Create the pose if it doesn't exist. */
	if (pose == NULL)
	{
		pose = limdl_pose_new ();
		if (pose == NULL)
			return NULL;
	}

	/* Clear the channel if NULL was given as the animation. */
	if (info == NULL)
	{
		if (keep)
			limdl_pose_fade_channel (pose, channel, LIMDL_POSE_FADE_AUTOMATIC);
		else
			limdl_pose_destroy_channel (pose, channel);
		return pose;
	}

	/* Merge the channel. */
	limdl_pose_merge_channel (pose, channel, keep, info);

	return pose;
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

static void private_rebuild_skeleton (
	LIRenObject* self)
{
	int count;
	LIMdlModel** models;

	/* Check if a skeleton exists. */
	self->skeleton_rebuild_needed = 0;
	if (self->pose_skeleton == NULL)
		return;

	/* Check for attachments. */
	if (!self->attachments.size ())
	{
		limdl_pose_skeleton_rebuild (self->pose_skeleton, NULL, 0);
		return;
	}

	/* Allocate space for models. */
	count = 0;
	models = (LIMdlModel**) lisys_calloc (self->attachments.size (), sizeof (LIMdlModel*));
	if (models == NULL)
		return;

	/* Add each model to the list. */
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
	{
		if (self->attachments[i]->get_replacer () == NULL)
		{
			LIMdlModel* m = self->attachments[i]->get_model ();
			if (m != NULL && m->nodes.count)
			{
				models[count] = m;
				count++;
			}
		}
	}

	/* Rebuild the skeleton. */
	limdl_pose_skeleton_rebuild (self->pose_skeleton, models, count);
	lisys_free (models);
}

static void private_remove_entity (
	LIRenObject* self,
	int          index)
{
	/* Remove from the list. */
	/* This must be done first to avoid potential double deletion. */
	LIRenAttachment* attachment = self->attachments[index];
	self->attachments.erase(self->attachments.begin () + index);

	/* Free attachments waiting for the removal of this one. */
	/* This is a potentially recursive operation so the list indices may change
	   wildly. To avoid out of bounds errors for sure, we restart the loop from
	   scratch immediately after removing something. */
	while (true)
	{
		bool found = false;
		for (size_t i = 0 ; i < self->attachments.size () ; i++)
		{
			if (attachment == self->attachments[i]->get_replacer ())
			{
				private_remove_entity (self, i);
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}

	/* Free the attachment. */
	OGRE_DELETE attachment;
}

static void private_update_entity_settings (
	LIRenObject* self)
{
	for (size_t i = 0 ; i < self->attachments.size () ; i++)
		self->attachments[i]->update_settings ();
}

/** @} */
/** @} */
/** @} */
