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
 * \addtogroup LIRenAttachmentEntity AttachmentEntity
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.hpp"
#include "render-attachment-entity.hpp"
#include "render-model.hpp"
#include "render-object.hpp"
#include <OgreSubEntity.h>
#include <OgreSubMesh.h>
#include <OgreResourceBackgroundQueue.h>
#include <OgreSkeletonManager.h>

#define LIREN_BACKGROUND_LOADING 1

LIRenAttachmentEntity::LIRenAttachmentEntity (LIRenObject* object, LIRenModel* model) :
	LIRenAttachment (object)
{
	lisys_assert (model != NULL);
	lisys_assert (!model->mesh.isNull ());

	this->loaded = false;
	this->loading_mesh = false;
	this->loading_deps = false;
	this->pose_buffer = NULL;
	this->entity = NULL;
	this->model = model;
	this->mesh = model->mesh;
}

LIRenAttachmentEntity::~LIRenAttachmentEntity ()
{
	if (pose_buffer != NULL)
		limdl_pose_buffer_free (pose_buffer);

	if (entity != NULL)
	{
		object->node->detachObject (entity);
		render->scene_manager->destroyEntity (entity);
	}
}

LIMdlNode* LIRenAttachmentEntity::find_node (const char* name)
{
	LIMdlNode* node;

	if (entity == NULL)
		return NULL;

	LIMdlModel* model = get_model ();
	if (model != NULL)
		node = limdl_model_find_node (model, name);
	else
		node = NULL;

	return node;
}

LIMdlModel* LIRenAttachmentEntity::get_model () const
{
	if (mesh.isNull ())
		return NULL;

	LIRenMeshBuilder* builder = (LIRenMeshBuilder*) lialg_strdic_find (
		render->mesh_builders, mesh->getName ().c_str ());
	if (builder == NULL)
		return NULL;

	return builder->get_model ();
}

bool LIRenAttachmentEntity::has_model (LIRenModel* model)
{
	return this->model == model;
}

bool LIRenAttachmentEntity::is_loaded () const
{
	return loaded;
}

void LIRenAttachmentEntity::remove_model (LIRenModel* model)
{
	if (model != this->model)
		return;
	this->model = NULL;

	if (pose_buffer != NULL)
	{
		limdl_pose_buffer_free (pose_buffer);
		pose_buffer = NULL;
	}

	if (entity != NULL)
	{
		object->node->detachObject (entity);
		render->scene_manager->destroyEntity (entity);
		entity = NULL;
	}

	resources.clear ();
	loading_mesh = false;
	loading_deps = false;
	loaded = true;
}

void LIRenAttachmentEntity::replace_texture (const char* name, Ogre::TexturePtr& texture)
{
	// TODO: Queue if not loaded yet.
	if (!loaded)
		return;

	if (mesh.isNull () || entity == NULL)
		return;

	for (size_t subent_idx = 0 ; subent_idx < entity->getNumSubEntities () ; ++subent_idx)
	{
		// Get the material of the subent.
		Ogre::SubEntity* subent = entity->getSubEntity (subent_idx);
		Ogre::MaterialPtr submat = subent->getMaterial ();
		if (submat.isNull ())
			continue;

		// Check if there are replaceable textures.
		if (!render->material_utils->has_overridable_texture (submat, name))
			continue;

		// Create a modified version of the material.
		Ogre::String new_name = render->id.next ();
		Ogre::MaterialPtr material = submat->clone (new_name, true, LIREN_RESOURCES_TEMPORARY);
		render->material_utils->replace_texture (material, name, texture->getName ());
		subent->setMaterial (material);
	}
}

