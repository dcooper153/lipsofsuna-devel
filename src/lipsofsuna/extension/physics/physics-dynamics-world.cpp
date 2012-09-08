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

#include "physics-dynamics-world.hpp"
#include "physics-private.h"

LIPhyDynamicsWorld::LIPhyDynamicsWorld (btDispatcher* dispatcher, btBroadphaseInterface* pairCache, btConstraintSolver* constraintSolver, btCollisionConfiguration* collisionConfiguration) :
	btDiscreteDynamicsWorld (dispatcher, pairCache, constraintSolver, collisionConfiguration)
{
	this->raycast_hooks = NULL;
	this->setGravity (btVector3 (0.0f, -10.0f, 0.0f));
}

LIPhyDynamicsWorld::~LIPhyDynamicsWorld ()
{
	lisys_assert (raycast_hooks == NULL);
}

void LIPhyDynamicsWorld::addCollisionObject (btCollisionObject* collisionObject, short int collisionFilterGroup, short int collisionFilterMask)
{
	btDiscreteDynamicsWorld::addCollisionObject (collisionObject, collisionFilterGroup, collisionFilterMask);
}

void LIPhyDynamicsWorld::rayTest (const btVector3& rayFromWorld, const btVector3& rayToWorld, RayResultCallback& resultCallback) const
{
	LIPhyRaycastHook* hook;

	/* Test against normal objects. */
	btDiscreteDynamicsWorld::rayTest (rayFromWorld, rayToWorld, resultCallback);

	/* Test against raycast hooks. */
	for (hook = raycast_hooks ; hook != NULL ; hook = hook->next)
	{
		hook->rayTest (rayFromWorld, rayToWorld, resultCallback);
	}
}

void LIPhyDynamicsWorld::removeCollisionObject (btCollisionObject* collisionObject)
{
	btDiscreteDynamicsWorld::removeCollisionObject (collisionObject);
}
