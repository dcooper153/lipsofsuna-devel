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
 * \addtogroup LIExtNpc Npc
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/ai.h>
#include <lipsofsuna/voxel.h>
#include <lipsofsuna/extension.h>

#define LIEXT_SCRIPT_NPC "Npc"

typedef struct _LIExtModule LIExtModule;
typedef struct _LIExtNpc LIExtNpc;

struct _LIExtModule
{
	LIAiManager* ai;
	LIAlgPtrdic* dictionary;
	LIMaiProgram* program;
	LIVoxManager* voxels;
};

LIExtModule*
liext_module_new (LIMaiProgram* program);

void
liext_module_free (LIExtModule* self);

LIExtNpc*
liext_module_find_npc (LIExtModule* self,
                       LIEngObject* owner);

LIAiPath*
liext_module_solve_path (LIExtModule*       self,
                         const LIEngObject* object,
                         const LIMatVector* target);

/*****************************************************************************/

void
liext_script_npc (LIScrClass* self,
                  void*       data);

#endif

/** @} */
/** @} */
