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

#ifndef __LIEXT_TILES_PHYSICS__PHYSICS_TERRAIN_RAYCAST_HOOK_HPP__
#define __LIEXT_TILES_PHYSICS__PHYSICS_TERRAIN_RAYCAST_HOOK_HPP__

#include "lipsofsuna/extension/physics/physics-private.h"
#include "lipsofsuna/extension/physics/physics-raycast-hook.hpp"
#include "physics-terrain.hpp"

class LIExtPhysicsVoxelRaycastHook : public LIPhyRaycastHook
{
public:
	LIExtPhysicsVoxelRaycastHook (LIPhyTerrain* terrain);
	virtual ~LIExtPhysicsVoxelRaycastHook ();
	virtual void rayTest (const btVector3& rayFromWorld, const btVector3& rayToWorld, btCollisionWorld::RayResultCallback& resultCallback) const;
private:
	LIPhyTerrain* terrain;
};

#endif