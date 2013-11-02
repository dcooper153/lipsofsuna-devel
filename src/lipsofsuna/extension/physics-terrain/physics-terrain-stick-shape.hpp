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

#ifndef __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_STICK_SHAPE_HPP__
#define __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_STICK_SHAPE_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/extension/physics/physics-private.h"
#include <btBulletCollisionCommon.h>

class LIExtPhysicsTerrainStickShape : public btConvexHullShape
{
public:
	LIExtPhysicsTerrainStickShape(int x, int z, const btVector3* points, int count) :
		btConvexHullShape ((const btScalar*) points, count, sizeof (btVector3)), x(x), z(z)
	{
	}
public:
	int x;
	int z;
};

#endif
