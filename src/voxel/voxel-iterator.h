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
#ifndef LIENG_DISABLE_GRAPHICS
#include <render/lips-render.h>
#endif
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

#endif

/** @} */
/** @} */
