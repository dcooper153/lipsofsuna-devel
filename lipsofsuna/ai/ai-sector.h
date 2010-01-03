/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LIAiSector Sector
 * @{
 */

#ifndef __AI_SECTOR_H__
#define __AI_SECTOR_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/voxel.h>
#include "ai-waypoint.h"
#include "ai-types.h"

struct _LIAiSector
{
	LIAiManager* manager;
	LIAiWaypoint points[LIAI_WAYPOINTS_PER_SECTOR];
	LIAlgSector* sector;
};

LIAiSector*
liai_sector_new (LIAlgSector* sector);

void
liai_sector_free (LIAiSector* self);

void
liai_sector_build (LIAiSector*  self,
                   LIVoxSector* voxels);

void
liai_sector_build_area (LIAiSector*  self,
                        LIVoxSector* voxels,
                        int          x,
                        int          y,
                        int          z,
                        int          xs,
                        int          ys,
                        int          zs);

LIAiWaypoint*
liai_sector_get_waypoint (LIAiSector* self,
                          int         x,
                          int         y,
                          int         z);

#endif

/** @} */
/** @} */
