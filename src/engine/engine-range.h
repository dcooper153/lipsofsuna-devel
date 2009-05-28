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

#ifndef __ENGINE_RANGE_H__
#define __ENGINE_RANGE_H__

/**
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengRange Range
 * @{
 */

typedef struct _liengRange liengRange;
struct _liengRange
{
	int minx;
	int miny;
	int minz;
	int maxx;
	int maxy;
	int maxz;
};

/**
 * \brief Creates a three-dimensional bin range that enclose an AABB.
 *
 * \param min Minimum point of the AABB.
 * \param max Maximum point of the AABB.
 * \param unit Bin side length.
 * \return Range.
 */
static inline liengRange
lieng_range_new_from_aabb (const limatVector* min,
                           const limatVector* max,
                           float              unit)
{
	liengRange self;

	self.minx = (int)(min->x / unit);
	self.miny = (int)(min->y / unit);
	self.minz = (int)(min->z / unit);
	self.maxx = (int)(max->x / unit);
	self.maxy = (int)(max->y / unit);
	self.maxz = (int)(max->z / unit);
	if (self.maxx * unit < max->x) self.maxx++;
	if (self.maxy * unit < max->y) self.maxy++;
	if (self.maxz * unit < max->z) self.maxz++;

	return self;
}

/**
 * \brief Creates a three-dimensional bin range that enclose a sphere.
 *
 * \param center Center point of the sphere.
 * \param radius Radius of the sphere.
 * \param unit Bin side length.
 * \return Range.
 */
static inline liengRange
lieng_range_new_from_sphere (const limatVector* center,
                             float              radius,
                             float              unit)
{
	limatVector min;
	limatVector max;
	liengRange self;

	min = limat_vector_subtract (*center, limat_vector_init (radius, radius, radius));
	max = limat_vector_add (*center, limat_vector_init (radius, radius, radius));
	self = lieng_range_new_from_aabb (&min, &max, unit);

	return self;
}

/*****************************************************************************/

#define LIENG_FOREACH_RANGE(iter, range) \
	for (lieng_range_iter_first (&iter, &range) ; iter.more ; \
	     lieng_range_iter_next (&iter))

typedef struct _liengRangeIter liengRangeIter;
struct _liengRangeIter
{
	int x;
	int y;
	int z;
	int more;
	liengRange range;
};

static inline void
lieng_range_iter_first (liengRangeIter* self,
                        liengRange*     range)
{
	self->range = *range;
	self->x = range->minx;
	self->y = range->miny;
	self->z = range->minz;
	self->more = 1;
}

static inline int
lieng_range_iter_next (liengRangeIter* self)
{
	if (self->x >= self->range.maxx)
	{
		self->x = self->range.minx;
		if (self->y >= self->range.maxy)
		{
			self->y = self->range.miny;
			if (self->z >= self->range.maxz)
			{
				self->more = 0;
				return 0;
			}
			else
				self->z++;
		}
		else
			self->y++;
	}
	else
		self->x++;
	return 1;
}

#endif

/** @} */
/** @} */
