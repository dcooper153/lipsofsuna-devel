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

struct _liengSector
{
	uint32_t id;
	liengEngine* engine;
	limatVector origin;
	lialgU32dic* objects;
};

liengSector*
lieng_sector_new (liengEngine* engine,
                  uint32_t     id,
                  const char*  dir);

void
lieng_sector_free (liengSector* self);

int
lieng_sector_insert_object (liengSector* self,
                            liengObject* object);

void
lieng_sector_remove_object (liengSector* self,
                            liengObject* object);

int
lieng_sector_save (liengSector* self);

void
lieng_sector_get_bounds (const liengSector* self,
                         limatAabb*         result);

#endif

/** @} */
/** @} */
