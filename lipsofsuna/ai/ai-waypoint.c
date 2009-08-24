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

#include "ai-waypoint.h"

/**
 * \brief Creates a new waypoint.
 *
 * \param point Position vector of the waypoint.
 * \return A new waypoint or NULL.
 */
liaiWaypoint*
liai_waypoint_new (const limatVector* point)
{
	liaiWaypoint* self;

	self = calloc (1, sizeof (liaiWaypoint));
	if (self == NULL)
		return NULL;
	self->position = *point;
	return self;
}

/**
 * \brief Frees the waypoint.
 *
 * \param self Waypoint.
 */
void
liai_waypoint_free (liaiWaypoint* self)
{
	free (self->links.links);
	free (self);
}

/**
 * \brief Creates a one way link between the two waypoints.
 *
 * \param self Waypoint.
 * \param waypoint Waypoint.
 * \return Nonzero on success.
 */
int
liai_waypoint_insert_link (liaiWaypoint* self,
                           liaiWaypoint* waypoint)
{
	liaiWaypointLink* tmp;

	/* Allocate space. */
	tmp = realloc (self->links.links, (self->links.count + 1) * sizeof (liaiWaypointLink));
	if (tmp == NULL)
		return 0;
	self->links.links = tmp;
	tmp += self->links.count++;

	/* Initialize link. */
	tmp->target = waypoint;
	tmp->distance = limat_vector_get_length (limat_vector_subtract (self->position, waypoint->position));
	return 1;
}

/** @} */
/** @} */
