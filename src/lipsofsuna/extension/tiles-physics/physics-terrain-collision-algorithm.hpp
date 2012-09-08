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

#ifndef __PHYSICS_TERRAIN_COLLISION_ALGORITHM_HPP__
#define __PHYSICS_TERRAIN_COLLISION_ALGORITHM_HPP__

#include "lipsofsuna/extension/physics/physics-private.h"
#include "lipsofsuna/extension/physics/physics-collision-configuration.hpp"

class LIExtPhysicsVoxelCollisionAlgorithm : public btConvexConvexAlgorithm
{
public:
	LIExtPhysicsVoxelCollisionAlgorithm (btPersistentManifold* mf, const btCollisionAlgorithmConstructionInfo& ci, btCollisionObject* body0, btCollisionObject* body1, btSimplexSolverInterface* simplexSolver, btConvexPenetrationDepthSolver* pdSolver, int numPerturbationIterations, int minimumPointsPerturbationThreshold);
	virtual void processCollision (btCollisionObject* body0, btCollisionObject* body1, const btDispatcherInfo& dispatchInfo, btManifoldResult* resultOut);
public:
	static void rebuild_shape_cache (float size);
	static float tile_size;
	static btConvexHullShape* slopes_above[16];
	static btConvexHullShape* slopes_below[16];
};

class LIExtPhysicsVoxelCollisionAlgorithmCreator : public LIPhyCollisionAlgorithmCreator
{
public:
	virtual btCollisionAlgorithm* create (
		btCollisionAlgorithmConstructionInfo& ci,
		btCollisionObject*                    body0,
		btCollisionObject*                    body1,
		btSimplexSolverInterface*             simplex_solver,
		btConvexPenetrationDepthSolver*       depth_solver,
		int                                   perturbation_iterations,
		int                                   perturbation_threshold)
	{
		LIPhyPointer* pointer;

		pointer = (LIPhyPointer*) body0->getUserPointer ();
		if (pointer->type != LIPHY_POINTER_TYPE_VOXEL)
			return NULL;

		void* mem = ci.m_dispatcher1->allocateCollisionAlgorithm (sizeof (LIExtPhysicsVoxelCollisionAlgorithm));
		return new(mem) LIExtPhysicsVoxelCollisionAlgorithm (ci.m_manifold, ci, body0, body1, simplex_solver, depth_solver, perturbation_iterations, perturbation_threshold);
	}
};

#endif
