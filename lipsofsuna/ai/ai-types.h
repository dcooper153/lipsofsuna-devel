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

#ifndef __AI_TYPES_H__
#define __AI_TYPES_H__

#define LIAI_WAYPOINT_INDEX(x, y, z) ((x) + LIAI_WAYPOINTS_PER_LINE * (y) + LIAI_WAYPOINTS_PER_PLANE * (z))
#define LIAI_WAYPOINT_WIDTH (LIVOX_TILE_WIDTH)
#define LIAI_WAYPOINTS_PER_LINE (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE)
#define LIAI_WAYPOINTS_PER_PLANE (LIAI_WAYPOINTS_PER_LINE * LIAI_WAYPOINTS_PER_LINE)
#define LIAI_WAYPOINTS_PER_SECTOR (LIAI_WAYPOINTS_PER_LINE * LIAI_WAYPOINTS_PER_PLANE)
#define LIAI_SECTOR_INDEX(x, y, z) ((x) + LIAI_SECTORS_PER_LINE * (y) + LIAI_SECTORS_PER_PLANE * (z))
#define LIAI_SECTORS_PER_LINE 256
#define LIAI_SECTORS_PER_PLANE 256
#define LIAI_SECTOR_WIDTH (LIAI_WAYPOINT_WIDTH * LIAI_WAYPOINTS_PER_LINE)

typedef struct _liaiManager liaiManager;
typedef struct _liaiSector liaiSector;
typedef struct _liaiWaypoint liaiWaypoint;

#endif
