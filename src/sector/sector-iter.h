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
 * \addtogroup lisec Sector 
 * @{
 * \addtogroup lisecIter Iter 
 * @{
 */

#ifndef __SECTOR_ITER_H__
#define __SECTOR_ITER_H__

#include "sector.h"
#include "sector-pointer.h"

#define LI_FOREACH_SECTOR(iter, center, radius) \
	for (lisec_iter_first (&iter, center, radius) ; iter.id != LISEC_SECTOR_INVALID ; \
	     lisec_iter_next (&iter))

typedef struct _lisecIter lisecIter;
struct _lisecIter
{
	int x;
	int x0;
	int x1;
	int y;
	int y0;
	int y1;	
	int z;
	int z0;
	int z1;
	uint32_t id;
	uint32_t center;
};

static inline void
lisec_iter_first (lisecIter* self,
                  uint32_t   center,
                  int        radius)
{
	int x;
	int y;
	int z;

	/* Format counter. */
	lisec_pointer_get_offset (center, &x, &y, &z);
	self->center = center;
	self->x0 = LI_MAX (-1, (int)(x - radius));
	self->x1 = LI_MIN (254, (int)(x + radius));
	self->y0 = LI_MAX (-1, (int)(y - radius));
	self->y1 = LI_MIN (254, (int)(y + radius));
	self->z0 = LI_MAX (-1, (int)(z - radius));
	self->z1 = LI_MIN (254, (int)(z + radius));

	/* Choose first. */
	self->x = self->x0;
	self->y = self->y0;
	self->z = self->z0;
	self->id = lisec_pointer_new_from_offset (self->x, self->y, self->z);
}

static inline int
lisec_iter_next (lisecIter* self)
{
	/* Advance counter. */
	if (self->x >= self->x1)
	{
		self->x = self->x0;
		if (self->y >= self->y1)
		{
			self->y = self->y0;
			if (self->z >= self->z1)
			{
				self->id = LISEC_SECTOR_INVALID;
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

	/* Choose next. */
	self->id = lisec_pointer_new_from_offset (self->x, self->y, self->z);
	return 1;
}

#endif

/** @} */
/** @} */

