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
 * \addtogroup livoxManager Manager
 * @{
 */

#ifndef __VOXEL_MANAGER_H__
#define __VOXEL_MANAGER_H__

#include <algorithm/lips-algorithm.h>
#include <system/lips-system.h>
#include "voxel-sector.h"
#include "voxel-types.h"

/*****************************************************************************/

struct _livoxManager
{
	lialgU32dic* materials;
	lialgU32dic* sectors;
	liphyPhysics* physics;
#ifndef LIVOX_DISABLE_GRAPHICS
	lirndApi* renderapi;
	lirndRender* render;
	lirndScene* scene;
#endif
};

#ifndef LIVOX_DISABLE_GRAPHICS
livoxManager*
livox_manager_new (liphyPhysics* physics,
                   lirndScene*   scene,
                   lirndApi*     rndapi);
#else
livoxManager*
livox_manager_new (liphyPhysics* physics);
#endif

void
livox_manager_free (livoxManager* self);

void
livox_manager_clear (livoxManager* self);

void
livox_manager_clear_materials (livoxManager* self);

livoxSector*
livox_manager_create_sector (livoxManager* self,
                             uint32_t      id);

void
livox_manager_copy_voxels (livoxManager* self,
                           int           xstart,
                           int           ystart,
                           int           zstart,
                           int           xsize,
                           int           ysize,
                           int           zsize,
                           livoxVoxel*   result);

livoxMaterial*
livox_manager_find_material (livoxManager* self,
                             uint32_t      id);

livoxSector*
livox_manager_find_sector (livoxManager* self,
                           uint32_t      id);

int
livox_manager_insert_material (livoxManager*  self,
                               livoxMaterial* material);

int
livox_manager_load_materials (livoxManager* self,
                              liarcSql*     sql);

livoxSector*
livox_manager_load_sector (livoxManager* self,
                           uint32_t      id,
                           liarcSql*     sql);

void
livox_manager_update (livoxManager* self,
                      float         secs);

int
livox_manager_write (livoxManager* self,
                     liarcSql*     sql);

#endif

/** @} */
/** @} */
