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

#include "ext-module.h"
#include "physics-terrain-raycast-hook.hpp"

LIExtPhysicsVoxelRaycastHook::LIExtPhysicsVoxelRaycastHook (LIPhyTerrain* terrain)
{
	this->terrain = terrain;
}

LIExtPhysicsVoxelRaycastHook::~LIExtPhysicsVoxelRaycastHook ()
{
}

void LIExtPhysicsVoxelRaycastHook::rayTest (const btVector3& rayFromWorld, const btVector3& rayToWorld, btCollisionWorld::RayResultCallback& resultCallback) const
{
	LIMatVector start;
	LIMatVector end;
	LIPhyContact result;

	/* Check for the collision mask. */
	if (!(terrain->collision_group & resultCallback.m_collisionFilterMask) ||
	    !(terrain->collision_mask & resultCallback.m_collisionFilterGroup))
		return;

	/* Cast the ray against the terrain. */
	start = limat_vector_init (rayFromWorld[0], rayFromWorld[1], rayFromWorld[2]);
	end = limat_vector_init (rayToWorld[0], rayToWorld[1], rayToWorld[2]);
	if (liphy_terrain_cast_ray (terrain, &start, &end, &result))
	{
		if (result.fraction < resultCallback.m_closestHitFraction)
		{
			btVector3 normal (result.normal.x, result.normal.y, result.normal.z);
			LIPhyTerrain* terrain = (LIPhyTerrain*) this->terrain;
			LIPhyPointer* pointer = (LIPhyPointer*) terrain->object->getUserPointer ();
			pointer->tile[0] = result.terrain_tile[0];
			pointer->tile[1] = result.terrain_tile[1];
			pointer->tile[2] = result.terrain_tile[2];
			btCollisionWorld::LocalRayResult rayres (terrain->object, NULL, normal, result.fraction);
			resultCallback.addSingleResult (rayres, true);
		}
	}
}
