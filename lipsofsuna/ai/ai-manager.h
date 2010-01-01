/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liaiPath Path
 * @{
 */

#ifndef __AI_MANAGER_H__
#define __AI_MANAGER_H__

#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include <voxel/lips-voxel.h>
#include "ai-path.h"
#include "ai-types.h"

struct _liaiManager
{
	lialgAstar* astar;
	lialgSectors* sectors;
	licalCallbacks* callbacks;
	licalHandle calls[1];
	livoxManager* voxels;
};

liaiManager*
liai_manager_new (licalCallbacks* callbacks,
                  lialgSectors*   sectors,
                  livoxManager*   voxels);

void
liai_manager_free (liaiManager* self);

liaiWaypoint*
liai_manager_find_waypoint (liaiManager*       self,
                            const limatVector* point);

int
liai_manager_load_sector (liaiManager* self,
                          int          sx,
                          int          sy,
                          int          sz);

liaiPath*
liai_manager_solve_path (liaiManager*       self,
                         const limatVector* start,
                         const limatVector* end);

#endif

/** @} */
/** @} */
