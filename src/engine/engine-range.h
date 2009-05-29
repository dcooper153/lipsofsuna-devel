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
 * \param unit Bin side length.
 * \param mini Minimum range coordinate.
 * \param maxi Maximum range coordinate.
 * \return Range.
 */
static inline liengRange
lieng_range_new (int x,
                 int y,
                 int z,
                 int radius,
                 int mini,
                 int maxi)
{
	int size;
	liengRange self;

	size = maxi - mini;
	self.min = mini;
	self.max = maxi;
	self.minx = x;
	self.miny = y;
	self.minz = z;
	self.maxx = LI_MIN (self.minx + radius, maxi);
	self.maxy = LI_MIN (self.miny + radius, maxi);
	self.maxz = LI_MIN (self.minz + radius, maxi);
	self.minx = LI_MAX (self.minx - radius, mini);
	self.miny = LI_MAX (self.miny - radius, mini);
	self.minz = LI_MAX (self.minz - radius, mini);

	return self;
}

/**
 * \brief Creates a three-dimensional bin range that enclose an AABB.
 *
 * \param min Minimum point of the AABB.
 * \param max Maximum point of the AABB.
 * \param unit Bin side length.
 * \param mini Minimum range coordinate.
 * \param maxi Maximum range coordinate.
 * \return Range.
 */
static inline liengRange
lieng_range_new_from_aabb (const limatVector* min,
                           const limatVector* max,
                           float              unit,
                           int                mini,
                           int                maxi)
{
	liengRange self;

	self.min = mini;
	self.max = maxi;
	self.minx = (int)(min->x / unit);
	self.miny = (int)(min->y / unit);
	self.minz = (int)(min->z / unit);
	self.maxx = (int)(max->x / unit);
	self.maxy = (int)(max->y / unit);
	self.maxz = (int)(max->z / unit);
	if (self.maxx * unit < max->x) self.maxx++;
	if (self.maxy * unit < max->y) self.maxy++;
	if (self.maxz * unit < max->z) self.maxz++;
	self.minx = LI_MAX (self.minx, mini);
	self.miny = LI_MAX (self.miny, mini);
	self.minz = LI_MAX (self.minz, mini);
	self.maxx = LI_MIN (self.maxx, maxi);
	self.maxy = LI_MIN (self.maxy, maxi);
	self.maxz = LI_MIN (self.maxz, maxi);

	return self;
}

/**
 * \brief Creates a three-dimensional bin range that enclose a sphere.
 *
 * \param index Index of the center bin.
 * \param radius Radius of the sphere in bins.
 * \param unit Bin side length.
 * \param mini Minimum range coordinate.
 * \param maxi Maximum range coordinate.
 * \return Range.
 */
static inline liengRange
lieng_range_new_from_index (int index,
                            int radius,
                            int mini,
                            int maxi)
{
	int size;
	liengRange self;

	size = maxi - mini;
	self.min = mini;
	self.max = maxi;
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
 * \param mini Minimum range coordinate.
 * \param maxi Maximum range coordinate.
 * \return Range.
 */
static inline liengRange
lieng_range_new_from_sphere (const limatVector* center,
                             float              radius,
                             float              unit,
                             int                mini,
                             int                maxi)
{
	limatVector min;
	limatVector max;
	liengRange self;

	min = limat_vector_subtract (*center, limat_vector_init (radius, radius, radius));
	max = limat_vector_add (*center, limat_vector_init (radius, radius, radius));
	self = lieng_range_new_from_aabb (&min, &max, unit, mini, maxi);

	return self;
}

static inline int
lieng_range_contains (const liengRange* self,
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

static inline int
lieng_range_contains_index (const liengRange* self,
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

	return lieng_range_contains (self, x, y, z);
}

#endif

/** @} */
/** @} */
