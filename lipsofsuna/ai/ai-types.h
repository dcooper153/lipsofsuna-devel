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

#ifndef __AI_TYPES_H__
#define __AI_TYPES_H__

#define LIAI_WAYPOINT_INDEX(x, y, z) ((x) + LIAI_WAYPOINTS_PER_LINE * (y) + LIAI_WAYPOINTS_PER_PLANE * (z))
#define LIAI_WAYPOINT_WIDTH (2.0f)
#define LIAI_WAYPOINTS_PER_LINE (16)
#define LIAI_WAYPOINTS_PER_PLANE (LIAI_WAYPOINTS_PER_LINE * LIAI_WAYPOINTS_PER_LINE)
#define LIAI_WAYPOINTS_PER_SECTOR (LIAI_WAYPOINTS_PER_LINE * LIAI_WAYPOINTS_PER_PLANE)

enum
{
	LIAI_WAYPOINT_FLAG_FLYABLE  = 0x01,
	LIAI_WAYPOINT_FLAG_WALKABLE = 0x02
};

typedef struct _LIAiManager LIAiManager;
typedef struct _LIAiSector LIAiSector;
typedef struct _LIAiWaypoint LIAiWaypoint;

#endif
