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

#ifndef __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_HPP__
#define __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/extension/physics/physics-private.h"
#include "physics-terrain.h"
#include "physics-terrain-raycast-hook.hpp"
#include <btBulletCollisionCommon.h>

class LIExtPhysicsTerrainShape;

struct _LIExtPhysicsTerrain
{
	int collision_group;
	int collision_mask;
	int realized;
	btCollisionObject* object;
	LIPhyPointer* pointer;
	LIExtPhysicsTerrainModule* module;
	LIExtPhysicsTerrainShape* shape;
	LIPhyPhysics* physics;
	LIExtTerrain* terrain;
	LIExtPhysicsTerrainRaycastHook* raycast_hook;
};

class LIExtPhysicsTerrainShape : public btBoxShape
{
public:
	LIExtPhysicsTerrainShape (LIExtPhysicsTerrain* t) : btBoxShape (btVector3 (10000.0f, 10000.0f, 10000.0f)), terrain (t)
	{
		this->m_shapeType = CUSTOM_CONVEX_SHAPE_TYPE;
	}
	virtual const char* getName () const
	{
		return "Terrain";
	}
	LIExtPhysicsTerrain* terrain;
};

#endif
