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

#include <system/lips-system.h>
#include "ai-manager.h"
#include "ai-sector.h"

static float
private_astar_cost (liaiManager*  self,
                    void*         object,
                    liaiWaypoint* start,
                    liaiWaypoint* end);

static float
private_astar_heuristic (liaiManager*  self,
                         void*         object,
                         liaiWaypoint* start,
                         liaiWaypoint* end);

static int
private_astar_passable (liaiManager*  self,
                        void*         object,
                        liaiWaypoint* start,
                        liaiWaypoint* end);

static void*
private_astar_successor (liaiManager*  self,
                         void*         object,
                         liaiWaypoint* node,
                         int           index);

static int
private_block_load (liaiManager*      self,
                    livoxUpdateEvent* event);

static liaiPath*
private_solve_path (liaiManager*  self,
                    liaiWaypoint* start,
                    liaiWaypoint* end);

/*****************************************************************************/

/**
 * \brief Creates a new AI manager.
 *
 * \param callbacks Callbacks.
 * \param sectors Sector manager.
 * \param voxels Voxel manager or NULL.
 * \return New AI manager or NULL.
 */
liaiManager*
liai_manager_new (licalCallbacks* callbacks,
                  lialgSectors*   sectors,
                  livoxManager*   voxels)
{
	liaiManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liaiManager));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;
	self->sectors = sectors;
	self->voxels = voxels;

	/* Register sector content. */
	if (!lialg_sectors_insert_content (self->sectors, "ai", self,
		(lialgSectorFreeFunc) liai_sector_free,
		(lialgSectorLoadFunc) liai_sector_new))
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate path solver. */
	self->astar = lialg_astar_new (
		(lialgAstarCost) private_astar_cost,
		(lialgAstarHeuristic) private_astar_heuristic,
		(lialgAstarPassable) private_astar_passable,
		(lialgAstarSuccessor) private_astar_successor);
	if (self->astar == NULL)
	{
		lialg_sectors_remove_content (self->sectors, "ai");
		lisys_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (self->voxels != NULL)
		lical_callbacks_insert (self->voxels->callbacks, self->voxels, "load-block", 1, private_block_load, self, self->calls + 0);

	return self;
}

/**
 * \brief Frees the AI manager.
 *
 * \param self AI manager.
 */
void
liai_manager_free (liaiManager* self)
{
	/* Unregister callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (licalHandle));

	/* unregister sector content. */
	if (self->sectors != NULL)
		lialg_sectors_remove_content (self->sectors, "ai");

	/* Free path solver. */
	if (self->astar != NULL)
		lialg_astar_free (self->astar);

	lisys_free (self);
}

liaiWaypoint*
liai_manager_find_waypoint (liaiManager*       self,
                            const limatVector* point)
{
	int x;
	int y;
	int z;
	int sx;
	int sy;
	int sz;
	liaiSector* sector;

	x = point->x / LIAI_WAYPOINT_WIDTH;
	y = point->y / LIAI_WAYPOINT_WIDTH;
	z = point->z / LIAI_WAYPOINT_WIDTH;
	sx = x / LIAI_WAYPOINTS_PER_LINE;
	sy = y / LIAI_WAYPOINTS_PER_LINE;
	sz = z / LIAI_WAYPOINTS_PER_LINE;
	x %= LIAI_WAYPOINTS_PER_LINE;
	y %= LIAI_WAYPOINTS_PER_LINE;
	z %= LIAI_WAYPOINTS_PER_LINE;

	sector = lialg_sectors_data_offset (self->sectors, "ai", sx, sy, sz, 0);
	if (sector == NULL)
		return NULL;

	return liai_sector_get_waypoint (sector, x, y, z);
}

/**
 * \brief Solves path from the starting point to the end point.
 *
 * \param self AI manager.
 * \param start Start position.
 * \param end Target position.
 * \return New path or NULL.
 */
liaiPath*
liai_manager_solve_path (liaiManager*       self,
                         const limatVector* start,
                         const limatVector* end)
{
	liaiWaypoint* wp0;
	liaiWaypoint* wp1;

	wp0 = liai_manager_find_waypoint (self, start);
	wp1 = liai_manager_find_waypoint (self, end);
	if (wp0 == NULL || wp1 == NULL)
		return NULL;

	return private_solve_path (self, wp0, wp1);
}

/*****************************************************************************/

static float
private_astar_cost (liaiManager*  self,
                    void*         object,
                    liaiWaypoint* start,
                    liaiWaypoint* end)
{
	limatVector diff;

	diff = limat_vector_subtract (start->position, end->position);

	/* FIXME: Gives lots of penalty for climbing. */
	return limat_vector_get_length (diff) + 50.0f * LI_MAX (0, diff.y);
}

static float
private_astar_heuristic (liaiManager*  self,
                         void*         object,
                         liaiWaypoint* start,
                         liaiWaypoint* end)
{
	return LI_ABS (start->position.x - end->position.x) +
	       LI_ABS (start->position.y - end->position.y) +
	       LI_ABS (start->position.z - end->position.z);
}

static int
private_astar_passable (liaiManager*  self,
                        void*         object,
                        liaiWaypoint* start,
                        liaiWaypoint* end)
{
	/* FIXME: Check for object size. */
	return 1;
}

