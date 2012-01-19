/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/* Adapted for Lips of Suna
 *
 * The modifications are based on this thread from Ogre forums:
 * http://www.ogre3d.org/forums/viewtopic.php?f=2&t=58756&p=402559
 *
 * Ogre and Bullet use different triangulation schemes, and the difference tends
 * to be disturbing when the vertex spacing is large. To solve this, we had to
 * override the triangle processing function and make it compatible with Ogre.
 * The function is copied from Bullet, and only one line is changed:
 *
 * - if (m_flipQuadEdges || (m_useDiamondSubdivision && !((j+x) & 1)))
 * + if (m_flipQuadEdges || (m_useDiamondSubdivision && !(j & 1)))
 *
 */

#include "heightfield-terrain-shape.hpp"

LIExtHeightfieldTerrainShape::LIExtHeightfieldTerrainShape (
	int heightStickWidth, int heightStickLength, void* heightfieldData,
	btScalar heightScale, btScalar minHeight, btScalar maxHeight,int upAxis,
	PHY_ScalarType hdt, bool flipQuadEdges) :
	btHeightfieldTerrainShape ( 
	heightStickWidth, heightStickLength, heightfieldData,
	heightScale, minHeight, maxHeight, upAxis, hdt, flipQuadEdges)
{
}

void LIExtHeightfieldTerrainShape::processAllTriangles(btTriangleCallback* callback,const btVector3& aabbMin,const btVector3& aabbMax) const
{
	// scale down the input aabb's so they are in local (non-scaled) coordinates
	btVector3	localAabbMin = aabbMin*btVector3(1.f/m_localScaling[0],1.f/m_localScaling[1],1.f/m_localScaling[2]);
	btVector3	localAabbMax = aabbMax*btVector3(1.f/m_localScaling[0],1.f/m_localScaling[1],1.f/m_localScaling[2]);

	// account for local origin
	localAabbMin += m_localOrigin;
	localAabbMax += m_localOrigin;

	//quantize the aabbMin and aabbMax, and adjust the start/end ranges
	int	quantizedAabbMin[3];
	int	quantizedAabbMax[3];
	quantizeWithClamp(quantizedAabbMin, localAabbMin,0);
	quantizeWithClamp(quantizedAabbMax, localAabbMax,1);
	
	// expand the min/max quantized values
	// this is to catch the case where the input aabb falls between grid points!
	for (int i = 0; i < 3; ++i) {
		quantizedAabbMin[i]--;
		quantizedAabbMax[i]++;
	}	

	int startX=0;
	int endX=m_heightStickWidth-1;
	int startJ=0;
	int endJ=m_heightStickLength-1;

	switch (m_upAxis)
	{
	case 0:
		{
			if (quantizedAabbMin[1]>startX)
				startX = quantizedAabbMin[1];
			if (quantizedAabbMax[1]<endX)
				endX = quantizedAabbMax[1];
			if (quantizedAabbMin[2]>startJ)
				startJ = quantizedAabbMin[2];
			if (quantizedAabbMax[2]<endJ)
				endJ = quantizedAabbMax[2];
			break;
		}
	case 1:
		{
			if (quantizedAabbMin[0]>startX)
				startX = quantizedAabbMin[0];
			if (quantizedAabbMax[0]<endX)
				endX = quantizedAabbMax[0];
			if (quantizedAabbMin[2]>startJ)
				startJ = quantizedAabbMin[2];
			if (quantizedAabbMax[2]<endJ)
				endJ = quantizedAabbMax[2];
			break;
		};
	case 2:
		{
			if (quantizedAabbMin[0]>startX)
				startX = quantizedAabbMin[0];
			if (quantizedAabbMax[0]<endX)
				endX = quantizedAabbMax[0];
			if (quantizedAabbMin[1]>startJ)
				startJ = quantizedAabbMin[1];
			if (quantizedAabbMax[1]<endJ)
				endJ = quantizedAabbMax[1];
			break;
		}
	default:
		{
			//need to get valid m_upAxis
			btAssert(0);
		}
	}

	
  

	for(int j=startJ; j<endJ; j++)
	{
		for(int x=startX; x<endX; x++)
		{
			btVector3 vertices[3];
			if (m_flipQuadEdges || (m_useDiamondSubdivision && !(j & 1)))
			{
        //first triangle
        getVertex(x,j,vertices[0]);
        getVertex(x+1,j,vertices[1]);
        getVertex(x+1,j+1,vertices[2]);
        callback->processTriangle(vertices,x,j);
        //second triangle
        getVertex(x,j,vertices[0]);
        getVertex(x+1,j+1,vertices[1]);
        getVertex(x,j+1,vertices[2]);
        callback->processTriangle(vertices,x,j);				
			} else
			{
        //first triangle
        getVertex(x,j,vertices[0]);
        getVertex(x,j+1,vertices[1]);
        getVertex(x+1,j,vertices[2]);
        callback->processTriangle(vertices,x,j);
        //second triangle
        getVertex(x+1,j,vertices[0]);
        getVertex(x,j+1,vertices[1]);
        getVertex(x+1,j+1,vertices[2]);
        callback->processTriangle(vertices,x,j);
			}
		}
	}
}
