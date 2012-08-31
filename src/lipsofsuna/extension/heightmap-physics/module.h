/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __EXT_HEIGHTMAP_PHYSICS_MODULE_H__
#define __EXT_HEIGHTMAP_PHYSICS_MODULE_H__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/extension/physics/ext-module.h"
#include "../heightmap/module.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#define LIEXT_SCRIPT_HEIGHTMAP_PHYSICS "HeightmapPhysics"

typedef struct _LIExtHeightmapPhysics LIExtHeightmapPhysics;
struct _LIExtHeightmapPhysics
{
	btHeightfieldTerrainShape* shape;
	btCollisionObject* body;
};

typedef struct _LIExtHeightmapPhysicsModule LIExtHeightmapPhysicsModule;
struct _LIExtHeightmapPhysicsModule
{
	LIExtHeightmapModule* heightmap;
	LIExtHeightmapHooks hooks;
	LIMaiProgram* program;
	LIPhyPhysics* physics;
};

LIExtHeightmapPhysicsModule* liext_heightmap_physics_module_new (
	LIMaiProgram* program);

void liext_heightmap_physics_module_free (
	LIExtHeightmapPhysicsModule* self);

#endif
