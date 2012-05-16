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

#include "render-types.h"
#include "render-entity.hpp"
#include "render-mesh.hpp"
#include <OgreSubMesh.h>

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenEntity Entity
 * @{
 */

LIRenEntity::LIRenEntity (const Ogre::String& name, LIRenModel* model) :
	Ogre::Entity (), builder (this, model->mesh)
{
	this->model = model;
	replacing_entity = NULL;

	/* A placeholder mesh is assigned to the entity until the real mesh is
	   fully loaded. This is because Ogre assumes that a mesh always exists,
	   and it won't wait for us to background load the textures if the real
	   mesh is assigned. */
	mName = name;
	mMesh = Ogre::MeshPtr (new LIRenMesh ());
	background_loaded_mesh = model->mesh;
	builder.start ();

	/* Clear the pose. */
	pose = NULL;
	pose_changed = false;
}

LIRenEntity::~LIRenEntity ()
{
	if (pose != NULL)
		limdl_pose_free (pose);
}

void LIRenEntity::initialize ()
{
	/* Create the mesh and skeleton instances. */
	mMesh = background_loaded_mesh;
	_initialise ();

	/* Mark all bones as manually controlled. */
	Ogre::SkeletonInstance* skeleton = getSkeleton ();
	if (skeleton != NULL)
	{
		for (int i = 0 ; i < skeleton->getNumBones () ; i++)
		{
			Ogre::Bone* bone = skeleton->getBone (i);
			bone->setManuallyControlled (true);
		}
	}
}

/**
 * \brief Updates the CPU side pose transformation.
 *
 * The pose tranformation is always recalculated for each frame. However, the
 * pose buffers are only calculated for objects that need to be rendered.
 * Hence, this function only set the pose_changed flag.
 *
 * \param secs Seconds since the last update.
 */
void LIRenEntity::update_pose (float secs)
{
	if (pose != NULL)
	{
		limdl_pose_update (pose, secs);
		pose_changed = true;
	}
}

/**
 * \brief Rebuilds the pose buffer and uploads it to the GPU.
 *
 * To reduce load when there are lots of animated objects, entities only
 * update their pose buffers when they are rendered. In addition, the buffers
 * are updated at most once per frame.
 */
void LIRenEntity::update_pose_buffer ()
{
	LIMdlModel* model;

	/* Get the skeleton. */
	/* If the model doesn't have one, we don't need to do anything. */
	Ogre::SkeletonInstance* skeleton = getSkeleton ();
	if (skeleton == NULL)
		return;

	/* Get the model. */
	model = get_model ();
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
			LIMdlNode* node = limdl_pose_find_node (this->pose, group->node->name);
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

bool LIRenEntity::get_loaded () const
{
	return mInitialised;
}

LIMdlModel* LIRenEntity::get_model () const
{
	if (background_loaded_mesh.isNull ())
		return NULL;
	return static_cast<LIRenMesh*>(background_loaded_mesh.get ())->get_model ();
}

void LIRenEntity::set_pose (LIMdlPose* pose)
{
	this->pose = limdl_pose_new_copy (pose);
	this->pose_changed = true;
	limdl_pose_set_model (this->pose, get_model ());
}

LIMdlPose* LIRenEntity::get_pose ()
{
	return pose;
}

LIRenModel* LIRenEntity::get_render_model ()
{
	return model;
}

LIRenEntity* LIRenEntity::get_replacing_entity ()
{
	return this->replacing_entity;
}

void LIRenEntity::set_replacing_entity (LIRenEntity* entity)
{
	this->replacing_entity = entity;
}

void LIRenEntity::_updateRenderQueue (Ogre::RenderQueue* queue)
{
	/* Update the render queue. */
	Ogre::Entity::_updateRenderQueue (queue);

	/* Update the animation. */
	if (this->pose_changed)
	{
		Ogre::Entity* displayEntity = this;
		if (mMeshLodIndex > 0 && mMesh->isLodManual ())
			displayEntity = mLodEntityList[mMeshLodIndex - 1];
		if (mInitialised && displayEntity->hasSkeleton ())
		{
			this->pose_changed = false;
			if (displayEntity->hasSkeleton ())
				update_pose_buffer ();
		}
	}
}

/** @} */
/** @} */
/** @} */
