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

#ifndef __EXT_PHYSICS_MODULE_H__
#define __EXT_PHYSICS_MODULE_H__

#include "lipsofsuna/extension.h"
#include "physics.h"
#include "physics-constraint.h"
#include "physics-model.h"
#include "physics-object.h"
#include "physics-shape.h"
#include "physics-types.h"

#define LIEXT_SCRIPT_PHYSICS "Physics"

typedef struct _LIExtPhysicsModule LIExtPhysicsModule;
struct _LIExtPhysicsModule
{
	int simulate;
	LICalHandle calls[4];
	LIMaiProgram* program;
	LIPhyPhysics* physics;
};

LIExtPhysicsModule* liext_physics_new (
	LIMaiProgram* program);

void liext_physics_free (
	LIExtPhysicsModule* self);

/*****************************************************************************/

void liext_script_physics (
	LIScrScript* self);

#endif
