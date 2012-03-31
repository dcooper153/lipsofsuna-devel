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

LIRenEntity::LIRenEntity (const Ogre::String& name, const Ogre::MeshPtr& mesh) :
	Ogre::Entity (), builder (this, mesh)
{
	/* A placeholder mesh is assigned to the entity until the real mesh is
	   fully loaded. This is because Ogre assumes that a mesh always exists,
	   and it won't wait for us to background load the textures if the real
	   mesh is assigned. */
	mName = name;
	mMesh = Ogre::MeshPtr (new LIRenMesh ());
	background_loaded_mesh = mesh;
	builder.start ();
}

LIRenEntity::~LIRenEntity ()
{
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

LIMdlModel* LIRenEntity::get_model () const
{
	if (background_loaded_mesh.isNull ())
		return NULL;
	return static_cast<LIRenMesh*>(background_loaded_mesh.get ())->get_model ();
}

/** @} */
/** @} */
/** @} */