void LIRenAttachmentEntity::update (float secs)
{
	/* Only needed when background loading dependencies. */
	if (loaded)
		return;
	if (entity != NULL)
	{
		loaded = true;
		return;
	}

	/* Wait for the mesh to load. */
	if (!mesh->isLoaded ())
	{
		if (loading_mesh)
			return;
		loading_mesh = true;
#ifdef LIREN_BACKGROUND_LOADING
		Ogre::ResourceBackgroundQueue::getSingleton ().load (
			"Mesh", mesh->getName (), mesh->getGroup ());
#else
		mesh->load ();
#endif
		return;
	}

	/* Start loading dependencies. */
	if (!loading_deps)
	{
		loading_deps = true;
		for (size_t i = 0 ; i < mesh->getNumSubMeshes () ; i++)
		{
			Ogre::SubMesh* sub = mesh->getSubMesh (i);
			if (sub->isMatInitialised ())
			{
				Ogre::MaterialManager& mgr = Ogre::MaterialManager::getSingleton ();
				Ogre::MaterialPtr material = mgr.getByName (sub->getMaterialName (), mesh->getGroup ());
				if (!material.isNull ())
				{
					resources.push_back (material);
#ifdef LIREN_BACKGROUND_LOADING
					Ogre::ResourceBackgroundQueue::getSingleton ().load (
						"Material", material->getName (), material->getGroup ());
#else
					material->load (true);
#endif
				}
			}
		}
		return;
	}

	/* Wait for the dependencies to load. */
	for (size_t i = 0 ; i < resources.size () ; i++)
	{
		Ogre::ResourcePtr& resource = resources[i];
		if (!resource->isLoaded ())
			return;
	}

	/* Create the entity. */
	Ogre::String e_name = render->id.next ();
	entity = render->scene_manager->createEntity (e_name, mesh->getName (), LIREN_RESOURCES_TEMPORARY);
	object->node->attachObject (entity);

	/* Create the skeleton and its pose buffer. */
	if (create_skeleton ())
	{
		LIMdlModel* model = get_model ();
		lisys_assert (pose_buffer == NULL);
		if (model != NULL)
		{
			pose_buffer = limdl_pose_buffer_new (model);
			lisys_assert (pose_buffer != NULL);
			lisys_assert (pose_buffer->bones.count == entity->getSkeleton ()->getNumBones ());
		}
	}

	/* Set the entity flags. */
	entity->setCastShadows (object->get_shadow_casting ());

	/* Set entity visibility. */
	/* If a visible entity is added to a hidden scene node, the entity is
	   still rendered. Hence, newly added entities needs to be explicitly
	   hidden or Ogre will render our invisible objects. */
	entity->setVisible (object->get_visible ());

	/* Clear the now useless dependency list. */
	resources.clear ();
	loading_mesh = false;
	loading_deps = false;
}

/**
 * \brief Updates the CPU side pose transformation.
 *
 * The pose tranformation is always recalculated for each frame. However, the
 * pose buffers are only calculated for objects that need to be rendered.
 * Hence, this function only set the pose_changed flag.
 *
 * \param skeleton Skeleton whose transform to copy.
 */
void LIRenAttachmentEntity::update_pose (LIMdlPoseSkeleton* skeleton)
{
	if (entity == NULL || pose_buffer == NULL)
		return;

	/* Update the pose buffer. */
	limdl_pose_buffer_update (pose_buffer, skeleton);

	/* Get the skeleton. */
	/* If the model doesn't have one, we don't need to do anything. */
	Ogre::SkeletonInstance* ogre_skeleton = entity->getSkeleton ();
	lisys_assert (ogre_skeleton != NULL);
	lisys_assert (pose_buffer->bones.count == ogre_skeleton->getNumBones ());

	/* Update bones. */
	for (int i = 0 ; i < pose_buffer->bones.count ; i++)
	{
		LIMdlPoseBufferBone* src_bone = pose_buffer->bones.array + i;
		Ogre::Bone* dst_bone = ogre_skeleton->getBone (i);
		LIMatTransform t = src_bone->transform;
		LIMatVector s = src_bone->scale;
		dst_bone->setScale (s.x, s.y, s.z);
		dst_bone->setPosition (t.position.x, t.position.y, t.position.z);
		dst_bone->setOrientation (t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z);
	}

	/* Queue a skeleton update. */
	ogre_skeleton->_notifyManualBonesDirty ();
}

void LIRenAttachmentEntity::update_settings ()
{
	if (entity == NULL)
		return;

	entity->setCastShadows (object->get_shadow_casting ());
}

bool LIRenAttachmentEntity::create_skeleton ()
{
	Ogre::SkeletonInstance* skeleton = entity->getSkeleton ();
	if (skeleton == NULL)
		return false;

	/* Set the initial bone transformations. */
	/* The mesh may not have set these correctly if it depended on bones
	   in external skeletons. Because of external bones, we need to set
	   the transformations using the pose skeleton of the object. */
	LIMdlModel* model = get_model ();
	if (object->pose_skeleton != NULL && model != NULL)
	{
		for (int i = 0 ; i < model->weight_groups.count ; i++)
		{
			Ogre::Bone* bone = skeleton->getBone (i + 1);
			LIMdlWeightGroup* group = model->weight_groups.array + i;
			LIMdlNode* node = limdl_pose_skeleton_find_node (object->pose_skeleton, group->bone);
			if (node != NULL)
			{
				LIMatTransform t = node->rest_transform.global;
				bone->setPosition (t.position.x, t.position.y, t.position.z);
				bone->setOrientation (t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z);
			}
		}
	}

	/* Make all bones manually controlled. */
	for (int i = 0 ; i < skeleton->getNumBones () ; i++)
	{
		Ogre::Bone* bone = skeleton->getBone (i);
		bone->setManuallyControlled (true);
	}

	/* Set the binding pose. */
	skeleton->setBindingPose ();

	return true;
}

/** @} */
/** @} */
/** @} */
