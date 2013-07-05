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

#include "lipsofsuna/extension/physics/physics-private.h"
#include "physics-terrain.h"
#include "physics-terrain.hpp"
#include "physics-terrain-collision-algorithm.hpp"

void* liext_physics_terrain_collision_algorithm_new (
	LIPhyPhysics* physics)
{
	LIExtPhysicsTerrainCollisionAlgorithmCreator* self;

	self = new LIExtPhysicsTerrainCollisionAlgorithmCreator ();
	physics->configuration->add_algorithm (self);

	return self;
}

void liext_physics_terrain_collision_algorithm_free (
	LIPhyPhysics* physics,
	void*         self)
{
	LIExtPhysicsTerrainCollisionAlgorithmCreator* s;

	s = (LIExtPhysicsTerrainCollisionAlgorithmCreator*) self;
	physics->configuration->remove_algorithm (s);
	delete s;
}

/*****************************************************************************/

LIExtPhysicsTerrainCollisionAlgorithm::LIExtPhysicsTerrainCollisionAlgorithm (
	btPersistentManifold* mf,
	const btCollisionAlgorithmConstructionInfo& ci,
#if BT_BULLET_VERSION >= 280
	const btCollisionObjectWrapper* body0,
	const btCollisionObjectWrapper* body1,
#else
	btCollisionObject* body0,
	btCollisionObject* body1,
#endif
	btSimplexSolverInterface* simplexSolver,
	btConvexPenetrationDepthSolver* pdSolver,
	int numPerturbationIterations,
	int minimumPointsPerturbationThreshold) :
		btConvexConvexAlgorithm (mf, ci, body0, body1, simplexSolver, pdSolver, numPerturbationIterations, minimumPointsPerturbationThreshold)
{
}

void LIExtPhysicsTerrainCollisionAlgorithm::processCollision (
#if BT_BULLET_VERSION >= 280
	const btCollisionObjectWrapper* wrapper0,
	const btCollisionObjectWrapper* wrapper1,
#else
	btCollisionObject* body0,
	btCollisionObject* body1,
#endif
	const btDispatcherInfo& dispatchInfo,
	btManifoldResult* resultOut)
{
	int x;
	int z;

	// Get the objects and shapes.
#if BT_BULLET_VERSION >= 280
	const btCollisionObject* body0 = wrapper0->getCollisionObject();
	const btCollisionObject* body1 = wrapper1->getCollisionObject();
	const btCollisionShape* shape0 = wrapper0->getCollisionShape();
	const btCollisionShape* shape1 = wrapper1->getCollisionShape();
	const btTransform& xform0 = wrapper0->getWorldTransform();
	const btTransform& xform1 = wrapper1->getWorldTransform();
#else
	const btCollisionShape* shape0 = body0->getCollisionShape();
	const btCollisionShape* shape1 = body1->getCollisionShape();
	const btTransform& xform0 = body0->getWorldTransform();
	const btTransform& xform1 = body1->getWorldTransform();
#endif

	// Filter collisions.
	const btBroadphaseProxy* proxy0 = body0->getBroadphaseHandle ();
	const btBroadphaseProxy* proxy1 = body1->getBroadphaseHandle ();
	if (!(proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) ||
	    !(proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask))
		return;

	// Identify the bodies.
	btTransform xform_convex;
	btTransform xform_terrain;
	btCollisionObject* object_convex;
	btCollisionObject* object_terrain;
	const btConvexHullShape* shape_convex;
	const LIExtPhysicsTerrainShape* shape_terrain;
	if (shape0->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE)
	{
		object_terrain = (btCollisionObject*) body0;
		object_convex = (btCollisionObject*) body1;
		lisys_assert (shape0->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE);
		lisys_assert (shape1->getShapeType () == CONVEX_HULL_SHAPE_PROXYTYPE);
		shape_terrain = (LIExtPhysicsTerrainShape*) shape0;
		shape_convex = (btConvexHullShape*) shape1;
		xform_terrain = xform0;
		xform_convex = xform1;
	}
	else
	{
		object_terrain = (btCollisionObject*) body1;
		object_convex = (btCollisionObject*) body0;
		lisys_assert (shape1->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE);
		lisys_assert (shape0->getShapeType () == CONVEX_HULL_SHAPE_PROXYTYPE);
		shape_terrain = (LIExtPhysicsTerrainShape*) shape1;
		shape_convex = (btConvexHullShape*) shape0;
		xform_terrain = xform1;
		xform_convex = xform0;
	}

	// Get the terrain data pointer.
	LIPhyPointer* pointer_terrain = (LIPhyPointer*) object_terrain->getUserPointer ();
	lisys_assert (pointer_terrain->type == LIPHY_POINTER_TYPE_TERRAIN);

	// Get the range of intersecting grid points.
	btVector3 aabb_min;
	btVector3 aabb_max;
	shape_convex->getAabb (xform_convex, aabb_min, aabb_max);
	float grid_size = shape_terrain->terrain->terrain->grid_size;
	int min_x = (int)(aabb_min[0] / grid_size);
	int max_x = (int)(aabb_max[0] / grid_size);
	float min_y = aabb_min[1];
	float max_y = aabb_max[1];
	int min_z = (int)(aabb_min[2] / grid_size);
	int max_z = (int)(aabb_max[2] / grid_size);

	// Process collisions with each tile.
	for (z = min_z ; z <= max_z ; z++)
	for (x = min_x ; x <= max_x ; x++)
	{
		// Get the column.
		LIExtTerrainColumn* column = liext_terrain_get_column (shape_terrain->terrain->terrain, x, z);
		if (column == NULL)
			continue;

		// Initialize the stick vertices.
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

		// Collide against each stick.
		float ys = 0.0f;
		float ys_min = 0.0f;
		float ys_max = 0.0f;
		LIExtTerrainStick* stick;
		for (stick = column->sticks ; stick != NULL ; stick = stick->next)
		{
			ys += stick->height;
			if (ys_min >= max_y)
				break;

			// Set the top surface offset.
			verts[4][1] = ys + stick->vertices[0][0].offset;
			verts[5][1] = ys + stick->vertices[1][0].offset;
			verts[6][1] = ys + stick->vertices[0][1].offset;
			verts[7][1] = ys + stick->vertices[1][1].offset;
			ys_max = LIMAT_MAX (ys_max, verts[4][1]);
			ys_max = LIMAT_MAX (ys_max, verts[5][1]);
			ys_max = LIMAT_MAX (ys_max, verts[6][1]);
			ys_max = LIMAT_MAX (ys_max, verts[7][1]);

			// Collide against a temporary convex shape.
			if (ys_max >= min_y && stick->material != 0)
			{
				btConvexHullShape shape ((btScalar*) verts, 8, sizeof (btVector3));
				pointer_terrain->tile[0] = x;
				pointer_terrain->tile[1] = 0;
				pointer_terrain->tile[2] = z;
#if BT_BULLET_VERSION >= 280
				btCollisionObjectWrapper ob0(0, &shape, object_terrain, xform_terrain);
				btCollisionObjectWrapper ob1(0, shape_convex, object_convex, xform_convex);
				btConvexConvexAlgorithm::processCollision (&ob0, &ob1, dispatchInfo, resultOut);
#else
				object_terrain->setCollisionShape (&shape);
				btConvexConvexAlgorithm::processCollision (object_terrain, object_convex, dispatchInfo, resultOut);
				object_terrain->setCollisionShape ((btCollisionShape*) shape_terrain);
#endif
			}

			// Set the bottom surface offset.
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

	// TODO: Liquid handling.
}
