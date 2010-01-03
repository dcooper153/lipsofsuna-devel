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
 * \addtogroup LIVoxManager Manager
 * @{
 */

#ifndef __VOXEL_MANAGER_H__
#define __VOXEL_MANAGER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/system.h>
#include "voxel-sector.h"
#include "voxel-types.h"

/*****************************************************************************/

enum
{
	LIVOX_FIND_EMPTY = 0x01,
	LIVOX_FIND_FULL  = 0x02,
	LIVOX_FIND_ALL   = 0xFF
};

typedef struct _LIVoxUpdateEvent LIVoxUpdateEvent;
struct _LIVoxUpdateEvent
{
	int sector[3];
	int block[3];
};

struct _LIVoxManager
{
	LIAlgSectors* sectors;
	LIAlgU32dic* materials;
	LIArcSql* sql;
	LICalCallbacks* callbacks;
};

LIVoxManager*
livox_manager_new (LICalCallbacks* callbacks,
                   LIAlgSectors*   sectors);

void
livox_manager_free (LIVoxManager* self);

int
livox_manager_check_occluder (const LIVoxManager* self,
                              const LIVoxVoxel*   voxel);

void
livox_manager_clear_materials (LIVoxManager* self);

void
livox_manager_copy_voxels (LIVoxManager* self,
                           int           xstart,
                           int           ystart,
                           int           zstart,
                           int           xsize,
                           int           ysize,
                           int           zsize,
                           LIVoxVoxel*   result);

int
livox_manager_erase_voxel (LIVoxManager*      self,
                           const LIMatVector* point);

LIVoxMaterial*
livox_manager_find_material (LIVoxManager* self,
                             uint32_t      id);

LIVoxVoxel*
livox_manager_find_voxel (LIVoxManager*      self,
                          int                flags,
                          const LIMatVector* point,
                          LIMatVector*       center);

int
livox_manager_insert_material (LIVoxManager*  self,
                               LIVoxMaterial* material);

int
livox_manager_insert_voxel (LIVoxManager*      self,
                            const LIMatVector* point,
                            const LIVoxVoxel*  terrain);

int
livox_manager_load_materials (LIVoxManager* self);

void
livox_manager_mark_updates (LIVoxManager* self);

void
livox_manager_paste_voxels (LIVoxManager* self,
                            int           xstart,
                            int           ystart,
                            int           zstart,
                            int           xsize,
                            int           ysize,
                            int           zsize,
                            LIVoxVoxel*   voxels);

void
livox_manager_remove_material (LIVoxManager* self,
                               int           id);

int
livox_manager_replace_voxel (LIVoxManager*      self,
                             const LIMatVector* point,
                             const LIVoxVoxel*  terrain);

int
livox_manager_rotate_voxel (LIVoxManager*      self,
                            const LIMatVector* point,
                            int                axis,
                            int                step);

void
livox_manager_update (LIVoxManager* self,
                      float         secs);

void
livox_manager_update_marked (LIVoxManager* self);

int
livox_manager_write (LIVoxManager* self);

int
livox_manager_write_materials (LIVoxManager* self);

void
livox_manager_set_sql (LIVoxManager* self,
                       LIArcSql*     sql);

void
livox_manager_get_voxel (LIVoxManager* self,
                         int           x,
                         int           y,
                         int           z,
                         LIVoxVoxel*   value);

int
livox_manager_set_voxel (LIVoxManager*     self,
                         int               x,
                         int               y,
                         int               z,
                         const LIVoxVoxel* value);

#endif

/** @} */
/** @} */
