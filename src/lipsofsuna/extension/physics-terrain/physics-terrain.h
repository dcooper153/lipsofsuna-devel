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

#ifndef __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_H__
#define __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/extension/physics/ext-module.h"
#include "lipsofsuna/extension/terrain/terrain.h"

typedef struct _LIExtPhysicsTerrainModule LIExtPhysicsTerrainModule;
typedef struct _LIExtPhysicsTerrain LIExtPhysicsTerrain;

LIAPICALL (LIExtPhysicsTerrain*, liext_physics_terrain_new, (
	LIExtPhysicsTerrainModule* module,
	LIExtTerrain*              terrain,
	int                        collision_group,
	int                        collision_mask));

LIAPICALL (void, liext_physics_terrain_free, (
	LIExtPhysicsTerrain* self));

LIAPICALL (int, liext_physics_terrain_cast_ray, (
	const LIExtPhysicsTerrain* self,
	const LIMatVector*         start,
	const LIMatVector*         end,
	LIPhyContact*              result));

LIAPICALL (int, liext_physics_terrain_cast_shape, (
	const LIExtPhysicsTerrain* self,
	const LIMatTransform*      start,
	const LIMatTransform*      end,
	const LIPhyShape*          shape,
	LIPhyContact*              result));

LIAPICALL (int, liext_physics_terrain_cast_sphere, (
	const LIExtPhysicsTerrain* self,
	const LIMatVector*         start,
	const LIMatVector*         end,
	float                      radius,
	LIPhyContact*              result));

LIAPICALL (void, liext_physics_terrain_remove, (
	LIExtPhysicsTerrain* self,
	LIExtTerrain*        terrain));

LIAPICALL (void, liext_physics_terrain_set_collision_group, (
	LIExtPhysicsTerrain* self,
	int                  value));

LIAPICALL (void, liext_physics_terrain_set_collision_mask, (
	LIExtPhysicsTerrain* self,
	int                  value));

LIAPICALL (void, liext_physics_terrain_set_friction, (
	LIExtPhysicsTerrain* self,
	float                value));

LIAPICALL (void, liext_physics_terrain_set_id, (
	LIExtPhysicsTerrain* self,
	int                  value));

LIAPICALL (int, liext_physics_terrain_get_valid, (
	const LIExtPhysicsTerrain* self));

LIAPICALL (void, liext_physics_terrain_set_visible, (
	LIExtPhysicsTerrain* self,
	int                  value));

#endif
