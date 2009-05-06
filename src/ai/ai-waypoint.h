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
 * \addtogroup liaiWaypoint Waypoint
 * @{
 */

#ifndef __AI_WAYPOINT_H__
#define __AI_WAYPOINT_H__

#include <math/lips-math.h>

typedef struct _liaiWaypointLink liaiWaypointLink;
typedef struct _liaiWaypoint liaiWaypoint;

struct _liaiWaypointLink
{
	float distance;
	liaiWaypoint* target;
};

struct _liaiWaypoint
{
	limatVector position;
	struct
	{
		int count;
		liaiWaypointLink* links;
	} links;
};

liaiWaypoint*
liai_waypoint_new (const limatVector* point);

void
liai_waypoint_free (liaiWaypoint* self);

int
liai_waypoint_insert_link (liaiWaypoint* self,
                           liaiWaypoint* waypoint);

#endif

/** @} */
/** @} */
