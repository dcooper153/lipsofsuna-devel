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

#include "physics-private.h"
#include "physics-object.h"
#include "physics-convex-result-callback.hpp"

LIPhyConvexResultCallback::LIPhyConvexResultCallback (LIPhyObject** ignore_array, int ignore_count) :
	btCollisionWorld::ClosestConvexResultCallback (btVector3 (0.0, 0.0, 0.0), btVector3 (0.0, 0.0, 0.0))
{
	m_closestHitFraction = 1.0f;
	this->ignore_count = ignore_count;
	this->ignore_array = ignore_array;
	liphy_contact_init (&result);
}

btScalar LIPhyConvexResultCallback::addSingleResult (btCollisionWorld::LocalConvexResult& result, bool world)
{
	int i;
	bool ret;
	LIPhyObject* object;
	LIPhyPointer* pointer = (LIPhyPointer*) result.m_hitCollisionObject->getUserPointer ();

	if (pointer == NULL)
	{
		/* Heightmap. */
		ret = ClosestConvexResultCallback::addSingleResult (result, world);
		liphy_contact_init (&this->result);
	}
	else if (pointer->type == LIPHY_POINTER_TYPE_OBJECT)
	{
		/* Object. */
		object = (LIPhyObject*) pointer->pointer;
		for (i = 0 ; i < ignore_count ; i++)
		{
			if (object == ignore_array[i])
				return 1.0;
		}
		ret = ClosestConvexResultCallback::addSingleResult (result, world);
		liphy_contact_init (&this->result);
		this->result.object_id = liphy_object_get_external_id (object);
	}
	else
	{
		/* Terrain. */
		ret = ClosestConvexResultCallback::addSingleResult (result, world);
		liphy_contact_init (&this->result);
		this->result.terrain_id = pointer->id;
		this->result.terrain_tile[0] = pointer->tile[0];
		this->result.terrain_tile[1] = pointer->tile[1];
		this->result.terrain_tile[2] = pointer->tile[2];
	}
	this->result.fraction = m_closestHitFraction;
	this->result.point = limat_vector_init (m_hitPointWorld[0], m_hitPointWorld[1], m_hitPointWorld[2]);
	this->result.normal = limat_vector_init (m_hitNormalWorld[0], m_hitNormalWorld[1], m_hitNormalWorld[2]);

	return ret;
}
