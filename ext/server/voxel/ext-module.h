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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvVoxel Voxel
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <algorithm/lips-algorithm.h>
#include <callback/lips-callback.h>
#include <script/lips-script.h>
#include <server/lips-server.h>
#include <voxel/lips-voxel.h>
#include "ext-listener.h"

#define LIEXT_SCRIPT_VOXEL "Lips.Voxel"

typedef struct _liextModule liextModule;
struct _liextModule
{
	float radius;
	lialgPtrdic* listeners;
	licalHandle calls[5];
	lisrvServer* server;
	livoxManager* voxels;
};

liextModule*
liext_module_new (lisrvServer* server);

void
liext_module_free (liextModule* self);

void
liext_module_fill_box (liextModule*       self,
                       const limatVector* min,
                       const limatVector* max,
                       livoxVoxel         terrain);

void
liext_module_fill_sphere (liextModule*       self,
                          const limatVector* center,
                          float              radius,
                          livoxVoxel         terrain);

int
liext_module_write (liextModule* self,
                    liarcSql*    sql);

/*****************************************************************************/

void
liextVoxelScript (liscrClass* self,
                  void*       data);

#endif

/** @} */
/** @} */
/** @} */
