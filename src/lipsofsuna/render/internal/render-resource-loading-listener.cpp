/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#include "lipsofsuna/system.h"
#include "lipsofsuna/paths.h"
#include "render.hpp"
#include "render-resource-loading-listener.hpp"
#include <OgreResourceGroupManager.h>

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenResourceLoadingListener ResourceLoadingListener
 * @{
 */

LIRenResourceLoadingListener::LIRenResourceLoadingListener (LIPthPaths* paths) : paths (paths)
{
}

Ogre::DataStreamPtr LIRenResourceLoadingListener::resourceLoading (const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource)
{
	/* Use the file lookup system the rest of the game uses to ensure proper
	   overriding. With the order that was used for loading scripts, the
	   priorities of images and other non-script data would be inverted. */
	if (group == LIREN_RESOURCES_PERMANENT)
	{
		const char* path = lipth_paths_find_file (paths, name.c_str ());
		if (path != NULL)
		{
			FILE* file = fopen (path, "rb");
			if (file != NULL)
				return Ogre::DataStreamPtr (OGRE_NEW Ogre::FileHandleDataStream (file, true));
		}
	}
	return Ogre::DataStreamPtr ();
}

void LIRenResourceLoadingListener::resourceStreamOpened (const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource, Ogre::DataStreamPtr& dataStream)
{
}

bool LIRenResourceLoadingListener::resourceCollision (Ogre::Resource* resource, Ogre::ResourceManager* resourceManager)
{
	/* Replace old resources. */
	/* This occurs when loading a script that overrides a previously defined script.
	   Scripts are always loaded before any models that could use them so we can
	   safely remove the old one. */
	resourceManager->remove (resource->getName ());
	return true;
}

/** @} */
/** @} */
/** @} */
