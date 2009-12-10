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
 * \addtogroup liextsrvNpc Npc
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <ai/lips-ai.h>
#include <script/lips-script.h>
#include <voxel/lips-voxel.h>

#define LIEXT_SCRIPT_NPC "Lips.Npc"

typedef struct _liextModule liextModule;
typedef struct _liextNpc liextNpc;

struct _liextModule
{
	liaiManager* ai;
	lisrvServer* server;
	livoxManager* voxels;
};

liextModule*
liext_module_new (lisrvServer* server);

void
liext_module_free (liextModule* self);

liaiPath*
liext_module_solve_path (liextModule*       self,
                         const liengObject* object,
                         const limatVector* target);

/*****************************************************************************/

void
liextNpcScript (liscrClass* self,
                void*       data);

#endif

/** @} */
/** @} */
/** @} */
