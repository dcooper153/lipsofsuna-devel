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
 * \addtogroup liengSector Sector
 * @{
 */

#ifndef __ENGINE_SECTOR_H__
#define __ENGINE_SECTOR_H__

#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include <system/lips-system.h>
#include "engine.h"
#include "engine-types.h"

#define LIENG_SECTOR_INDEX_FROM_POINT(p) LIENG_SECTOR_INDEX((int)((p).x / LIENG_SECTOR_WIDTH), (int)((p).y / LIENG_SECTOR_WIDTH), (int)((p).z / LIENG_SECTOR_WIDTH))
#define LIENG_SECTOR_INDEX(x, y, z) ((x) + LIENG_SECTORS_PER_LINE * (y) + LIENG_SECTORS_PER_PLANE * (z))
#define LIENG_SECTOR_WIDTH 64.0f
#define LIENG_SECTORS_PER_LINE 256
#define LIENG_SECTORS_PER_PLANE (LIENG_SECTORS_PER_LINE * LIENG_SECTORS_PER_LINE)
#define LIENG_SECTORS_PER_WORLD (LIENG_SECTORS_PER_LINE * LIENG_SECTORS_PER_LINE * LIENG_SECTORS_PER_LINE)

struct _liengSector
{
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t dirty;
	lialgU32dic* objects;
	liengEngine* engine;
	limatVector origin;
};

liengSector*
lieng_sector_default_new (liengEngine* engine,
                          uint32_t     id);

liengSector*
lieng_sector_new (liengEngine* engine,
                  uint32_t     id);

void
lieng_sector_free (liengSector* self);

int
lieng_sector_insert_object (liengSector* self,
                            liengObject* object);

void
lieng_sector_remove_object (liengSector* self,
                            liengObject* object);

void
lieng_sector_update (liengSector* self,
                     float        secs);

void
lieng_sector_get_bounds (const liengSector* self,
                         limatAabb*         result);

#endif

/** @} */
/** @} */
