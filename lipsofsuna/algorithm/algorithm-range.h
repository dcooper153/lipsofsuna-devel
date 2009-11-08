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

#ifndef __ALGORITHM_RANGE_H__
#define __ALGORITHM_RANGE_H__

/**
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup lialgRange Range
 * @{
 */

#define LIALG_RANGE_DEFAULT_SIZE 1024

typedef struct _lialgRange lialgRange;
struct _lialgRange
{
	int min;
	int max;
	int minx;
	int miny;
	int minz;
	int maxx;
	int maxy;
	int maxz;
};

/**
 * \brief Creates a three-dimensional bin range that enclose a sphere.
 *
 * \param x Center bin offset.
 * \param y Center bin offset.
 * \param z Center bin offset.
 * \param radius Radius of the sphere in bins.
 * \return Range.
 */
static inline lialgRange
lialg_range_new (int x,
                 int y,
                 int z,
                 int radius)
{
	lialgRange self;

	self.min = 0;
	self.max = LIALG_RANGE_DEFAULT_SIZE + 1;
	self.minx = x - radius;
	self.miny = y - radius;
	self.minz = z - radius;
	self.maxx = x + radius;
	self.maxy = y + radius;
	self.maxz = z + radius;

	return self;
}

/**
 * \brief Creates a three-dimensional bin range that enclose an AABB.
 *
 * \param min Minimum point of the AABB.
 * \param max Maximum point of the AABB.
 * \param unit Bin side length.
 * \return Range.
 */
static inline lialgRange
lialg_range_new_from_aabb (const limatVector* min,
                           const limatVector* max,
                           float              unit)
{
	lialgRange self;

	self.min = 0;
	self.max = LIALG_RANGE_DEFAULT_SIZE + 1;
	self.minx = (int)(min->x / unit);
	self.miny = (int)(min->y / unit);
	self.minz = (int)(min->z / unit);
	self.maxx = (int)(max->x / unit);
	self.maxy = (int)(max->y / unit);
	self.maxz = (int)(max->z / unit);

	return self;
}

/**
 * \brief Creates a three-dimensional bin range that enclose a sphere.
 *
 * \param index Index of the center bin.
 * \param radius Radius of the sphere in bins.
 * \param mini Minimum range coordinate.
 * \param maxi Maximum range coordinate.
 * \return Range.
 */
static inline lialgRange
lialg_range_new_from_index (int index,
                            int radius,
                            int mini,
                            int maxi)
{
	int size;
	lialgRange self;

	size = maxi - mini;
	self.min = mini;
	self.max = maxi + 1;
	self.minx = index % size;
	self.miny = index / size % size;
	self.minz = index / size / size;
	self.maxx = LI_MIN (self.minx + radius, maxi);
	self.maxy = LI_MIN (self.miny + radius, maxi);
	self.maxz = LI_MIN (self.minz + radius, maxi);
	self.minx = LI_MAX (self.minx - radius, mini);
	self.miny = LI_MAX (self.miny - radius, mini);
	self.minz = LI_MAX (self.minz - radius, mini);

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
static inline lialgRange
lialg_range_new_from_sphere (const limatVector* center,
                             float              radius,
                             float              unit)
{
	limatVector min;
	limatVector max;
	lialgRange self;

	min = limat_vector_subtract (*center, limat_vector_init (radius, radius, radius));
	max = limat_vector_add (*center, limat_vector_init (radius, radius, radius));
	self = lialg_range_new_from_aabb (&min, &max, unit);

	return self;
}

/**
 * \brief Clamps the range.
 *
 * \param self Range.
 * \param min Minimum range coordinate.
 * \param min Maximum range coordinate.
 * \return New range.
 */
static inline lialgRange
lialg_range_clamp (const lialgRange self,
                   int              min,
                   int              max)
{
	lialgRange ret;

	ret.min = min;
	ret.max = max + 1;
	ret.minx = LI_MAX (self.minx, min);
	ret.miny = LI_MAX (self.miny, min);
	ret.minz = LI_MAX (self.minz, min);
	ret.maxx = LI_MIN (self.maxx, max);
	ret.maxy = LI_MIN (self.maxy, max);
	ret.maxz = LI_MIN (self.maxz, max);

	return ret;
}

/**
 * \brief Checks if the range contains the given bin.
 *
 * \param self Range.
 * \param x Bin coordinate.
 * \param y Bin coordinate.
 * \param z Bin coordinate.
 */
static inline int
lialg_range_contains (const lialgRange* self,
                      int               x,
                      int               y,
                      int               z)
{
	if (self->minx <= x && x <= self->maxx &&
	    self->miny <= y && y <= self->maxy &&
	    self->minz <= z && z <= self->maxz)
		return 1;

	return 0;
}

/**
 * \brief Checks if the range contains the given index.
 *
 * \param self Range.
 * \param index Bin index.
 */
static inline int
lialg_range_contains_index (const lialgRange* self,
                            int               index)
{
	int x;
	int y;
	int z;
	int size;

	size = self->max - self->min;
	x = index % size;
	y = index / size % size;
	z = index / size / size;

	return lialg_range_contains (self, x, y, z);
}

#endif

/** @} */
/** @} */
