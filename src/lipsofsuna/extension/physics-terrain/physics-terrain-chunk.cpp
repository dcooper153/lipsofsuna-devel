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
#include "physics-terrain-chunk.hpp"
#include "physics-terrain-stick-shape.hpp"

LIExtPhysicsTerrainChunk::LIExtPhysicsTerrainChunk(LIExtPhysicsTerrain* terrain, LIExtTerrainChunk* chunk, int x, int z, int size) :
	x(x), z(z), size(size), stamp(0), realized(false), object(NULL), shape(NULL), terrain(terrain), chunk(chunk)
{
}

LIExtPhysicsTerrainChunk::~LIExtPhysicsTerrainChunk()
{
	delete_object();
	delete_shape();
}

void LIExtPhysicsTerrainChunk::update()
{
	// Check for updates.
	if (!realized || stamp == chunk->stamp)
		return;
	stamp = chunk->stamp;

	// Rebuild the shape.
	delete_object();
	delete_shape();
	create_shape();
	create_object();
}

void LIExtPhysicsTerrainChunk::set_collision_group(int value)
{
	if (!object)
		return;
	terrain->module->physics->dynamics->removeCollisionObject (object);
	terrain->module->physics->dynamics->addCollisionObject (object, terrain->collision_group, terrain->collision_mask);
}

void LIExtPhysicsTerrainChunk::set_collision_mask(int value)
{
	if (!object)
		return;
	terrain->module->physics->dynamics->removeCollisionObject (object);
	terrain->module->physics->dynamics->addCollisionObject (object, terrain->collision_group, terrain->collision_mask);
}

void LIExtPhysicsTerrainChunk::set_friction(float value)
{
	object->setFriction (terrain->friction);
}

/**
 * \brief Realizes or unrealizes the chunk.
 * \param value True to realize, false to unrealize.
 */
void LIExtPhysicsTerrainChunk::set_visible (
	bool value)
{
	// Set the new value.
	if (realized == value)
		return;
	realized = value;

	// Add to or remove from the physics world.
	if (value)
	{
		create_shape ();
		create_object ();
	}
	else
	{
		delete_object();
		delete_shape();
	}
}

void LIExtPhysicsTerrainChunk::create_object ()
{
	float grid_size = terrain->terrain->grid_size;

	object = new btCollisionObject ();
	object->setCollisionFlags (btCollisionObject::CF_STATIC_OBJECT);
	object->setActivationState (DISABLE_DEACTIVATION);
	object->setCollisionShape (shape);
	object->setWorldTransform (btTransform (btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(x * grid_size, 0.0, z * grid_size)));
	object->setUserPointer (terrain->pointer);
	object->setFriction (terrain->friction);
	object->setCollisionFlags (object->getCollisionFlags() |
		btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	terrain->module->physics->dynamics->addCollisionObject (object, terrain->collision_group, terrain->collision_mask);
}

void LIExtPhysicsTerrainChunk::create_shape ()
{
	float grid_size = terrain->terrain->grid_size;

	shape = new btCompoundShape();
	shape->setUserPointer(terrain);

	for (int z = 0 ; z < size ; z++)
	for (int x = 0 ; x < size ; x++)
	{
		// Get the column.
		LIExtTerrainColumn* column = liext_terrain_chunk_get_column (chunk, x, z);

		// Initialize the column position.
		btVector3 offset = btVector3 (x, 0.0f, z) * grid_size;
		btQuaternion rotation (0.0, 0.0, 0.0, 1.0);
		btTransform transform (rotation, offset);

		// Initialize the stick vertices.
		btVector3 verts[8] =
		{
			btVector3(     0.0f, 0.0f, 0.0f),
			btVector3(grid_size, 0.0f, 0.0f),
			btVector3(     0.0f, 0.0f, grid_size),
			btVector3(grid_size, 0.0f, grid_size),
			btVector3(     0.0f, 0.0f, 0.0f),
			btVector3(grid_size, 0.0f, 0.0f),
			btVector3(     0.0f, 0.0f, grid_size),
			btVector3(grid_size, 0.0f, grid_size)
		};

		// Create a convex hull shape for each stick.
		float ys = 0.0f;
		LIExtTerrainStick* stick;
		for (stick = column->sticks ; stick != NULL ; stick = stick->next)
		{
			ys += stick->height;

			// Set the top surface offset.
			verts[4][1] = ys + stick->vertices[0][0].offset;
			verts[5][1] = ys + stick->vertices[1][0].offset;
			verts[6][1] = ys + stick->vertices[0][1].offset;
			verts[7][1] = ys + stick->vertices[1][1].offset;

			// Create the convex hull shape.
			if (stick->material != 0)
			{
				btConvexHullShape* s = new LIExtPhysicsTerrainStickShape (this->x + x, this->z + z, verts, 8);
				shape->addChildShape(transform, s);
			}

			// Set the bottom surface offset.
			verts[0] = verts[4];
			verts[1] = verts[5];
			verts[2] = verts[6];
			verts[3] = verts[7];
		}
	}
}

void LIExtPhysicsTerrainChunk::delete_object ()
{
	if (!object)
		return;
	terrain->module->physics->dynamics->removeCollisionObject (object);
	delete object;
	object = NULL;
}

void LIExtPhysicsTerrainChunk::delete_shape ()
{
	if (!shape)
		return;
	while (shape->getNumChildShapes())
	{
		btCollisionShape* s = shape->getChildShape(0);
		shape->removeChildShapeByIndex(0);
		delete s;
	}
	delete shape;
	shape = NULL;
}

/** @} */
/** @} */
