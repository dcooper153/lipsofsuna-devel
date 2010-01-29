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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtVoxel Voxel
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/main.h>
#include <lipsofsuna/server.h>
#include <lipsofsuna/voxel.h>

typedef struct _LIExtBlock LIExtBlock;
typedef struct _LIExtBlockKey LIExtBlockKey;
typedef struct _LIExtListener LIExtListener;
typedef struct _LIExtListenerBlock LIExtListenerBlock;
typedef struct _LIExtModule LIExtModule;

#include "ext-block.h"
#include "ext-listener.h"

#define LIEXT_SCRIPT_MATERIAL "Material"
#define LIEXT_SCRIPT_TILE "Tile"
#define LIEXT_SCRIPT_VOXEL "Voxel"

struct _LIExtModule
{
	float radius;
	LIAlgPtrdic* listeners;
	LIAlgMemdic* blocks;
	LIArcWriter* assign_packet;
	LICalHandle calls[8];
	LIMaiProgram* program;
	LIVoxManager* voxels;
};

LIExtModule*
liext_module_new (LIMaiProgram* program);

void
liext_module_free (LIExtModule* self);

int
liext_module_erase_point (LIExtModule*       self,
                          const LIMatVector* point);

void
liext_module_fill_box (LIExtModule*       self,
                       const LIMatVector* min,
                       const LIMatVector* max,
                       LIVoxVoxel         terrain);

void
liext_module_fill_sphere (LIExtModule*       self,
                          const LIMatVector* center,
                          float              radius,
                          LIVoxVoxel         terrain);

int
liext_module_write (LIExtModule* self,
                    LIArcSql*    sql);

LIVoxManager*
liext_module_get_voxels (LIExtModule* self);

/*****************************************************************************/

void
liext_script_material (LIScrClass* self,
                     void*       data);

void
liext_script_tile (LIScrClass* self,
                 void*       data);

void
liext_script_voxel (LIScrClass* self,
                  void*       data);

#endif

/** @} */
/** @} */