static void*
private_astar_successor (liaiManager*  self,
                         void*         object,
                         liaiWaypoint* node,
                         int           index)
{
	int i;
	int x;
	int y;
	int z;
	int sx;
	int sy;
	int sz;
	int pos;
	liaiWaypoint* wp;
	liaiSector* sector;
	static const int rel[26][3] =
	{
		{ -1,  0, -1 },
		{  0,  0, -1 },
		{  1,  0, -1 },
		{ -1,  0,  0 },
		{  1,  0,  0 },
		{ -1,  0,  1 },
		{  0,  0,  1 },
		{  1,  0,  1 },
		{ -1, -1, -1 },
		{  0, -1, -1 },
		{  1, -1, -1 },
		{ -1, -1,  0 },
		{  0, -1,  0 },
		{  1, -1,  0 },
		{ -1, -1,  1 },
		{  0, -1,  1 },
		{  1, -1,  1 },
		{ -1,  1, -1 },
		{  0,  1, -1 },
		{  1,  1, -1 },
		{ -1,  1,  0 },
		{  0,  1,  0 },
		{  1,  1,  0 },
		{ -1,  1,  1 },
		{  0,  1,  1 },
		{  1,  1,  1 }
	};

	for (i = pos = 0 ; i < 26 ; i++)
	{
		/* Find next node. */
		x = node->x + rel[i][0];
		y = node->y + rel[i][1];
		z = node->z + rel[i][2];
		sx = node->sector->sector->x;
		sy = node->sector->sector->y;
		sz = node->sector->sector->z;
		if (x < 0) { x += LIAI_WAYPOINTS_PER_LINE; sx--; }
		if (y < 0) { y += LIAI_WAYPOINTS_PER_LINE; sy--; }
		if (z < 0) { z += LIAI_WAYPOINTS_PER_LINE; sz--; }
		if (x >= LIAI_WAYPOINTS_PER_LINE) { x -= LIAI_WAYPOINTS_PER_LINE; sx++; }
		if (y >= LIAI_WAYPOINTS_PER_LINE) { y -= LIAI_WAYPOINTS_PER_LINE; sy++; }
		if (z >= LIAI_WAYPOINTS_PER_LINE) { z -= LIAI_WAYPOINTS_PER_LINE; sz++; }

		/* Find next sector. */
		sector = node->sector;
		if (sx != sector->sector->x || sy != sector->sector->y || sz != sector->sector->z)
		{
			sector = lialg_sectors_data_offset (sector->sector->manager, "ai", sx, sy, sz, 0);
			if (sector == NULL)
				continue;
		}

		/* Get next node. */
		/* FIXME: No support for flying monsters. */
		wp = liai_sector_get_waypoint (sector, x, y, z);
#warning Walkability flags are broken so path solving assumes all monsters can fly.
		if (wp->flags & LIAI_WAYPOINT_FLAG_FLYABLE)
/*		if ((wp->flags & LIAI_WAYPOINT_FLAG_WALKABLE) ||
		   ((wp->flags & LIAI_WAYPOINT_FLAG_FLYABLE) && y < 0))*/
		{
			if (pos++ == index)
				return wp;
		}
	}

	return NULL;
}

static int
private_block_load (liaiManager*      self,
                    livoxUpdateEvent* event)
{
	liaiSector* ai;
	lialgSector* sector;
	livoxSector* voxel;

	/* Find or create sector. */
	sector = lialg_sectors_sector_offset (self->sectors, event->sector[0], event->sector[1], event->sector[2], 1);
	if (sector == NULL)
		return 1;
	ai = lialg_strdic_find (sector->content, "ai");
	voxel = lialg_strdic_find (sector->content, "voxel");
	if (ai == NULL || voxel == NULL)
		return 1;

	/* Build waypoints. */
	liai_sector_build_area (ai, voxel, event->block[0], event->block[1], event->block[2],
		LIVOX_TILES_PER_LINE, LIVOX_TILES_PER_LINE, LIVOX_TILES_PER_LINE);

	return 1;
}

/**
 * \brief Solves path from the starting point to the end point.
 *
 * \param self AI manager.
 * \param start Start waypoint.
 * \param end Target waypoint.
 * \return New path or NULL.
 */
static liaiPath*
private_solve_path (liaiManager*  self,
                    liaiWaypoint* start,
                    liaiWaypoint* end)
{
	int i;
	liaiPath* path;
	liaiWaypoint* point;
	lialgAstarResult* result;

	/* Solve path, */
	result = lialg_astar_solve (self->astar, self, NULL, start, end);
	if (result == NULL)
		return NULL;

	/* Allocate path. */
	path = liai_path_new ();
	if (path == NULL)
	{
		lialg_astar_result_free (result);
		return NULL;
	}

	/* Convert results. */
	for (i = 0 ; i < result->length ; i++)
	{
		point = result->nodes[i];
		if (!liai_path_append_point (path, &point->position))
		{
			lialg_astar_result_free (result);
			liai_path_free (path);
			return NULL;
		}
	}
	lialg_astar_result_free (result);

	return path;
}

/** @} */
/** @} */
