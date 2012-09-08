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

#include "physics-collision-configuration.hpp"

LIPhyCollisionConfiguration::LIPhyCollisionConfiguration () : btDefaultCollisionConfiguration ()
{
	algorithms = NULL;
	void* mem = btAlignedAlloc (sizeof (LIPhyCollisionAlgorithmCreateFunc), 16);
	create_func = new(mem) LIPhyCollisionAlgorithmCreateFunc (this, this->m_simplexSolver, this->m_pdSolver);
}

LIPhyCollisionConfiguration::~LIPhyCollisionConfiguration ()
{
	lisys_assert (algorithms == NULL);
	create_func->~LIPhyCollisionAlgorithmCreateFunc ();
	btAlignedFree (this->create_func);
}

btCollisionAlgorithmCreateFunc* LIPhyCollisionConfiguration::getCollisionAlgorithmCreateFunc (int proxyType0, int proxyType1)
{
	if ((proxyType0 == CONVEX_HULL_SHAPE_PROXYTYPE) && (proxyType1 == CUSTOM_CONVEX_SHAPE_TYPE))
		return create_func;
	if ((proxyType0 == CUSTOM_CONVEX_SHAPE_TYPE) && (proxyType1 == CONVEX_HULL_SHAPE_PROXYTYPE))
		return create_func;
	return btDefaultCollisionConfiguration::getCollisionAlgorithmCreateFunc (proxyType0, proxyType1);
}

void LIPhyCollisionConfiguration::add_algorithm (LIPhyCollisionAlgorithmCreator* algorithm)
{
	algorithm->next = algorithms;
	algorithms = algorithm;
}

void LIPhyCollisionConfiguration::remove_algorithm (LIPhyCollisionAlgorithmCreator* algorithm)
{
	LIPhyCollisionAlgorithmCreator* ptr;

	if (algorithm == algorithms)
	{
		algorithms = algorithm->next;
		return;
	}
	for (ptr = algorithms ; ptr != NULL ; ptr = ptr->next)
	{
		if (ptr->next == algorithm)
		{
			ptr->next = algorithm->next;
			return;
		}
	}
	lisys_assert (0);
}
