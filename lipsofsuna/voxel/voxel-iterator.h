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
 * \addtogroup livoxIterator Iterator
 * @{
 */

#ifndef __VOXEL_ITERATOR_H__
#define __VOXEL_ITERATOR_H__

#include <algorithm/lips-algorithm.h>
#include "voxel.h"
#include "voxel-block.h"
#include "voxel-manager.h"
#include "voxel-sector.h"
#include "voxel-types.h"

typedef struct _livoxBlockIter livoxBlockIter;
struct _livoxBlockIter
{
	int blocks;
	lialgU32dicIter sectors;
	livoxBlock* block;
	int (*filter)(livoxBlock*);
};

/**
 * \brief Iterates through all terrain blocks in all loaded sectors.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Block iterator.
 * \param manager Voxel manager.
 */
#define LIVOX_FOREACH_BLOCK(iter, manager) \
	for (livox_block_iter_first (&iter, manager, NULL, NULL) ; \
	     iter.block != NULL ; \
	     livox_block_iter_next (&iter))

static inline int
livox_block_iter_next (livoxBlockIter* self)
{
	livoxBlock* block;
	livoxSector* sector;

	/* Find next block. */
	for (sector = self->sectors.value ; sector != NULL ; sector = self->sectors.value)
	{
		if (self->blocks < LIVOX_BLOCKS_PER_SECTOR)
		{
			while (++self->blocks < LIVOX_BLOCKS_PER_SECTOR)
			{
				block = livox_sector_get_block (sector, self->blocks);
				if (self->filter == NULL || self->filter (block))
				{
					self->block = block;
					return 1;
				}
			}
		}
		lialg_u32dic_iter_next (&self->sectors);
		self->blocks = 0;
	}

	/* No more sectors. */
	self->block = NULL;

	return 0;
}

static inline int
livox_block_iter_first (livoxBlockIter* self,
                        livoxManager*   manager,
                        void*           filter)
{
	self->filter = filter;
	self->blocks = -1;

	/* Find first sector. */
	lialg_u32dic_iter_start (&self->sectors, manager->sectors);
	if (self->sectors.value == NULL)
	{
		self->block = NULL;
		return 0;
	}

	/* Find first block or object. */
	return livox_block_iter_next (self);
}

/*****************************************************************************/

typedef struct _livoxVoxelIter livoxVoxelIter;
struct _livoxVoxelIter
{
	int load;
	int voxel[3];
	livoxManager* voxels;
	livoxSector* sector;
	lialgRange tiles;
	lialgRange range0;
	lialgRange range1;
	lialgRangeIter rangei0;
	lialgRangeIter rangei1;
};

/**
 * \brief Iterates through voxels in the given range.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Voxel iterator.
 * \param manager Voxel manager.
 * \param range Range in voxels.
 * \param load Nonzero if should load sectors on demand.
 */
#define LIVOX_VOXEL_FOREACH(iter, manager, range, load) \
	for (livox_voxel_iter_first (&(iter), manager, &(range), load) ; \
	     iter.sector != NULL ; \
	     livox_voxel_iter_next (&iter))

static inline int
livox_voxel_iter_first (livoxVoxelIter* self,
                        livoxManager*   voxels,
                        lialgRange*     tiles,
                        int             load)
{
	int ret;
	int offset[3];

	self->voxels = voxels;
	self->load = load;
	self->tiles = *tiles;
	self->sector = NULL;

	/* Initialize sector range. */
	self->range0 = lialg_range_new (
		tiles->minx / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		tiles->miny / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		tiles->minz / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		tiles->maxx / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		tiles->maxy / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		tiles->maxz / (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE));
	self->range0 = lialg_range_clamp (self->range0, 0, 255);

	/* Find first valid sector. */
	if (!lialg_range_iter_first (&self->rangei0, &self->range0))
		return 0;
	while (1)
	{
		if (self->load)
			self->sector = livox_manager_load_sector (self->voxels, self->rangei0.index);
		else
			self->sector = livox_manager_find_sector (self->voxels, self->rangei0.index);
		if (self->sector != NULL)
			break;
		if (!lialg_range_iter_next (&self->rangei0))
		{
			self->sector = NULL;
			return 0;
		}
	}

	/* Initialize voxel range. */
	livox_sector_get_offset (self->sector, offset + 0, offset + 1, offset + 2);
	self->range1 = lialg_range_new (
		self->tiles.minx - offset[0] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.miny - offset[1] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.minz - offset[2] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.maxx - offset[0] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.maxy - offset[1] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.maxz - offset[2] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE));
	self->range1 = lialg_range_clamp (self->range1, 0, LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE - 1);

	/* Find first voxel. */
	ret = lialg_range_iter_first (&self->rangei1, &self->range1);
	assert (ret != 0);
	self->voxel[0] = self->rangei1.x;
	self->voxel[1] = self->rangei1.y;
	self->voxel[2] = self->rangei1.z;

	return 1;
}

static inline int
livox_voxel_iter_next (livoxVoxelIter* self)
{
	int ret;
	int offset[3];

	/* Next voxel. */
	if (lialg_range_iter_next (&self->rangei1))
	{
		self->voxel[0] = self->rangei1.x;
		self->voxel[1] = self->rangei1.y;
		self->voxel[2] = self->rangei1.z;
		return 1;
	}

	/* Next sector. */
	while (1)
	{
		if (!lialg_range_iter_next (&self->rangei0))
		{
			self->sector = NULL;
			return 0;
		}
		if (self->load)
			self->sector = livox_manager_load_sector (self->voxels, self->rangei0.index);
		else
			self->sector = livox_manager_find_sector (self->voxels, self->rangei0.index);
		if (self->sector != NULL)
			break;
	}

	/* Initialize voxel range. */
	livox_sector_get_offset (self->sector, offset + 0, offset + 1, offset + 2);
	self->range1 = lialg_range_new (
		self->tiles.minx - offset[0] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.miny - offset[1] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.minz - offset[2] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.maxx - offset[0] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.maxy - offset[1] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE),
		self->tiles.maxz - offset[2] * (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE));
	self->range1 = lialg_range_clamp (self->range1, 0, LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE - 1);

	/* Find first voxel. */
	ret = lialg_range_iter_first (&self->rangei1, &self->range1);
	assert (ret != 0);
	self->voxel[0] = self->rangei1.x;
	self->voxel[1] = self->rangei1.y;
	self->voxel[2] = self->rangei1.z;

	return 1;
}

#endif

/** @} */
/** @} */
