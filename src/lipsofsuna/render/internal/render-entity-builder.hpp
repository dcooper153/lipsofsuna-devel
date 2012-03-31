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

#ifndef __RENDER_INTERNAL_ENTITY_BUILDER_HPP__
#define __RENDER_INTERNAL_ENTITY_BUILDER_HPP__

#include "lipsofsuna/system.h"
#include "render-types.h"
#include <OgreEntity.h>
#include <OgreResource.h>

class LIRenEntity;

class LIRenEntityBuilder : public Ogre::Resource::Listener
{
public:
	LIRenEntityBuilder (LIRenEntity* entity, const Ogre::MeshPtr& mesh);
	virtual ~LIRenEntityBuilder ();
	void abort ();
	void start ();
public:
	virtual void backgroundLoadingComplete (Ogre::Resource* resource);
protected:
	LIRenEntity* entity;
	std::vector<bool> listening;
	std::vector<Ogre::ResourcePtr> resources;
};

#endif
