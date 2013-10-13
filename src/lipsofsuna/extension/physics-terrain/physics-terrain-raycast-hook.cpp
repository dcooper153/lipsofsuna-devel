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

#include "physics-terrain.hpp"
#include "physics-terrain-raycast-hook.hpp"

LIExtPhysicsTerrainRaycastHook::LIExtPhysicsTerrainRaycastHook (LIExtPhysicsTerrain* terrain)
{
	this->terrain = terrain;
}

LIExtPhysicsTerrainRaycastHook::~LIExtPhysicsTerrainRaycastHook ()
{
}

void LIExtPhysicsTerrainRaycastHook::rayTest (const btVector3& rayFromWorld, const btVector3& rayToWorld, btCollisionWorld::RayResultCallback& resultCallback) const
{
	int grid_x;
	int grid_z;
	float fraction;
	LIMatVector start;
	LIMatVector end;
	LIMatVector normal;
	LIMatVector point;

	/* Check that the terrain exists. */
	if (terrain->terrain == NULL)
		return;

	/* Check for the collision mask. */
	if (!terrain->realized)
		return;
	if (!(terrain->collision_group & resultCallback.m_collisionFilterMask) ||
	    !(terrain->collision_mask & resultCallback.m_collisionFilterGroup))
		return;

	/* Cast the ray against the terrain. */
	start = limat_vector_init (rayFromWorld[0], rayFromWorld[1], rayFromWorld[2]);
	end = limat_vector_init (rayToWorld[0], rayToWorld[1], rayToWorld[2]);
	if (liext_terrain_intersect_ray (terrain->terrain, &start, &end, &grid_x, &grid_z, &point, &normal, &fraction))
	{
		if (fraction < resultCallback.m_closestHitFraction)
		{
			btVector3 normal1 (normal.x, normal.y, normal.z);
			terrain->pointer->tile[0] = grid_x;
			terrain->pointer->tile[1] = 0;
			terrain->pointer->tile[2] = grid_z;
			btCollisionWorld::LocalRayResult rayres (terrain->object, NULL, normal1, fraction);
			resultCallback.addSingleResult (rayres, true);
		}
	}
}

void LIExtPhysicsTerrainRaycastHook::sphereSweepTest (float radius, const btVector3& rayFromWorld, const btVector3& rayToWorld, btCollisionWorld::ConvexResultCallback& resultCallback)
{
	LIMatVector start;
	LIMatVector end;
	LIExtTerrainCollision result;

	/* Check that the terrain exists. */
	if (terrain->terrain == NULL)
		return;

	/* Check for the collision mask. */
	if (!terrain->realized)
		return;
	if (!(terrain->collision_group & resultCallback.m_collisionFilterMask) ||
	    !(terrain->collision_mask & resultCallback.m_collisionFilterGroup))
		return;

	/* Cast the sphere against the terrain. */
	start = limat_vector_init (rayFromWorld[0], rayFromWorld[1], rayFromWorld[2]);
	end = limat_vector_init (rayToWorld[0], rayToWorld[1], rayToWorld[2]);
	if (liext_terrain_cast_sphere (terrain->terrain, &start, &end, radius, &result))
	{
		if (result.fraction < resultCallback.m_closestHitFraction)
		{
			btVector3 point (result.point.x, result.point.y, result.point.z);
			btVector3 normal (result.normal.x, result.normal.y, result.normal.z);
			btCollisionWorld::LocalConvexResult conres (terrain->object, NULL, point, normal, result.fraction);
			resultCallback.addSingleResult (conres, true);
		}
	}
}
