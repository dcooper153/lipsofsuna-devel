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

#ifndef __EXT_HEIGHTMAP_PHYSICS_HEIGHTFIELD_TERRAIN_SHAPE_H__
#define __EXT_HEIGHTMAP_PHYSICS_HEIGHTFIELD_TERRAIN_SHAPE_H__

#include "lipsofsuna/extension.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

class LIExtHeightfieldTerrainShape : public btHeightfieldTerrainShape
{
public:
	LIExtHeightfieldTerrainShape (
		int heightStickWidth, int heightStickLength, void* heightfieldData,
		btScalar heightScale, btScalar minHeight, btScalar maxHeight,int upAxis,
		PHY_ScalarType hdt, bool flipQuadEdges);
	virtual void processAllTriangles(btTriangleCallback* callback,const btVector3& aabbMin,const btVector3& aabbMax) const;
};

#endif
