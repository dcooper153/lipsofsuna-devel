/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LIVoxBlock Block
 * @{
 */

#ifndef __VOXEL_BLOCK_H__
#define __VOXEL_BLOCK_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "voxel-types.h"

typedef struct _LIVoxBlock LIVoxBlock;
typedef struct _LIVoxBlockAddr LIVoxBlockAddr;

struct _LIVoxBlockAddr
{
	uint8_t sector[3];
	uint8_t block[3];
};

void
livox_block_free (LIVoxBlock*   self,
                  LIVoxManager* manager);

void
livox_block_fill (LIVoxBlock*   self,
                  LIVoxManager* manager,
                  LIVoxVoxel*   terrain);

void
livox_block_optimize (LIVoxBlock* self);

int
livox_block_read (LIVoxBlock*   self,
                  LIVoxManager* manager,
                  LIArcReader*  reader);

int
livox_block_write (LIVoxBlock*  self,
                   LIArcWriter* writer);

int
livox_block_get_dirty (const LIVoxBlock* self);

void
livox_block_set_dirty (LIVoxBlock* self,
                       int         value);

int
livox_block_get_empty (const LIVoxBlock* self);

int
livox_block_get_stamp (const LIVoxBlock* self);

LIVoxVoxel*
livox_block_get_voxel (LIVoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z);

int
livox_block_set_voxel (LIVoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z,
                       LIVoxVoxel* voxel);

#endif

/** @} */
/** @} */
