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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtPhysicsTerrain PhysicsTerrain
 * @{
 */

#include "module.h"
#include "physics-terrain.hpp"

/**
 * \brief Creates a new physics terrain.
 * \param physics Physics engine.
 * \param terrain Terrain.
 * \param collision_group Collision group.
 * \param collision_mask Collision mask.
 * \return New physics terrain.
 */
LIExtPhysicsTerrain* liext_physics_terrain_new (
	LIExtPhysicsTerrainModule* module,
	LIExtTerrain*              terrain,
	int                        collision_group,
	int                        collision_mask)
{
	btQuaternion r(0.0f, 0.0f, 0.0f, 1.0f);
	btVector3 p(0.0f, 0.0f, 0.0f);
	LIExtPhysicsTerrain* self;

	/* Allocate self. */
	self = (LIExtPhysicsTerrain*) lisys_calloc (1, sizeof (LIExtPhysicsTerrain));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->terrain = terrain;
	self->collision_group = collision_group;
	self->collision_mask = collision_mask;

	/* Prepare collision object to tile index lookup. */
	self->pointer = (LIPhyPointer*) lisys_calloc (1, sizeof (LIPhyPointer));
	if (self->pointer == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->pointer->object = 0;
	self->pointer->pointer = self;

	/* Create the collision shape. */
	self->shape = new LIExtPhysicsTerrainShape (self);

	/* Create the collision object. */
	self->object = new btCollisionObject ();
	self->object->setCollisionFlags (btCollisionObject::CF_STATIC_OBJECT);
	self->object->setActivationState (DISABLE_DEACTIVATION);
	self->object->setCollisionShape (self->shape);
	self->object->setWorldTransform (btTransform (r, p));
	self->object->setUserPointer (self->pointer);

	/* Create the raycast hook. */
	self->raycast_hook = new LIExtPhysicsTerrainRaycastHook (self);
	liphy_physics_add_raycast_hook (self->module->physics, self->raycast_hook);

	/* Add to the terrain dictionary. */
	if (!lialg_ptrdic_insert (module->terrains, self, self))
	{
		liext_physics_terrain_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the terrain block.
 * \param self Terrain.
 */
void liext_physics_terrain_free (
	LIExtPhysicsTerrain* self)
{
	lialg_ptrdic_remove (self->module->terrains, self);
	if (self->terrain != NULL)
	{
		liext_physics_terrain_remove (self, self->terrain);
	}
	lisys_free (self->pointer);
	lisys_free (self);
}

int liext_physics_terrain_cast_ray (
	const LIExtPhysicsTerrain* self,
	const LIMatVector*         start,
	const LIMatVector*         end,
	LIPhyCollision*            result)
{
	int grid_x;
	int grid_z;
	float fraction;
	LIMatVector p0;
	LIMatVector p1;
	LIMatVector point;
	LIMatVector normal;

	/* Make sure the terrain has not been garbage collected. */
	if (self->terrain == NULL)
		return 0;

	p0 = *start;
	p1 = *end;
	if (!liext_terrain_intersect_ray (self->terrain, &p0, &p1, &grid_x, &grid_z, &point, &normal, &fraction))
		return 0;
	result->fraction = fraction;
	result->normal = normal;
	result->point = point;
	result->object = NULL;
	result->terrain = NULL; // FIXME
	result->terrain_tile[0] = grid_x;
	result->terrain_tile[1] = 0;
	result->terrain_tile[2] = grid_z;

	return 1;
}

int liext_physics_terrain_cast_shape (
	const LIExtPhysicsTerrain* self,
	const LIMatTransform*      start,
	const LIMatTransform*      end,
	const LIPhyShape*          shape,
	LIPhyCollision*            result)
{
	/* TODO */
	return 0;
}

int liext_physics_terrain_cast_sphere (
	const LIExtPhysicsTerrain* self,
	const LIMatVector*         start,
	const LIMatVector*         end,
	float                      radius,
	LIPhyCollision*            result)
{
	/* TODO */
	return 0;
}

/**
 * \brief If the given terrain is being used, removes references to it and disables physics.
 *
 * This function is used to disable the physics terrain when the terrain
 * object it is using has been garbage collected. After calling this with
 * the terrain pointer being used, all physics operations become no-op.
 *
 * \param self Terrain.
 * \param terrain Terrain to remove.
 */
void liext_physics_terrain_remove (
	LIExtPhysicsTerrain* self,
	LIExtTerrain*        terrain)
{
	if (self->terrain != terrain)
		return;
	self->terrain = NULL;

	/* Remove the raycast hook. */
	if (self->raycast_hook != NULL)
	{
		liphy_physics_remove_raycast_hook (self->module->physics, self->raycast_hook);
		delete self->raycast_hook;
	}

	/* Delete the physics object. */
	liext_physics_terrain_set_visible (self, 0);
	delete self->object;
	delete self->shape;
	self->object = NULL;
	self->shape = NULL;
}

/**
 * \brief Returns nonzero if there is a terrain object assigned.
 * \param self Terrain.
 * \return Nonzero if attached, zero if not.
 */
int liext_physics_terrain_get_valid (
	const LIExtPhysicsTerrain* self)
{
	return self->terrain != NULL;
}

/**
 * \brief Realizes or unrealizes the terrain.
 * \param self Terrain.
 * \param value Nonzero to realize, zero to unrealize.
 */
void liext_physics_terrain_set_visible (
	LIExtPhysicsTerrain* self,
	int                  value)
{
	/* Make sure the terrain has not been garbage collected. */
	if (self->terrain == NULL)
		return;

	if (self->realized == value)
		return;
	self->realized = value;
	if (value)
		self->physics->dynamics->addCollisionObject (self->object, self->collision_group, self->collision_mask);
	else
		self->physics->dynamics->removeCollisionObject (self->object);
}

/** @} */
/** @} */
