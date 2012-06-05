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
	render_model = model;
	replacing_entity = NULL;

	/* Clear the pose buffer. */
	pose_buffer = NULL;
	pose_changed = false;

	/* A placeholder mesh is assigned to the entity until the real mesh is
	   fully loaded. This is because Ogre assumes that a mesh always exists,
	   and it won't wait for us to background load the textures if the real
	   mesh is assigned. */
	mName = name;
	mMesh = Ogre::MeshPtr (new LIRenMesh ());
	background_loaded_mesh = model->mesh;

	/* Start building the mesh. */
	/* NOTE: It is possible that the mesh gets built inside this call already.
	   Because of that, all members need to be initialized before calling this. */
	builder.start ();
}

LIRenEntity::~LIRenEntity ()
{
	if (pose_buffer != NULL)
		limdl_pose_buffer_free (pose_buffer);
}

void LIRenEntity::initialize ()
{
	/* The entity builder may call this multiple times because Ogre
	   may generate duplicate events when resources are loaded. The
	   duplicates are filtered here because it's the most convenient. */
	if (mInitialised)
		return;

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

	/* Create the pose buffer. */
	if (skeleton != NULL)
	{
		lisys_assert (pose_buffer == NULL);
		pose_buffer = limdl_pose_buffer_new (get_model ());
		pose_changed = true;
	}
}

/**
 * \brief Updates the CPU side pose transformation.
 *
 * The pose tranformation is always recalculated for each frame. However, the
 * pose buffers are only calculated for objects that need to be rendered.
 * Hence, this function only set the pose_changed flag.
 *
 * \param pose Pose whose transform to copy.
 * \param secs Seconds since the last update.
 */
void LIRenEntity::update_pose (LIMdlPose* pose, float secs)
{
	if (pose_buffer != NULL)
	{
		limdl_pose_buffer_update (pose_buffer, pose);
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
	/* Get the skeleton. */
	/* If the model doesn't have one, we don't need to do anything. */
	Ogre::SkeletonInstance* skeleton = getSkeleton ();
	if (skeleton == NULL)
		return;
	lisys_assert (pose_buffer != NULL);

	/* Update bones. */
	for (int i = 0 ; i < pose_buffer->bones.count ; i++)
	{
		LIMdlPoseBufferBone* src_bone = pose_buffer->bones.array + i;
		Ogre::Bone* dst_bone = skeleton->getBone (i);
		LIMatTransform t = src_bone->transform;
		LIMatVector s = src_bone->scale;
		dst_bone->setScale (s.x, s.y, s.z);
		dst_bone->setPosition (t.position.x, t.position.y, t.position.z);
		dst_bone->setOrientation (t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z);
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

LIMdlPoseBuffer* LIRenEntity::get_pose_buffer ()
{
	return pose_buffer;
}

LIRenModel* LIRenEntity::get_render_model ()
{
	return render_model;
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
	/* Make sure that the entity is initialized. */
	/* Ogre may or may not check for this, but we do it just in case. */
	if (!mInitialised)
		return;
	lisys_assert (!mMesh.isNull ());

	/* Update the animation. */
	if (this->pose_changed)
	{
		Ogre::Entity* displayEntity = this;
		if (mMeshLodIndex > 0 && mMesh->isLodManual ())
			displayEntity = mLodEntityList[mMeshLodIndex - 1];
		if (displayEntity->hasSkeleton ())
		{
			this->pose_changed = false;
			if (displayEntity->hasSkeleton ())
				update_pose_buffer ();
		}
	}

	/* Update the render queue. */
	Ogre::Entity::_updateRenderQueue (queue);
}

/** @} */
/** @} */
/** @} */
