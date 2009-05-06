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

#ifndef __AI_PATH_SOLVER_H__
#define __AI_PATH_SOLVER_H__

#include <algorithm/lips-algorithm.h>
#include "ai-path.h"
#include "ai-waypoint.h"

typedef struct _liaiPathSolver liaiPathSolver;
struct _liaiPathSolver
{
	lialgAstar* astar;
};

liaiPathSolver*
liai_path_solver_new ();

void
liai_path_solver_free (liaiPathSolver* self);

liaiPath*
liai_path_solver_solve (liaiPathSolver* self,
                        liaiWaypoint*   start,
                        liaiWaypoint*   end);

#endif

/** @} */
/** @} */
