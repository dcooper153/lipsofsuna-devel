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
 * \addtogroup lisecPointer Pointer
 * @{
 */

#ifndef __SECTOR_POINTER_H__
#define __SECTOR_POINTER_H__

#include <system/lips-system.h>
#include "sector.h"

static inline uint32_t
lisec_pointer_new_from_offset (int x,
                               int y,
                               int z)
{
	if (x < -1 || x >= 254 || y < -1 || y >= 254 || z < -1 || z >= 254)
		return LISEC_SECTOR_INVALID;
	return ((x + 1) << 0) | ((y + 1) << 8) | ((z + 1) << 16);
}

static inline uint32_t
lisec_pointer_new (const limatVector* point)
{
	if (point->x < -LISEC_SECTOR_SIZE || point->x > 254 * LISEC_SECTOR_SIZE ||
	    point->y < -LISEC_SECTOR_SIZE || point->y > 254 * LISEC_SECTOR_SIZE ||
	    point->z < -LISEC_SECTOR_SIZE || point->z > 254 * LISEC_SECTOR_SIZE)
		return LISEC_SECTOR_INVALID;
	return lisec_pointer_new_from_offset (
		(int)(point->x >= 0.0f? point->x / LISEC_SECTOR_SIZE : -1),
		(int)(point->y >= 0.0f? point->y / LISEC_SECTOR_SIZE : -1),
		(int)(point->z >= 0.0f? point->z / LISEC_SECTOR_SIZE : -1));
}

static inline void
lisec_pointer_get_offset (uint32_t self,
                          int*     x,
                          int*     y,
                          int*     z)
{
	*x = ((self >> 0) & 0xFF) - 1;
	*y = ((self >> 8) & 0xFF) - 1;
	*z = ((self >> 16) & 0xFF) - 1;
}

static inline void
lisec_pointer_get_origin (uint32_t  self,
                          limatVector* result)
{
	int x = ((self >> 0) & 0xFF) - 1;
	int y = ((self >> 8) & 0xFF) - 1;
	int z = ((self >> 16) & 0xFF) - 1;
	*result = limat_vector_init (x * LISEC_SECTOR_SIZE, y * LISEC_SECTOR_SIZE, z * LISEC_SECTOR_SIZE);
}

static inline int
lisec_pointer_get_nearby (uint32_t self,
                          uint32_t sector,
                          int      radius)
{
	int x0 = (self >> 0) & 0xFF;
	int y0 = (self >> 8) & 0xFF;
	int z0 = (self >> 16) & 0xFF;
	int x1 = (sector >> 0) & 0xFF;
	int y1 = (sector >> 8) & 0xFF;
	int z1 = (sector >> 16) & 0xFF;

	if (x0 - radius <= x1 && x1 <= x0 + radius &&
	    y0 - radius <= y1 && y1 <= y0 + radius &&
	    z0 - radius <= z1 && z1 <= z0 + radius)
		return 1;
	return 0;
}

#endif

/** @} */
/** @} */

