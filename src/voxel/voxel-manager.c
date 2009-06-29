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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxManager Manager
 * @{
 */

#include "voxel-manager.h"
#include "voxel-private.h"

static void
private_clear_sectors (livoxManager* self);

static void
private_mark_block (livoxManager* self,
                    livoxSector*  sector,
                    int           x,
                    int           y,
                    int           z);

/*****************************************************************************/

#ifndef LIVOX_DISABLE_GRAPHICS
livoxManager*
livox_manager_new (liphyPhysics* physics,
                   lirndRender*  render,
                   lirndApi*     rndapi)
#else
livoxManager*
livox_manager_new (liphyPhysics* physics)
#endif
{
	livoxManager* self;

	self = calloc (1, sizeof (livoxManager));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->render = render;
	self->renderapi = rndapi;
	self->sectors = lialg_u32dic_new ();
	if (self->sectors == NULL)
	{
		free (self);
		return NULL;
	}

	return self;
}

void
livox_manager_free (livoxManager* self)
{
	if (self->sectors != NULL)
	{
		private_clear_sectors (self);
		lialg_u32dic_free (self->sectors);
	}
	free (self);
}

/**
 * \brief Creates an empty sector.
 *
 * If a sector with the same number already exists, it is returned instead of
 * creating a new one.
 *
 * \brief Voxel manager.
 * \param id Sector index.
 * \return Sector owned by the manager or NULL.
 */
livoxSector*
livox_manager_create_sector (livoxManager* self,
                             uint32_t      id)
{
	livoxSector* sector;

	sector = livox_manager_find_sector (self, id);
	if (sector != NULL)
		return sector;
	return livox_sector_new (self, id);
}

livoxSector*
livox_manager_find_sector (livoxManager* self,
                           uint32_t      id)
{
	return lialg_u32dic_find (self->sectors, id);
}

/**
 * \brief Loads a sector from an SQL database.
 *
 * If a sector with the same number already exists, no loading takes place.
 * If a sector with the requested ID cannot be found from the database,
 * an empty sector is created.
 *
 * \brief Voxel manager.
 * \param id Sector index.
 * \param sql Databaes.
 * \return Sector owned by the manager or NULL.
 */
livoxSector*
livox_manager_load_sector (livoxManager* self,
                           uint32_t      id,
                           liarcSql*     sql)
{
	livoxSector* sector;

	sector = livox_manager_find_sector (self, id);
	if (sector != NULL)
		return sector;
	sector = livox_sector_new (self, id);
	if (sector == NULL)
		return NULL;
	livox_sector_read (sector, sql);

	return sector;
}

void
livox_manager_update (livoxManager* self,
                      float         secs)
{
	int i;
	int j;
	int x;
	int y;
	int z;
	lialgU32dicIter iter;
	livoxSector* sector;
	struct
	{
		int x;
		int y;
		int z;
		int mask;
	}
	neighbors[26] =
	{
		{ -1, -1, -1, 0x01|0x04|0x10 },
		{  0, -1, -1, 0x00|0x04|0x10 },
		{  1, -1, -1, 0x02|0x04|0x10 },
		{ -1,  0, -1, 0x01|0x00|0x10 },
		{  0,  0, -1, 0x00|0x00|0x10 },
		{  1,  0, -1, 0x02|0x00|0x10 },
		{ -1,  1, -1, 0x01|0x08|0x10 },
		{  0,  1, -1, 0x00|0x08|0x10 },
		{  1,  1, -1, 0x02|0x08|0x10 },
		{ -1, -1,  0, 0x01|0x04|0x00 },
		{  0, -1,  0, 0x00|0x04|0x00 },
		{  1, -1,  0, 0x02|0x04|0x00 },
		{ -1,  0,  0, 0x01|0x00|0x00 },
		{  1,  0,  0, 0x02|0x00|0x00 },
		{ -1,  1,  0, 0x01|0x08|0x00 },
		{  0,  1,  0, 0x00|0x08|0x00 },
		{  1,  1,  0, 0x02|0x08|0x00 },
		{ -1, -1,  1, 0x01|0x04|0x20 },
		{  0, -1,  1, 0x00|0x04|0x20 },
		{  1, -1,  1, 0x02|0x04|0x20 },
		{ -1,  0,  1, 0x01|0x00|0x20 },
		{  0,  0,  1, 0x00|0x00|0x20 },
		{  1,  0,  1, 0x02|0x00|0x20 },
		{ -1,  1,  1, 0x01|0x08|0x20 },
		{  0,  1,  1, 0x00|0x08|0x20 },
		{  1,  1,  1, 0x02|0x08|0x20 },
	};

	/* Update block boundaries. */
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		if (!sector->dirty)
			continue;
		for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
		{
			for (j = 0 ; j < 26 ; j++)
			{
				if ((sector->blocks[i].dirty & neighbors[j].mask) != neighbors[j].mask)
					continue;
				private_mark_block (self, sector,
					neighbors[j].x + x,
					neighbors[j].y + y,
					neighbors[j].z + z);
			}
		}
	}

	/* Rebuild modified terrain. */
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		if (!sector->dirty)
			continue;
		for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
		{
			if (!sector->blocks[i].dirty)
				continue;
			livox_sector_build_block (sector, x, y, z);
			sector->blocks[i].dirty = 0;
		}
		sector->dirty = 0;
	}
}

/*****************************************************************************/

static void
private_clear_sectors (livoxManager* self)
{
	lialgU32dicIter iter;
	livoxSector* sector;

	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		livox_sector_free (sector);
	}
	lialg_u32dic_clear (self->sectors);
}

static void
private_mark_block (livoxManager* self,
                    livoxSector*  sector,
                    int           x,
                    int           y,
                    int           z)
{
	int sx;
	int sy;
	int sz;
	uint32_t id;
	livoxSector* sector1;

	/* Find affected sector. */
	sx = sector->x;
	sy = sector->y;
	sz = sector->z;
	if (x < 0)
	{
		x = LIVOX_BLOCKS_PER_LINE - 1;
		sx--;
	}
	else if (x >= LIVOX_BLOCKS_PER_LINE)
	{
		x = 0;
		sx++;
	}
	if (y < 0)
	{
		y = LIVOX_BLOCKS_PER_LINE - 1;
		sy--;
	}
	else if (y >= LIVOX_BLOCKS_PER_LINE)
	{
		y = 0;
		sy++;
	}
	if (z < 0)
	{
		z = LIVOX_BLOCKS_PER_LINE - 1;
		sz--;
	}
	else if (z >= LIVOX_BLOCKS_PER_LINE)
	{
		z = 0;
		sz++;
	}
	id = LIVOX_SECTOR_INDEX (sx, sy, sz);

	/* Mark block as dirty. */
	sector1 = livox_manager_find_sector (self, id);
	if (sector1 == NULL)
		return;
	sector1->blocks[LIVOX_BLOCK_INDEX (x, y, z)].dirty |= 0x80;
	sector1->dirty = 1;
}

/** @} */
/** @} */
