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

#ifndef __ALGORITHM_SECTORS_H__
#define __ALGORITHM_SECTORS_H__

#include <math/lips-math.h>
#include "algorithm-u32dic.h"

typedef struct _lialgSector lialgSector;
struct _lialgSector
{
	int index;
	int x;
	int y;
	int z;
	void* data;
};

typedef struct _lialgSectors lialgSectors;
struct _lialgSectors
{
	int count;
	float width;
	lialgU32dic* sectors;
};

#ifdef __cplusplus
extern "C" {
#endif

lialgSectors*
lialg_sectors_new (int   count,
                   float width);

void
lialg_sectors_free (lialgSectors* self);

lialgSector*
lialg_sectors_create (lialgSectors* self,
                      int           index);

lialgSector*
lialg_sectors_find (lialgSectors* self,
                    int           index);

void
lialg_sectors_index_to_offset (lialgSectors* self,
                               int           index,
                               int*          x,
                               int*          y,
                               int*          z);

int
lialg_sectors_offset_to_index (lialgSectors* self,
                               int           x,
                               int           y,
                               int           z);

int
lialg_sectors_point_to_index (lialgSectors*      self,
                              const limatVector* point);

void
lialg_sectors_remove (lialgSectors* self,
                      int           index);

#ifdef __cplusplus
}
#endif

#endif
