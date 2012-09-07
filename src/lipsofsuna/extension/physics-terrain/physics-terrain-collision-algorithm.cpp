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

#include "lipsofsuna/extension/physics/physics-private.h"
#include "physics-terrain.h"
#include "physics-terrain.hpp"
#include "physics-terrain-collision-algorithm.hpp"

float LIExtPhysicsTerrainCollisionAlgorithm::tile_size = -1.0f;
btConvexHullShape* LIExtPhysicsTerrainCollisionAlgorithm::slopes_above[16];
btConvexHullShape* LIExtPhysicsTerrainCollisionAlgorithm::slopes_below[16];

LIExtPhysicsTerrainCollisionAlgorithm::LIExtPhysicsTerrainCollisionAlgorithm (btPersistentManifold* mf, const btCollisionAlgorithmConstructionInfo& ci, btCollisionObject* body0, btCollisionObject* body1, btSimplexSolverInterface* simplexSolver, btConvexPenetrationDepthSolver* pdSolver, int numPerturbationIterations, int minimumPointsPerturbationThreshold) :
	btConvexConvexAlgorithm (mf, ci, body0, body1, simplexSolver, pdSolver, numPerturbationIterations, minimumPointsPerturbationThreshold)
{
}

void LIExtPhysicsTerrainCollisionAlgorithm::processCollision (btCollisionObject* body0, btCollisionObject* body1, const btDispatcherInfo& dispatchInfo, btManifoldResult* resultOut)
{
	int x;
	int z;
	btCollisionObject* object_convex;
	btCollisionObject* object_terrain;
	btConvexHullShape* shape_convex;
	LIPhyPointer* pointer_terrain;
	LIExtPhysicsTerrainShape* shape_terrain;

	/* Filter collisions. */
	btBroadphaseProxy* proxy0 = body0->getBroadphaseHandle ();
	btBroadphaseProxy* proxy1 = body1->getBroadphaseHandle ();
	if (!(proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) ||
	    !(proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask))
		return;

	/* Identify the bodies. */
	if (body0->getCollisionShape ()->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE)
	{
		object_terrain = body0;
		object_convex = body1;
	}
	else
	{
		object_terrain = body1;
		object_convex = body0;
	}
	lisys_assert (object_convex->getCollisionShape ()->getShapeType () == CONVEX_HULL_SHAPE_PROXYTYPE);
	lisys_assert (object_terrain->getCollisionShape ()->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE);
	shape_convex = (btConvexHullShape*) object_convex->getCollisionShape ();
	shape_terrain = (LIExtPhysicsTerrainShape*) object_terrain->getCollisionShape ();
	pointer_terrain = (LIPhyPointer*) object_terrain->getUserPointer ();

	/* Get the range of intersecting grid points. */
	btVector3 aabb_min;
	btVector3 aabb_max;
	btTransform transform = object_convex->getWorldTransform ();
	shape_convex->getAabb (transform, aabb_min, aabb_max);
	float grid_size = shape_terrain->terrain->terrain->grid_size;
	int min_x = (int)(aabb_min[0] / grid_size);
	int max_x = (int)(aabb_max[0] / grid_size);
	float min_y = aabb_min[1];
	float max_y = aabb_max[1];
	int min_z = (int)(aabb_min[2] / grid_size);
	int max_z = (int)(aabb_max[2] / grid_size);

	/* Process collisions with each tile. */
	for (z = min_z ; z <= max_z ; z++)
	for (x = min_x ; x <= max_x ; x++)
	{
		/* Get the column. */
		LIExtTerrainColumn* column = liext_terrain_get_column (shape_terrain->terrain->terrain, x, z);
		if (column == NULL)
			continue;

		/* Initialize the stick vertices. */
		btVector3 offset = btVector3 (x, 0.0f, z) * grid_size;
		btVector3 verts[8] =
		{
			offset + btVector3(     0.0f, 0.0f, 0.0f),
			offset + btVector3(grid_size, 0.0f, 0.0f),
			offset + btVector3(     0.0f, 0.0f, grid_size),
			offset + btVector3(grid_size, 0.0f, grid_size),
			offset + btVector3(     0.0f, 0.0f, 0.0f),
			offset + btVector3(grid_size, 0.0f, 0.0f),
			offset + btVector3(     0.0f, 0.0f, grid_size),
			offset + btVector3(grid_size, 0.0f, grid_size)
		};

		/* Collide against each stick. */
		float ys = 0.0f;
		float ys_min = 0.0f;
		float ys_max = 0.0f;
		LIExtTerrainStick* stick;
		for (stick = column->sticks ; stick != NULL ; stick = stick->next)
		{
			ys += stick->height;
			if (ys_min >= max_y)
				break;

			/* Set the top surface offset. */
			verts[4][1] = ys + stick->vertices[0][0].offset;
			verts[5][1] = ys + stick->vertices[1][0].offset;
			verts[6][1] = ys + stick->vertices[0][1].offset;
			verts[7][1] = ys + stick->vertices[1][1].offset;
			ys_max = LIMAT_MAX (ys_max, verts[4][1]);
			ys_max = LIMAT_MAX (ys_max, verts[5][1]);
			ys_max = LIMAT_MAX (ys_max, verts[6][1]);
			ys_max = LIMAT_MAX (ys_max, verts[7][1]);

			/* Collide against a temporary convex shape. */
			if (ys_max >= min_y && stick->material != 0)
			{
				btConvexHullShape shape ((btScalar*) verts, 8, sizeof (btVector3));
				object_terrain->setCollisionShape (&shape);
				pointer_terrain->tile[0] = x;
				pointer_terrain->tile[1] = 0;
				pointer_terrain->tile[2] = z;
				btConvexConvexAlgorithm::processCollision (object_terrain, object_convex, dispatchInfo, resultOut);
				object_terrain->setCollisionShape (shape_terrain);
			}

			/* Set the bottom surface offset. */
			verts[0] = verts[4];
			verts[1] = verts[5];
			verts[2] = verts[6];
			verts[3] = verts[7];
			ys_min = LIMAT_MIN (ys_max, verts[4][1]);
			ys_min = LIMAT_MIN (ys_max, verts[5][1]);
			ys_min = LIMAT_MIN (ys_max, verts[6][1]);
			ys_min = LIMAT_MIN (ys_max, verts[7][1]);
		}
	}

	/* TODO: Liquid handling. */
}
