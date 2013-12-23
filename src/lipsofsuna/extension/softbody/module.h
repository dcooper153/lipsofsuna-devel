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

#ifndef __EXT_SOFTBODY_MODULE_H__
#define __EXT_SOFTBODY_MODULE_H__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/render.h"
#include "lipsofsuna/extension/physics/physics.h"

#define LIEXT_SCRIPT_SOFTBODY "Softbody"

typedef struct _LIExtSoftbodyModule LIExtSoftbodyModule;
struct _LIExtSoftbodyModule
{
	LIMaiProgram* program;
	LIPhyPhysics* physics;
	LIRenRender* render;
};

LIAPICALL (LIExtSoftbodyModule*, liext_softbody_module_new, (
	LIMaiProgram* program));

LIAPICALL (void, liext_softbody_module_free, (
	LIExtSoftbodyModule* self));

LIAPICALL (void, liext_softbody_module_get_memstat, (
	LIExtSoftbodyModule* self,
	LIMaiMemstat*        stat));

/*****************************************************************************/

LIAPICALL (void, liext_script_softbody, (
	LIScrScript* self));

#endif
