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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#ifndef __EXT_BLOCK_H__
#define __EXT_BLOCK_H__

#include <client/lips-client.h>
#include <system/lips-system.h>
#include <voxel/lips-voxel.h>

typedef struct _liextBlock liextBlock;
struct _liextBlock
{
	licliModule* module;
	liphyObject* physics;
	lirndGroup* group;
};

liextBlock*
liext_block_new (licliModule* module);

void
liext_block_free (liextBlock* self);

int
liext_block_build (liextBlock*     self,
                   liextModule*    module,
                   livoxBlock*     block,
                   livoxBlockAddr* addr);

#endif

/** @} */
/** @} */
/** @} */
