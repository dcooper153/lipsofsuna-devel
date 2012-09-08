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

#ifndef __PHYSICS_CONVEX_RESULT_CALLBACK_HPP__
#define __PHYSICS_CONVEX_RESULT_CALLBACK_HPP__

#include "physics-contact.h"
#include <btBulletCollisionCommon.h>

class LIPhyConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	LIPhyConvexResultCallback (LIPhyObject** ignore_array, int ignore_count);
	virtual btScalar addSingleResult (btCollisionWorld::LocalConvexResult& result, bool world);
public:
	int ignore_count;
	LIPhyObject** ignore_array;
	LIPhyContact result;
};

#endif
