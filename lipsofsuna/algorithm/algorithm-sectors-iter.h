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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup lialgSectorsIter SectorsIter
 * @{
 */

#ifndef __ALGORITHM_SECTORS_ITER_H__
#define __ALGORITHM_SECTORS_ITER_H__

#include "algorithm-range.h"
#include "algorithm-range-iter.h"
#include "algorithm-sectors.h"

typedef struct _lialgSectorsIter lialgSectorsIter;
struct _lialgSectorsIter
{
	lialgRangeIter range;
	lialgSectors* sectors;
	lialgSector* sector;
};

/**
 * \brief Iterates through a range of created sectors.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Sector iterator.
 * \param sectors Sectors.
 * \param x Center sector.
 * \param y Center sector.
 * \param z Center sector.
 * \param radius Search radius in sectors.
 */
#define LIALG_SECTORS_FOREACH(iter, sectors, x, y, z, radius) \
	for (lialg_sector_iter_first (&iter, sectors, x, y, z, radius) ; iter.sector != NULL ; \
	     lialg_sector_iter_next (&iter))

static inline int
lialg_sector_iter_first (lialgSectorsIter* self,
                         lialgSectors*     sectors,
                         int               x,
                         int               y,
                         int               z,
                         int               radius)
{
	lialgRange range;

	/* Initialize self. */
	memset (self, 0, sizeof (lialgSectorsIter));
	self->sectors = sectors;
	range = lialg_range_new_from_center (x, y, z, radius);
	range = lialg_range_clamp (range, 0, sectors->count - 1);
	if (!lialg_range_iter_first (&self->range, &range))
		return 0;

	/* Find first non-empty sector. */
	while (self->range.more)
	{
		self->sector = lialg_sectors_find (sectors, self->range.index);
		lialg_range_iter_next (&self->range);
		if (self->sector != NULL)
			return 1;
	}

	return 0;
}

static inline int
lialg_sector_iter_next (lialgSectorsIter* self)
{
	/* Find next non-empty sector. */
	while (self->range.more)
	{
		self->sector = lialg_sectors_find (self->sectors, self->range.index);
		lialg_range_iter_next (&self->range);
		if (self->sector != NULL)
			return 1;
	}

	return 0;
}

#endif

/** @} */
/** @} */
