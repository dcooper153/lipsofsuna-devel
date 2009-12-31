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
 * \addtogroup lialgSectors Sectors
 * @{
 */

#include <system/lips-system.h>
#include "algorithm-sectors.h"

lialgSectors*
lialg_sectors_new (int   count,
                   float width)
{
	lialgSectors* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lialgSectors));
	if (self == NULL)
		return NULL;
	self->count = count;
	self->width = width;

	/* Allocate dictionary. */
	self->sectors = lialg_u32dic_new ();
	if (self->sectors == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
lialg_sectors_free (lialgSectors* self)
{
	lialg_u32dic_free (self->sectors);
	lisys_free (self);
}

lialgSector*
lialg_sectors_create (lialgSectors* self,
                      int           index)
{
	lialgSector* sector;

	sector = lisys_calloc (1, sizeof (lialgSector));
	if (sector == NULL)
		return NULL;
	sector->index = index;
	lialg_sectors_index_to_offset (self, index, &sector->x, &sector->y, &sector->z);
	if (!lialg_u32dic_insert (self->sectors, index, sector))
	{
		lisys_free (sector);
		return NULL;
	}

	return sector;
}

lialgSector*
lialg_sectors_find (lialgSectors* self,
                    int           index)
{
	return lialg_u32dic_find (self->sectors, index);
}

void
lialg_sectors_index_to_offset (lialgSectors* self,
                               int           index,
                               int*          x,
                               int*          y,
                               int*          z)
{
	*x = index % self->count;
	*y = index / self->count % self->count;
	*z = index / self->count / self->count % self->count;
}

int
lialg_sectors_offset_to_index (lialgSectors* self,
                               int           x,
                               int           y,
                               int           z)
{
	return x + (y + z * self->count) * self->count;
}

int
lialg_sectors_point_to_index (lialgSectors*      self,
                              const limatVector* point)
{
	int x;
	int y;
	int z;

	x = LI_CLAMP (point->x / self->width, 0, self->count - 1);
	y = LI_CLAMP (point->y / self->width, 0, self->count - 1);
	z = LI_CLAMP (point->z / self->width, 0, self->count - 1);

	return x + (y + z * self->count) * self->count;
}

void
lialg_sectors_remove (lialgSectors* self,
                      int           index)
{
	lialgSector* sector;

	sector = lialg_u32dic_find (self->sectors, index);
	if (sector != NULL)
	{
		lialg_u32dic_remove (self->sectors, index);
		lisys_free (sector);
	}
}
