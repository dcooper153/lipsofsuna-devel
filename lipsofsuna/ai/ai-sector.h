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
 * \addtogroup liaiSector Sector
 * @{
 */

#ifndef __AI_SECTOR_H__
#define __AI_SECTOR_H__

#include <math/lips-math.h>
#include <algorithm/lips-algorithm.h>
#include "ai-waypoint.h"

typedef struct _liaiSector liaiSector;
struct _liaiSector
{
	lialgList* waypoints; // FIXME: Slow, use an octree.
};

liaiSector*
liai_sector_new ();

liaiSector*
liai_sector_new_from_data (liReader* reader);

liaiSector*
liai_sector_new_from_file (const char* path);

void
liai_sector_free (liaiSector* self);

liaiWaypoint*
liai_sector_find_waypoint (liaiSector*     self,
                           const limatVector* point);

int
liai_sector_insert_waypoint (liaiSector*   self,
                             liaiWaypoint* waypoint);

void
liai_sector_remove_waypoint (liaiSector*   self,
                             liaiWaypoint* waypoint);

int
liai_sector_save (const liaiSector* self,
                  const char*       path);

#endif

/** @} */
/** @} */
