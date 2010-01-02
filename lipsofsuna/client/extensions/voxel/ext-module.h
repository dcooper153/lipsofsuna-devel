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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <algorithm/lips-algorithm.h>
#include <script/lips-script.h>
#include <voxel/lips-voxel.h>

typedef struct _liextModule liextModule;

#include "ext-block.h"

#define LIEXT_SCRIPT_VOXEL "Lips.Voxel"

struct _liextModule
{
	lialgMemdic* blocks;
	licalHandle calls[4];
	licliClient* client;
	livoxManager* voxels;
};

liextModule*
liext_module_new (licliClient* client);

void
liext_module_free (liextModule* self);

int
liext_module_build_all (liextModule* self);

int
liext_module_build_block (liextModule* self,
                          int          sx,
                          int          sy,
                          int          sz,
                          int          bx,
                          int          by,
                          int          bz);

void
liext_module_clear_all (liextModule* self);

/*****************************************************************************/

void
liextVoxelScript (liscrClass* self,
                  void*       data);

#endif

/** @} */
/** @} */
/** @} */
