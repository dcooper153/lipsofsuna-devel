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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup lialgAstar Astar
 * @{
 */

#ifndef __ALGORITHM_ASTAR_H__
#define __ALGORITHM_ASTAR_H__

#include "algorithm-ptrdic.h"
#include "algorithm-priority-queue.h"

typedef float (*lialgAstarCost)(void* world, void* object, void* start, void* end);
typedef float (*lialgAstarHeuristic)(void* world, void* object, void* start, void* end);
typedef int (*lialgAstarPassable)(void* world, void* object, void* start, void* end);
typedef void* (*lialgAstarSuccessor)(void* world, void* object, void* node, int index);

typedef struct _lialgAstar lialgAstar;
struct _lialgAstar
{
	void* world;
	void* object;
	void* target;
	lialgPtrdic* open;
	lialgPtrdic* closed;
	lialgPriorityQueue* priority;
	struct
	{
		lialgAstarCost cost;
		lialgAstarHeuristic heuristic;
		lialgAstarPassable passable;
		lialgAstarSuccessor successor;
	} calls;
};

typedef struct _lialgAstarResult lialgAstarResult;
struct _lialgAstarResult
{
	int length;
	void** nodes;
};

lialgAstar*
lialg_astar_new (lialgAstarCost      cost,
                 lialgAstarHeuristic heuristic,
                 lialgAstarPassable  passable,
                 lialgAstarSuccessor successor);

void
lialg_astar_free (lialgAstar* self);

lialgAstarResult*
lialg_astar_solve (lialgAstar* self,
                   void*       world,
                   void*       object,
                   void*       start,
                   void*       end);

void
lialg_astar_result_free (lialgAstarResult* self);

#endif

/** @} */
/** @} */
