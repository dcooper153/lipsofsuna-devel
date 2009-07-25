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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengIterator Iterator
 * @{
 */

#ifndef __ENGINE_ITERATOR_H__
#define __ENGINE_ITERATOR_H__

#include <algorithm/lips-algorithm.h>
#include "engine-range.h"
#include "engine-sector.h"
#include "engine-selection.h"

/**
 * \brief Iterates through a range of bins in 3D.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Range iterator.
 * \param range Iterated range.
 */
#define LIENG_FOREACH_RANGE(iter, range) \
	for (lieng_range_iter_first (&iter, &range) ; iter.more ; \
	     lieng_range_iter_next (&iter))

typedef struct _liengRangeIter liengRangeIter;
struct _liengRangeIter
{
	int index;
	int x;
	int y;
	int z;
	int more;
	liengRange range;
};

static inline int
lieng_range_iter_first (liengRangeIter* self,
                        liengRange*     range)
{
	int r = range->max - range->min;

	/* Find first bin. */
	self->range = *range;
	self->x = range->minx;
	self->y = range->miny;
	self->z = range->minz;
	if (range->minx > range->maxx ||
	    range->miny > range->maxy ||
	    range->minz > range->maxz)
	{
		self->more = 0;
		return 0;
	}

	/* Calculate bin index. */
	self->more = 1;
	self->index =
		(self->x - self->range.min) +
		(self->y - self->range.min) * r +
		(self->z - self->range.min) * r * r;

	return 1;
}

static inline int
lieng_range_iter_next (liengRangeIter* self)
{
	int r = self->range.max - self->range.min;

	/* Find next bin. */
	if (++self->x > self->range.maxx)
	{
		self->x = self->range.minx;
		if (++self->y > self->range.maxy)
		{
			self->y = self->range.miny;
			if (++self->z > self->range.maxz)
			{
				self->more = 0;
				return 0;
			}
		}
	}

	/* Calculate bin index. */
	self->index =
		(self->x - self->range.min) +
		(self->y - self->range.min) * r +
		(self->z - self->range.min) * r * r;

	return 1;
}

/*****************************************************************************/

typedef struct _liengSectorIter liengSectorIter;
struct _liengSectorIter
{
	liengRangeIter range;
	liengEngine* engine;
	liengSector* sector;
};

/**
 * \brief Iterates through a range of loaded sectors.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Sector iterator.
 * \param engine Engine.
 * \param x Center sector.
 * \param y Center sector.
 * \param z Center sector.
 * \param radius Search radius in sectors.
 */
#define LIENG_FOREACH_SECTOR(iter, engine, x, y, z, radius) \
	for (lieng_sector_iter_first (&iter, engine, x, y, z, radius) ; iter.sector != NULL ; \
	     lieng_sector_iter_next (&iter))

static inline int
lieng_sector_iter_first (liengSectorIter* self,
                         liengEngine*     engine,
                         int              x,
                         int              y,
                         int              z,
                         int              radius)
{
	liengRange range;

	/* Initialize self. */
	memset (self, 0, sizeof (liengSectorIter));
	self->engine = engine;
	range = lieng_range_new (x, y, z, radius, 0, 256);
	if (!lieng_range_iter_first (&self->range, &range))
		return 0;

	/* Find first non-empty sector. */
	while (self->range.more)
	{
		self->sector = lieng_engine_find_sector (engine, self->range.index);
		lieng_range_iter_next (&self->range);
		if (self->sector != NULL)
			return 1;
	}

	return 0;
}

static inline int
lieng_sector_iter_next (liengSectorIter* self)
{
	/* Find next non-empty sector. */
	while (self->range.more)
	{
		self->sector = lieng_engine_find_sector (self->engine, self->range.index);
		lieng_range_iter_next (&self->range);
		if (self->sector != NULL)
			return 1;
	}

	return 0;
}

/*****************************************************************************/

typedef struct _liengObjectIter liengObjectIter;
struct _liengObjectIter
{
	liengSectorIter sectors;
	lialgU32dicIter objects;
	liengObject* object;
};

/**
 * \brief Iterates through objects in a range of sectors.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Object iterator.
 * \param engine Engine.
 * \param x Center sector.
 * \param y Center sector.
 * \param z Center sector.
 * \param radius Search radius in sectors.
 */
#define LIENG_FOREACH_OBJECT(iter, engine, x, y, z, radius) \
	for (lieng_object_iter_first (&iter, engine, x, y, z, radius) ; iter.object != NULL ; \
	     lieng_object_iter_next (&iter))

static inline int
lieng_object_iter_first (liengObjectIter* self,
                         liengEngine*     engine,
                         int              x,
                         int              y,
                         int              z,
                         int              radius)
{
	/* Initialize self. */
	memset (self, 0, sizeof (liengObjectIter));
	if (!lieng_sector_iter_first (&self->sectors, engine, x, y, z, radius))
		return 0;

	/* Find first object. */
	while (self->sectors.sector != NULL)
	{
		lialg_u32dic_iter_start (&self->objects, self->sectors.sector->objects);
		if (self->objects.value != NULL)
		{
			self->object = self->objects.value;
			return 1;
		}
		if (!lieng_sector_iter_next (&self->sectors))
			break;
	}

	return 0;
}

static inline int
lieng_object_iter_next (liengObjectIter* self)
{
	self->object = NULL;

	/* Find next object in sector. */
	while (self->objects.value != NULL)
	{
		if (lialg_u32dic_iter_next (&self->objects))
		{
			self->object = self->objects.value;
			return 1;
		}
	}

	/* Find next object in another sector. */
	while (lieng_sector_iter_next (&self->sectors))
	{
		lialg_u32dic_iter_start (&self->objects, self->sectors.sector->objects);
		if (self->objects.value != NULL)
		{
			self->object = self->objects.value;
			return 1;
		}
	}

	return 0;
}

/*****************************************************************************/

typedef struct _liengSelectionIter liengSelectionIter;
struct _liengSelectionIter
{
	lialgPtrdicIter objects;
	liengObject* object;
};

/**
 * \brief Iterates through all currently selected objects.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Selection iterator.
 * \param engine Engine.
 */
#define LIENG_FOREACH_SELECTION(iter, engine) \
	for (lieng_selection_iter_first (&iter, engine) ; iter.object != NULL ; \
	     lieng_selection_iter_next (&iter))

static inline int
lieng_selection_iter_first (liengSelectionIter* self,
                            liengEngine*        engine)
{
	liengSelection* selection;

	/* Find first object. */
	lialg_ptrdic_iter_start (&self->objects, engine->selection);
	if (self->objects.value != NULL)
	{
		selection = self->objects.value;
		self->object = selection->object;
		return 1;
	}
	else
	{
		self->object = NULL;
		return 1;
	}
}

static inline int
lieng_selection_iter_next (liengSelectionIter* self)
{
	liengSelection* selection;

	if (lialg_ptrdic_iter_next (&self->objects))
	{
		selection = self->objects.value;
		self->object = selection->object;
		return 1;
	}
	else
	{
		self->object = NULL;
		return 0;
	}
}

#endif

/** @} */
/** @} */
