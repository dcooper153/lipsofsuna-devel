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
 * \addtogroup LIRenEntityBuilder EntityBuilder
 * @{
 */

#include "render-types.h"
#include "render-entity.hpp"
#include "render-entity-builder.hpp"
#include <OgreSubMesh.h>
#include <OgreResourceBackgroundQueue.h>

#define LIREN_BACKGROUND_LOADING 1

LIRenEntityBuilder::LIRenEntityBuilder (LIRenEntity* entity, const Ogre::MeshPtr& mesh)
{
	this->entity = entity;
	listening.push_back (false);
	resources.push_back (mesh);
}

LIRenEntityBuilder::~LIRenEntityBuilder ()
{
	abort ();
}

void LIRenEntityBuilder::abort ()
{
#ifdef LIREN_BACKGROUND_LOADING
	for (size_t i = 0 ; i < resources.size () ; i++)
	{
		if (listening[i])
		{
			Ogre::ResourcePtr& resource = resources[i];
			resource->removeListener (this);
		}
	}
#endif
	resources.clear ();
	listening.clear ();
}

void LIRenEntityBuilder::start ()
{
	Ogre::MeshPtr mesh = resources[0];

	if (!mesh->isLoaded ())
	{
#ifdef LIREN_BACKGROUND_LOADING
		listening[0] = true;
		mesh->addListener (this);
		Ogre::ResourceBackgroundQueue::getSingleton ().load (
			"LIRenMesh", mesh->getName (), mesh->getGroup ());
#else
		mesh->load ();
		backgroundLoadingComplete (mesh.get ());
#endif
	}
	else
		backgroundLoadingComplete (mesh.get ());
}

void LIRenEntityBuilder::backgroundLoadingComplete (Ogre::Resource* resource)
{
	Ogre::MeshPtr mesh = resources[0];

	/* Add materials to the list of needed resources. */
	if (resource == mesh.get ())
	{
		for (size_t i = 0 ; i < mesh->getNumSubMeshes () ; i++)
		{
			Ogre::SubMesh* sub = mesh->getSubMesh (i);
			if (sub->isMatInitialised ())
			{
				Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton ()
					.getByName (sub->getMaterialName (), mesh->getGroup ());
				listening.push_back (false);
				resources.push_back (material);
			}
		}
	}

	/* Background load one missing resource. */
	for (size_t i = 0 ; i < resources.size () ; i++)
	{
		Ogre::ResourcePtr& resource = resources[i];
		if (!resource->isLoaded ())
		{
#ifdef LIREN_BACKGROUND_LOADING
			if (!listening[i])
			{
				listening[i] = true;
				resource->addListener (this);
				Ogre::ResourceBackgroundQueue::getSingleton ().load (
					"Material", resource->getName (), resource->getGroup ());
			}
			return;
#else
			resource->load (true);
#endif
		}
	}

	/* Initialize the entity when all resources have been loaded. */
	entity->initialize ();
}

/** @} */
/** @} */
/** @} */
