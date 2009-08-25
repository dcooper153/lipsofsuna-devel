/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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

/**
 * \addtogroup liai Ai
 * @{
 * \addtogroup liaiPathSolver Path Solver
 * @{
 */

#include <system/lips-system.h>
#include "ai-path-solver.h"

static float
private_astar_cost (void*         world,
                    void*         object,
                    liaiWaypoint* start,
                    liaiWaypoint* end);

static float
private_astar_heuristic (void*         world,
                         void*         object,
                         liaiWaypoint* start,
                         liaiWaypoint* end);

static int
private_astar_passable (void*         world,
                        void*         object,
                        liaiWaypoint* start,
                        liaiWaypoint* end);

static void*
private_astar_successor (void*         world,
                         void*         object,
                         liaiWaypoint* node,
                         int           index);

/*****************************************************************************/

/**
 * \brief Creates a new path solver.
 *
 * \return New path solver or NULL.
 */
liaiPathSolver*
liai_path_solver_new ()
{
	liaiPathSolver* self;

	self = lisys_calloc (1, sizeof (liaiPathSolver));
	if (self == NULL)
		return NULL;
	self->astar = lialg_astar_new (
		(lialgAstarCost) private_astar_cost,
		(lialgAstarHeuristic) private_astar_heuristic,
		(lialgAstarPassable) private_astar_passable,
		(lialgAstarSuccessor) private_astar_successor);
	if (self->astar == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the path solver.
 *
 * \param self Path solver.
 */
void
liai_path_solver_free (liaiPathSolver* self)
{
	lialg_astar_free (self->astar);
	lisys_free (self);
}

/**
 * \brief Solves path from the starting point to end point.
 *
 * \param self Path solver.
 * \param start Start waypoint.
 * \param end Target waypoint.
 * \return New path or NULL.
 */
liaiPath*
liai_path_solver_solve (liaiPathSolver* self,
                        liaiWaypoint*   start,
                        liaiWaypoint*   end)
{
	int i;
	liaiPath* path;
	liaiWaypoint* point;
	lialgAstarResult* result;

	/* Solve path, */
	result = lialg_astar_solve (self->astar, NULL, NULL, start, end);
	if (result == NULL)
		return NULL;

	/* Allocate path. */
	path = liai_path_new ();
	if (path == NULL)
	{
		lialg_astar_result_free (result);
		return NULL;
	}

	/* Convert results. */
	for (i = 0 ; i < result->length ; i++)
	{
		point = result->nodes[i];
		if (!liai_path_append_point (path, &point->position))
		{
			lialg_astar_result_free (result);
			liai_path_free (path);
			return NULL;
		}
	}
	lialg_astar_result_free (result);

	return path;
}

/*****************************************************************************/

static float
private_astar_cost (void*         world,
                    void*         object,
                    liaiWaypoint* start,
                    liaiWaypoint* end)
{
	return limat_vector_get_length (
		limat_vector_subtract (start->position, end->position));
}

static float
private_astar_heuristic (void*         world,
                         void*         object,
                         liaiWaypoint* start,
                         liaiWaypoint* end)
{
	return LI_ABS (start->position.x - end->position.x) +
	       LI_ABS (start->position.y - end->position.y) +
	       LI_ABS (start->position.z - end->position.z);
}

static int
private_astar_passable (void*         world,
                        void*         object,
                        liaiWaypoint* start,
                        liaiWaypoint* end)
{
	/* FIXME: Check for object size. */
	return 1;
}

static void*
private_astar_successor (void*         world,
                         void*         object,
                         liaiWaypoint* node,
                         int           index)
{
	if (index >= node->links.count)
		return NULL;
	return node->links.links[index].target;
}

/** @} */
/** @} */
