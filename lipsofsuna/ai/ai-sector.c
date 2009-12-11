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

#include <system/lips-system.h>
#include "ai-sector.h"

/**
 * \brief Creates a new AI sector.
 *
 * \param manager AI manager.
 * \param x Sector offset.
 * \param y Sector offset.
 * \param z Sector offset.
 * \param voxels Voxel sector or NULL.
 * \return New sector or NULL.
 */
liaiSector*
liai_sector_new (liaiManager* manager,
                 int          x,
                 int          y,
                 int          z,
                 livoxSector* voxels)
{
	int i;
	int j;
	int k;
	int l;
	liaiSector* self;
	liaiWaypoint* wp;
	limatVector offset;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liaiSector));
	if (self == NULL)
		return NULL;
	self->x = x;
	self->y = y;
	self->z = z;
	self->manager = manager;

	/* Calculate offset. */
	offset = limat_vector_init (x, y, z);
	offset = limat_vector_multiply (offset, LIAI_SECTOR_WIDTH);

	/* Set waypoint positions. */
	for (k = l = 0 ; k < LIAI_WAYPOINTS_PER_LINE ; k++)
	for (j = 0 ; j < LIAI_WAYPOINTS_PER_LINE ; j++)
	for (i = 0 ; i < LIAI_WAYPOINTS_PER_LINE ; i++, l++)
	{
		wp = self->points + l;
		wp->x = i;
		wp->y = j;
		wp->z = k;
		wp->sector = self;
		wp->position = limat_vector_init (i + 0.5f, j + 0.5f, k + 0.5f);
		wp->position = limat_vector_multiply (wp->position, LIAI_WAYPOINT_WIDTH);
		wp->position = limat_vector_add (wp->position, offset);
	}

	/* Build waypoints. */
	liai_sector_build (self, voxels);

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
liai_sector_free (liaiSector* self)
{
	lisys_free (self);
}

/**
 * \brief Rebuilds the waypoints for the sector.
 *
 * \param self Sector.
 * \param voxels Voxel sector or NULL.
 */
void
liai_sector_build (liaiSector*  self,
                   livoxSector* voxels)
{
	liai_sector_build_area (self, voxels, 0, 0, 0,
		LIAI_WAYPOINTS_PER_LINE,
		LIAI_WAYPOINTS_PER_LINE,
		LIAI_WAYPOINTS_PER_LINE);
}

/**
 * \brief Rebuilds the waypoints for the given area.
 *
 * \param self Sector.
 * \param voxels Voxel sector or NULL.
 * \param x Start waypoint.
 * \param y Start waypoint.
 * \param z Start waypoint.
 * \param xs Waypoint count.
 * \param ys Waypoint count.
 * \param zs Waypoint count.
 */
void
liai_sector_build_area (liaiSector*  self,
                        livoxSector* voxels,
                        int          x,
                        int          y,
                        int          z,
                        int          xs,
                        int          ys,
                        int          zs)
{
	int i;
	int j;
	int k;
	liaiWaypoint* wp;
	liaiWaypoint* wp1;
	livoxVoxel* voxel;

	if (voxels != NULL)
	{
		/* Mark ground. */
		for (k = z ; k < z + zs ; k++)
		for (j = y ; j < y + ys ; j++)
		for (i = x ; i < x + xs ; i++)
		{
			wp = self->points + LIAI_WAYPOINT_INDEX (i, j, k);
			voxel = livox_sector_get_voxel (voxels, i, j, k);
			if (voxel->type)
				wp->flags = 0;
			else
				wp->flags = LIAI_WAYPOINT_FLAG_FLYABLE;
		}

		/* Mark walkable . */
#warning Walkability flags are broken for the bottommost plane of the sector
#warning Walkability flags are broken for the topmost plane of previously built blocks
		for (k = z ; k < z + zs ; k++)
		for (j = LI_MAX (y, 1) ; j < y + ys ; j++)
		for (i = x ; i < x + xs ; i++)
		{
			wp = self->points + LIAI_WAYPOINT_INDEX (i, j, k);
			wp1 = self->points + LIAI_WAYPOINT_INDEX (i, j + 1, k);
			if ((wp ->flags & LIAI_WAYPOINT_FLAG_FLYABLE) &&
			   !(wp1->flags & LIAI_WAYPOINT_FLAG_FLYABLE))
				wp->flags |= LIAI_WAYPOINT_FLAG_WALKABLE;
		}
	}
	else
	{
		for (k = z ; k < z + zs ; k++)
		for (j = y ; j < y + ys ; j++)
		for (i = x ; i < x + xs ; i++)
		{
			wp = self->points + LIAI_WAYPOINT_INDEX (i, j, k);
			wp->flags = 0;
		}
	}
}

/**
 * \brief Gets a waypoint be offset.
 *
 * \param self Sector.
 * \param x Waypoint offset.
 * \param y Waypoint offset.
 * \param z Waypoint offset.
 * \return Waypoint.
 */
liaiWaypoint*
liai_sector_get_waypoint (liaiSector* self,
                          int         x,
                          int         y,
                          int         z)
{
	return self->points + LIAI_WAYPOINT_INDEX (x, y, z);
}

/** @} */
/** @} */
