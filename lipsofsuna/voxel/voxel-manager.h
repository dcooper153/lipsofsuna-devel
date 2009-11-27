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
#include <callback/lips-callback.h>
#include <system/lips-system.h>
#include "voxel-sector.h"
#include "voxel-types.h"

/*****************************************************************************/

enum
{
	LIVOX_FIND_EMPTY = 0x01,
	LIVOX_FIND_FULL  = 0x02,
	LIVOX_FIND_ALL   = 0xFF
};

enum
{
	LIVOX_CALLBACK_FREE_BLOCK,
	LIVOX_CALLBACK_LOAD_BLOCK
};

typedef struct _livoxUpdateEvent livoxUpdateEvent;
struct _livoxUpdateEvent
{
	int sector[3];
	int block[3];
};

struct _livoxManager
{
	lialgU32dic* materials;
	lialgU32dic* sectors;
	liarcSql* sql;
	licalCallbacks* callbacks;
};

livoxManager*
livox_manager_new ();

void
livox_manager_free (livoxManager* self);

int
livox_manager_check_occluder (const livoxManager* self,
                              const livoxVoxel*   voxel);

void
livox_manager_clear (livoxManager* self);

void
livox_manager_clear_materials (livoxManager* self);

void
livox_manager_copy_voxels (livoxManager* self,
                           int           xstart,
                           int           ystart,
                           int           zstart,
                           int           xsize,
                           int           ysize,
                           int           zsize,
                           livoxVoxel*   result);

livoxSector*
livox_manager_create_sector (livoxManager* self,
                             uint32_t      id);

int
livox_manager_erase_voxel (livoxManager*      self,
                           const limatVector* point);

livoxMaterial*
livox_manager_find_material (livoxManager* self,
                             uint32_t      id);

livoxSector*
livox_manager_find_sector (livoxManager* self,
                           uint32_t      id);

livoxVoxel*
livox_manager_find_voxel (livoxManager*      self,
                          int                flags,
                          const limatVector* point,
                          limatVector*       center);

int
livox_manager_insert_material (livoxManager*  self,
                               livoxMaterial* material);

int
livox_manager_insert_voxel (livoxManager*      self,
                            const limatVector* point,
                            const livoxVoxel*  terrain);

int
livox_manager_load_materials (livoxManager* self);

livoxSector*
livox_manager_load_sector (livoxManager* self,
                           uint32_t      id);

void
livox_manager_mark_updates (livoxManager* self);

void
livox_manager_paste_voxels (livoxManager* self,
                            int           xstart,
                            int           ystart,
                            int           zstart,
                            int           xsize,
                            int           ysize,
                            int           zsize,
                            livoxVoxel*   voxels);

void
livox_manager_remove_material (livoxManager* self,
                               int           id);

int
livox_manager_replace_voxel (livoxManager*      self,
                             const limatVector* point,
                             const livoxVoxel*  terrain);

int
livox_manager_rotate_voxel (livoxManager*      self,
                            const limatVector* point,
                            int                axis,
                            int                step);

void
livox_manager_update (livoxManager* self,
                      float         secs);

void
livox_manager_update_marked (livoxManager* self);

int
livox_manager_write (livoxManager* self);

int
livox_manager_write_materials (livoxManager* self);

void
livox_manager_set_sql (livoxManager* self,
                       liarcSql*     sql);

void
livox_manager_get_voxel (livoxManager* self,
                         int           x,
                         int           y,
                         int           z,
                         livoxVoxel*   value);

int
livox_manager_set_voxel (livoxManager*     self,
                         int               x,
                         int               y,
                         int               z,
                         const livoxVoxel* value);

#endif

/** @} */
/** @} */
