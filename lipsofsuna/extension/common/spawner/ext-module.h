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
 * \addtogroup LIExtSpawner Spawner
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/main.h>

#define LIEXT_SCRIPT_SPAWNER "Lips.Spawner"

typedef struct _LIExtSpawner LIExtSpawner;
typedef struct _LIExtModule LIExtModule;

struct _LIExtModule
{
	LIAlgPtrdic* dictionary;
	LIMaiProgram* program;
};

LIExtModule*
liext_module_new (LIMaiProgram* program);

void
liext_module_free (LIExtModule* self);

LIExtSpawner*
liext_module_find_spawner (LIExtModule* self,
                           LIEngObject* owner);

/*****************************************************************************/

void
liext_script_spawner (LIScrClass* self,
                      void*       data);

#endif

/** @} */
/** @} */

