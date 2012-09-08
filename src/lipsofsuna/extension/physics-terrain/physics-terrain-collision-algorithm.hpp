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

#ifndef __LIEXT_PHYSICS_TERRAIN__PHYSICS_TERRAIN_COLLISION_ALGORITHM_HPP__
#define __LIEXT_PHYSICS_TERRAIN__PHYSICS_TERRAIN_COLLISION_ALGORITHM_HPP__

#include "lipsofsuna/extension/physics/physics-private.h"
#include "lipsofsuna/extension/physics/physics-collision-configuration.hpp"
#include <BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.h>
#include "physics-terrain-collision-algorithm.h"

class LIExtPhysicsTerrainCollisionAlgorithm : public btConvexConvexAlgorithm
{
public:
	LIExtPhysicsTerrainCollisionAlgorithm (btPersistentManifold* mf, const btCollisionAlgorithmConstructionInfo& ci, btCollisionObject* body0, btCollisionObject* body1, btSimplexSolverInterface* simplexSolver, btConvexPenetrationDepthSolver* pdSolver, int numPerturbationIterations, int minimumPointsPerturbationThreshold);
	virtual void processCollision (btCollisionObject* body0, btCollisionObject* body1, const btDispatcherInfo& dispatchInfo, btManifoldResult* resultOut);
};

class LIExtPhysicsTerrainCollisionAlgorithmCreator : public LIPhyCollisionAlgorithmCreator
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
		if (pointer->type != LIPHY_POINTER_TYPE_TERRAIN)
			return NULL;

		void* mem = ci.m_dispatcher1->allocateCollisionAlgorithm (sizeof (LIExtPhysicsTerrainCollisionAlgorithm));
		return new(mem) LIExtPhysicsTerrainCollisionAlgorithm (ci.m_manifold, ci, body0, body1, simplex_solver, depth_solver, perturbation_iterations, perturbation_threshold);
	}
};

#endif
