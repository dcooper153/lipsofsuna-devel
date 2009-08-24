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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxSector Sector
 * @{
 */

#ifndef __VOXEL_SECTOR_H__
#define __VOXEL_SECTOR_H__

#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include <system/lips-system.h>
#include "voxel.h"
#include "voxel-block.h"
#include "voxel-types.h"

livoxSector*
livox_sector_new (livoxManager* manager,
                  uint32_t      id);

void
livox_sector_free (livoxSector* self);

int
livox_sector_build_block (livoxSector* self,
                          int          x,
                          int          y,
                          int          z);

void
livox_sector_erase_aabb (livoxSector*     self,
                         const limatAabb* box);

void
livox_sector_erase_sphere (livoxSector*       self,
                           const limatVector* center,
                           float              radius);

void
livox_sector_fill (livoxSector* self,
                   livoxVoxel   terrain);

void
livox_sector_fill_aabb (livoxSector*     self,
                        const limatAabb* box,
                        livoxVoxel       terrain);

void
livox_sector_fill_sphere (livoxSector*       self,
                          const limatVector* center,
                          float              radius,
                          livoxVoxel         terrain);

int
livox_sector_read (livoxSector* self,
                   liarcSql*    sql);

void
livox_sector_update (livoxSector* self,
                     float        secs);

int
livox_sector_write (livoxSector* self,
                    liarcSql*    sql);

livoxBlock*
livox_sector_get_block (livoxSector* self,
                        int          index);

void
livox_sector_get_bounds (const livoxSector* self,
                         limatAabb*         result);

int
livox_sector_get_dirty (const livoxSector* self);

void
livox_sector_set_dirty (livoxSector* self,
                        int          value);

int
livox_sector_get_empty (const livoxSector* self);

void
livox_sector_get_offset (const livoxSector* self,
                         int*               x,
                         int*               y,
                         int*               z);

void
livox_sector_get_origin (const livoxSector* self,
                         limatVector*       result);

livoxVoxel
livox_sector_get_voxel (livoxSector* sector,
                        int          x,
                        int          y,
                        int          z);

int
livox_sector_set_voxel (livoxSector* self,
                        int          x,
                        int          y,
                        int          z,
                        livoxVoxel   terrain);

#endif

/** @} */
/** @} */
