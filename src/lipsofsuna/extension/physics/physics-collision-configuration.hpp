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

#ifndef __PHYSICS_COLLISION_CONFIGURATION_HPP__
#define __PHYSICS_COLLISION_CONFIGURATION_HPP__

#include "physics-private.h"
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.h>

class LIPhyCollisionAlgorithmCreator
{
public:
	LIPhyCollisionAlgorithmCreator () {}
	virtual ~LIPhyCollisionAlgorithmCreator () {}
	virtual btCollisionAlgorithm* create (
		btCollisionAlgorithmConstructionInfo& ci,
		btCollisionObject*                    body0,
		btCollisionObject*                    body1,
		btSimplexSolverInterface*             simplex_solver,
		btConvexPenetrationDepthSolver*       depth_solver,
		int                                   perturbation_iterations,
		int                                   perturbation_threshold) = 0;
public:
	LIPhyCollisionAlgorithmCreator* next;
};

/*****************************************************************************/

class LIPhyCollisionAlgorithmCreateFunc;

class LIPhyCollisionConfiguration : public btDefaultCollisionConfiguration
{
public:
	LIPhyCollisionConfiguration ();
	virtual ~LIPhyCollisionConfiguration ();
	virtual btCollisionAlgorithmCreateFunc* getCollisionAlgorithmCreateFunc (int proxyType0, int proxyType1);
public:
	void add_algorithm (LIPhyCollisionAlgorithmCreator* algorithm);
	void remove_algorithm (LIPhyCollisionAlgorithmCreator* algorithm);
public:
	LIPhyCollisionAlgorithmCreator* algorithms;
	LIPhyCollisionAlgorithmCreateFunc* create_func;
};

/*****************************************************************************/

class LIPhyCollisionAlgorithmCreateFunc : public btConvexConvexAlgorithm::CreateFunc
{
public:
	LIPhyCollisionAlgorithmCreateFunc (LIPhyCollisionConfiguration* configuration, btSimplexSolverInterface* simplexSolver, btConvexPenetrationDepthSolver* pdSolver) :
		btConvexConvexAlgorithm::CreateFunc (simplexSolver, pdSolver)
	{
		this->configuration = configuration;
	}
	virtual btCollisionAlgorithm* CreateCollisionAlgorithm (btCollisionAlgorithmConstructionInfo& ci, btCollisionObject* body0, btCollisionObject* body1)
	{
		LIPhyCollisionAlgorithmCreator* ptr;
		for (ptr = configuration->algorithms ; ptr != NULL ; ptr = ptr->next)
		{
			btCollisionAlgorithm* algo;
			if (body0->getCollisionShape ()->getShapeType () == CUSTOM_CONVEX_SHAPE_TYPE)
				algo = ptr->create (ci, body0, body1, m_simplexSolver, m_pdSolver, m_numPerturbationIterations, m_minimumPointsPerturbationThreshold);
			else
				algo = ptr->create (ci, body1, body0, m_simplexSolver, m_pdSolver, m_numPerturbationIterations, m_minimumPointsPerturbationThreshold);
			if (algo != NULL)
				return algo;
		}
		lisys_assert (0);
		
		return NULL;
	}
public:
	LIPhyCollisionConfiguration* configuration;
};

#endif
