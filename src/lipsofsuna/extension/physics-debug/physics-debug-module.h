/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_PHYSICS_DEBUG_MODULE_H__
#define __EXT_PHYSICS_DEBUG_MODULE_H__

#include "lipsofsuna/extension.h"

#define LIEXT_SCRIPT_PHYSICS_DEBUG "PhysicsDebug"

typedef struct _LIExtPhysicsDebugModule LIExtPhysicsDebugModule;
struct _LIExtPhysicsDebugModule
{
	LIMaiProgram* program;
};

LIExtPhysicsDebugModule* liext_physics_debug_module_new (
	LIMaiProgram* program);

void liext_physics_debug_module_free (
	LIExtPhysicsDebugModule* self);

void liext_physics_debug_module_get_memstat (
	LIExtPhysicsDebugModule* self,
	LIMaiMemstat*      stat);

/*****************************************************************************/

LIAPICALL (void, liext_script_physics_debug, (
	LIScrScript* self));

#endif
