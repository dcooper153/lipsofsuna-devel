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
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyTerrain Terrain
 * @{
 */

#include "lipsofsuna/system.h"
#include "lipsofsuna/extension/physics/physics.h"
#include "lipsofsuna/extension/physics/physics-private.h"
#include "physics-terrain.hpp"
#include "physics-terrain-raycast-hook.hpp"

/**
 * \brief Creates a terrain physics manager.
 * \param physics Physics engine.
 * \param voxels Voxel manager.
 * \param collision_group Collision group.
 * \param collision_mask Collision mask.
 * \return New terrain physics manager.
 */
LIPhyTerrain* liphy_terrain_new (
	LIPhyPhysics* physics,
	LIVoxManager* voxels,
	int           collision_group,
	int           collision_mask)
{
	btQuaternion r(0.0f, 0.0f, 0.0f, 1.0f);
	btVector3 p(0.0f, 0.0f, 0.0f);
	LIPhyTerrain* self;

	/* Allocate self. */
	self = (LIPhyTerrain*) lisys_calloc (1, sizeof (LIPhyTerrain));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->voxels = voxels;
	self->collision_group = collision_group;
	self->collision_mask = collision_mask;

	/* Prepare collision object to tile index lookup. */
	self->pointer = (LIPhyPointer*) lisys_calloc (1, sizeof (LIPhyPointer));
	if (self->pointer == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->pointer->type = LIPHY_POINTER_TYPE_VOXEL;
	self->pointer->pointer = self;

	/* Create the collision shape. */
	self->shape = new LIExtPhysicsVoxelShape (self);

	/* Create the collision object. */
	self->object = new btCollisionObject ();
	self->object->setCollisionFlags (btCollisionObject::CF_STATIC_OBJECT);
	self->object->setActivationState (DISABLE_DEACTIVATION);
	self->object->setCollisionShape (self->shape);
	self->object->setWorldTransform (btTransform (r, p));
	self->object->setUserPointer (self->pointer);

	/* Create the raycast hook. */
	self->raycast_hook = new LIExtPhysicsVoxelRaycastHook (self);
	liphy_physics_add_raycast_hook (physics, self->raycast_hook);

	/* Create the collision algorithm. */
	self->collision_algorithm = new LIExtPhysicsVoxelCollisionAlgorithmCreator ();
	physics->configuration->add_algorithm (self->collision_algorithm);

	return self;
}

/**
 * \brief Frees the terrain block.
 * \param self Terrain.
 */
void liphy_terrain_free (
	LIPhyTerrain* self)
{
	liphy_terrain_set_realized (self, 0);
	delete self->object;
	delete self->shape;
	if (self->raycast_hook != NULL)
	{
		liphy_physics_remove_raycast_hook (self->physics, self->raycast_hook);
		delete self->raycast_hook;
	}
	if (self->collision_algorithm != NULL)
	{
		self->physics->configuration->remove_algorithm (self->collision_algorithm);
		delete self->collision_algorithm;
	}
	lisys_free (self->pointer);
	lisys_free (self);
}

int liphy_terrain_cast_ray (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	LIPhyContact*       result)
{
	LIMatVector p0;
	LIMatVector p1;
	LIMatVector point;
	LIMatVector tile;

	p0 = limat_vector_multiply (*start, 1.0f / self->voxels->tile_width);
	p1 = limat_vector_multiply (*end, 1.0f / self->voxels->tile_width);
	if (!livox_manager_intersect_ray (self->voxels, &p0, &p1, &point, &tile))
		return 0;
	liphy_contact_init_from_voxel (result, self->voxels->tile_width, &p0, &p1, &point, &tile);

	return 1;
}

int liphy_terrain_cast_shape (
	const LIPhyTerrain* self,
	const LIMatTransform* start,
	const LIMatTransform* end,
	const LIPhyShape*     shape,
	LIPhyContact*         result)
{
	/* TODO */
	return 0;
}

int liphy_terrain_cast_sphere (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	float               radius,
	LIPhyContact*       result)
{
	/* TODO */
	return 0;
}

/**
 * \brief Realizes or unrealizes the terrain.
 * \param self Terrain.
 * \param value Nonzero to realize, zero to unrealize.
 */
void liphy_terrain_set_realized (
	LIPhyTerrain* self,
	int           value)
{
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
