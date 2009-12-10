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
 * \addtogroup liaiSector Sector
 * @{
 */

#ifndef __AI_SECTOR_H__
#define __AI_SECTOR_H__

#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include <voxel/lips-voxel.h>
#include "ai-waypoint.h"
#include "ai-types.h"

struct _liaiSector
{
	int x;
	int y;
	int z;
	liaiManager* manager;
	liaiWaypoint points[LIAI_WAYPOINTS_PER_SECTOR];
};

liaiSector*
liai_sector_new (liaiManager* manager,
                 int          x,
                 int          y,
                 int          z,
                 livoxSector* voxels);

void
liai_sector_free (liaiSector* self);

void
liai_sector_build (liaiSector*  self,
                   livoxSector* voxels);

void
liai_sector_build_area (liaiSector*  self,
                        livoxSector* voxels,
                        int          x,
                        int          y,
                        int          z,
                        int          xs,
                        int          ys,
                        int          zs);

liaiWaypoint*
liai_sector_get_waypoint (liaiSector* self,
                          int         x,
                          int         y,
                          int         z);

#endif

/** @} */
/** @} */
