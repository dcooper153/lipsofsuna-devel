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
 * \addtogroup livoxBlock Block
 * @{
 */

#ifndef __VOXEL_BLOCK_H__
#define __VOXEL_BLOCK_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>
#include <system/lips-system.h>
#include "voxel-types.h"

typedef struct _livoxBlock livoxBlock;
typedef struct _livoxBlockAddr livoxBlockAddr;

struct _livoxBlockAddr
{
	uint8_t sector[3];
	uint8_t block[3];
};

void
livox_block_free (livoxBlock*   self,
                  livoxManager* manager);

void
livox_block_fill (livoxBlock*   self,
                  livoxManager* manager,
                  livoxVoxel*   terrain);

void
livox_block_optimize (livoxBlock* self);

int
livox_block_read (livoxBlock*   self,
                  livoxManager* manager,
                  liarcReader*  reader);

int
livox_block_write (livoxBlock*  self,
                   liarcWriter* writer);

int
livox_block_get_dirty (const livoxBlock* self);

void
livox_block_set_dirty (livoxBlock* self,
                       int         value);

int
livox_block_get_empty (const livoxBlock* self);

int
livox_block_get_stamp (const livoxBlock* self);

livoxVoxel*
livox_block_get_voxel (livoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z);

int
livox_block_set_voxel (livoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z,
                       livoxVoxel* voxel);

#endif

/** @} */
/** @} */
