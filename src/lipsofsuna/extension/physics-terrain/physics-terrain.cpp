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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtPhysicsTerrain PhysicsTerrain
 * @{
 */

#include "module.h"
#include "physics-terrain.hpp"
#include "physics-terrain-stick-shape.hpp"

/**
 * \brief Creates a new physics terrain.
 * \param module Module.
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
	self->friction = 1.0f;

	/* Prepare collision object to tile index lookup. */
	self->pointer = (LIPhyPointer*) lisys_calloc (1, sizeof (LIPhyPointer));
	if (self->pointer == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->pointer->id = 1;
	self->pointer->type = LIPHY_POINTER_TYPE_TERRAIN;
	self->pointer->pointer = self;

	/* Create the chunk dictionary. */
	self->chunks = lialg_u32dic_new ();
	if (self->chunks == NULL)
	{
		liext_physics_terrain_free (self);
		return NULL;
	}

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
	if (self->chunks != NULL)
	{
		LIAlgU32dicIter iter;
		LIALG_U32DIC_FOREACH (iter, self->chunks)
			delete ((LIExtPhysicsTerrainChunk*) iter.value);
		lialg_u32dic_free (self->chunks);
	}
	lisys_free (self->pointer);
	lisys_free (self);
}

int liext_physics_terrain_cast_ray (
	const LIExtPhysicsTerrain* self,
	const LIMatVector*         start,
	const LIMatVector*         end,
	LIPhyContact*              result)
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
	liphy_contact_init (result);
	result->fraction = fraction;
	result->normal = normal;
	result->point = point;
	result->terrain_id = self->pointer->id;
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
	LIPhyContact*              result)
{
	/* TODO */
	return 0;
}

int liext_physics_terrain_cast_sphere (
	const LIExtPhysicsTerrain* self,
	const LIMatVector*         start,
	const LIMatVector*         end,
	float                      radius,
	LIPhyContact*              result)
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

	/* Mark as hidden. */
	liext_physics_terrain_set_visible (self, 0);

	/* Delete the chunks. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->chunks)
		delete ((LIExtPhysicsTerrainChunk*) iter.value);
	lialg_u32dic_clear (self->chunks);

	self->terrain = NULL;
}

void liext_physics_terrain_update (
	LIExtPhysicsTerrain* self,
	float                secs)
{
	LIAlgU32dicIter iter;

	// Update existing chunks.
	LIALG_U32DIC_FOREACH (iter, self->chunks)
	{
		LIExtPhysicsTerrainChunk* pchunk = (LIExtPhysicsTerrainChunk*) iter.value;
		LIExtTerrainChunk* tchunk = (LIExtTerrainChunk*) lialg_u32dic_find (self->terrain->chunks, iter.key);
		if (!tchunk)
		{
			lialg_u32dic_remove (self->chunks, iter.key);
			delete pchunk;
		}
		else
			pchunk->update();
	}

	// Create new chunks.
	LIALG_U32DIC_FOREACH (iter, self->terrain->chunks)
	{
		LIExtTerrainChunk* tchunk = (LIExtTerrainChunk*) iter.value;
		LIExtPhysicsTerrainChunk* pchunk = (LIExtPhysicsTerrainChunk*) lialg_u32dic_find (self->chunks, iter.key);
		if (!pchunk)
		{
			int chunk_sz = self->terrain->chunk_size;
			int chunk_x = (iter.key % 0xFFFF) * chunk_sz;
			int chunk_z = (iter.key / 0xFFFF) * chunk_sz;
			pchunk = new LIExtPhysicsTerrainChunk(self, tchunk, chunk_x, chunk_z, chunk_sz);
			if (lialg_u32dic_insert (self->chunks, iter.key, pchunk))
				pchunk->set_visible(self->realized);
			else
				delete pchunk;
		}
	}
}

/**
 * \brief Sets the collision group of the terrain.
 * \param self Terrain.
 * \param value Collision group number.
 */
