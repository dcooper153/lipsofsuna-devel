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

#ifndef __RENDER_INTERNAL_RESOURCE_LOADING_LISTENER_HPP__
#define __RENDER_INTERNAL_RESOURCE_LOADING_LISTENER_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/paths.h"
#include <OgreResourceGroupManager.h>

class LIRenResourceLoadingListener : public Ogre::ResourceLoadingListener
{
public:
	LIRenResourceLoadingListener (LIPthPaths* paths);
	virtual Ogre::DataStreamPtr resourceLoading (const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource);
	virtual void resourceStreamOpened (const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource, Ogre::DataStreamPtr& dataStream);
	virtual bool resourceCollision (Ogre::Resource* resource, Ogre::ResourceManager* resourceManager);
protected:
	LIPthPaths* paths;
};

#endif