void liext_physics_terrain_set_collision_group (
	LIExtPhysicsTerrain* self,
	int                  value)
{
	/* Make sure the terrain has not been garbage collected. */
	if (self->terrain == NULL)
		return;

	/* Set the new value. */
	if (value == self->collision_group)
		return;
	self->collision_group = value;

	/* Update the chunks. */
	if (self->realized)
	{
		LIAlgU32dicIter iter;
		LIALG_U32DIC_FOREACH (iter, self->chunks)
			((LIExtPhysicsTerrainChunk*) iter.value)->set_collision_group(value);
	}
}

/**
 * \brief Sets the collision group of the terrain.
 * \param self Terrain.
 * \param value Collision group number.
 */
void liext_physics_terrain_set_collision_mask (
	LIExtPhysicsTerrain* self,
	int                  value)
{
	/* Make sure the terrain has not been garbage collected. */
	if (self->terrain == NULL)
		return;

	/* Set the new value. */
	if (value == self->collision_mask)
		return;
	self->collision_mask = value;

	/* Update the chunks. */
	if (self->realized)
	{
		LIAlgU32dicIter iter;
		LIALG_U32DIC_FOREACH (iter, self->chunks)
			((LIExtPhysicsTerrainChunk*) iter.value)->set_collision_mask(value);
	}
}

/**
 * \brief Gets the grid position of the given collision object.
 * \param self Terrain.
 * \param object Internal collision object.
 * \param part Internal collision object part ID.
 * \param result Return location of 3 integers for the grid position.
 */
void liext_physics_terrain_get_column_by_object (
	const LIExtPhysicsTerrain* self,
	void*                      object,
	int                        part,
	int*                       result)
{
	btCollisionObject* cobj = (btCollisionObject*) object;
	btCollisionShape* cshape = cobj->getCollisionShape();

	LIExtPhysicsTerrainStickShape* shape;
	if (strcmp (cshape->getName(), "Convex"))
	{
		// Some versions of Bullet report collisions are the compound object
		// and use a separate index number, which tells which subshape was hit.
		btCompoundShape* compound = (btCompoundShape*) cshape;
		lisys_assert (compound->getNumChildShapes() >= part);
		shape = (LIExtPhysicsTerrainStickShape*) compound->getChildShape(part);
	}
	else
	{
		// Some other versions of bullet report collisions directly for the
		// child convex shapes, skipping parent compound shape.
		shape = (LIExtPhysicsTerrainStickShape*) cshape;
	}

	result[0] = shape->x;
	result[1] = 0;
	result[2] = shape->z;
}

/**
 * \brief Sets the friction coefficient of the terrain.
 * \param self Terrain.
 * \param value Number.
 */
void liext_physics_terrain_set_friction (
	LIExtPhysicsTerrain* self,
	float                value)
{
	self->friction = value;

	/* Update the chunks. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->chunks)
		((LIExtPhysicsTerrainChunk*) iter.value)->set_friction(value);
}

/**
 * \brief Sets the unique ID of the terrain.
 * \param self Terrain.
 * \param value Number.
 */
void liext_physics_terrain_set_id (
	LIExtPhysicsTerrain* self,
	int                  value)
{
	self->pointer->id = value;
}

/**
 * \brief Sets whether unloaded chunks will be considered solid.
 * \param self Terrain.
 * \param value Nonzero to enable collisions. False otherwise.
 */
void liext_physics_terrain_set_unloaded_collision (
	LIExtPhysicsTerrain* self,
	int                  value)
{
	/* Make sure the terrain has not been garbage collected. */
	if (self->terrain == NULL)
		return;
	self->unloaded_collision = value;
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

	/* Set the new value. */
	if (self->realized == value)
		return;
	self->realized = value;

	/* Update the chunks. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->chunks)
		((LIExtPhysicsTerrainChunk*) iter.value)->set_visible(value);
}

/** @} */
/** @} */
