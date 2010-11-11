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
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyTerrain Terrain
 * @{
 */

#include <lipsofsuna/system.h>
#include "physics-model.h"
#include "physics-terrain.h"
#include "physics-private.h"

#define VERTEX_WELD_EPSILON 0.05
#define DEFAULT_INDEX_CAPACITY 64
#define DEFAULT_VERTEX_CAPACITY 32

static int private_add_vertices (
	LIPhyTerrain*       self,
	int                 index,
	float               scale,
	const unsigned int* index_array,
	int                 index_count,
	const btScalar*     vertex_array,
	int                 vertex_count);

/*****************************************************************************/

/**
 * \brief Creates an empty terrain block.
 * \param physics Physics engine.
 * \param offset Region offset, in tile.
 * \param size Region size, in tiles.
 * \param collision_group Collision group.
 * \param collision_mask Collision mask.
 * \return New terrain block or NULL.
 */
LIPhyTerrain* liphy_terrain_new (
	LIPhyPhysics* physics,
	const int*    offset,
	const int*    size,
	int           collision_group,
	int           collision_mask)
{
	LIPhyTerrain* self;

	self = (LIPhyTerrain*) lisys_calloc (1, sizeof (LIPhyTerrain));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	memcpy (self->offset, offset, 3 * sizeof (int));
	memcpy (self->size, size, 3 * sizeof (int));
	self->collision_group = collision_group;
	self->collision_mask = collision_mask;

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
	liphy_terrain_clear (self);
	lisys_free (self);
}

/**
 * \brief Clears the terrain block.
 * \param self Terrain.
 */
void liphy_terrain_clear (
	LIPhyTerrain* self)
{
	if (self->realized && self->object != NULL)
		self->physics->dynamics->removeCollisionObject (self->object);
	if (self->object != NULL)
		lisys_free (self->object->getUserPointer ());
	delete self->object;
	delete self->shape;
	delete self->vertex_array;
	self->object = NULL;
	self->shape = NULL;
	self->vertex_array = NULL;

	lisys_free (self->indices.array);
	self->indices.count = 0;
	self->indices.capacity = 0;
	self->indices.array = NULL;
	lisys_free (self->materials.array);
	self->materials.count = 0;
	self->materials.capacity = 0;
	self->materials.array = NULL;
	lisys_free (self->vertices.array);
	self->vertices.count = 0;
	self->vertices.capacity = 0;
	self->vertices.array = NULL;
}

/**
 * \brief Merges a model to the terrain.
 * \param self Terrain.
 * \param model Model.
 * \param transform Model transformation or NULL for identity.
 * \param scale Scale factor.
 * \return Nonzero on success.
 */
int liphy_terrain_add_model (
	LIPhyTerrain*         self,
	int                   index,
	LIPhyModel*           model,
	const LIMatTransform* transform,
	float                 scale)
{
	int i;
	int j;
	int ret = 1;
	btScalar* tmp;
	btScalar* src;
	LIMatVector vector;
	LIPhyModelMesh* mesh;

	if (!liphy_model_build (model, 1))
		return 0;
	for (i = 0 ; i < model->meshes.count ; i++)
	{
		mesh = model->meshes.array + i;
		tmp = new btScalar[4 * mesh->vertices.count];
		for (j = 0 ; j < mesh->vertices.count ; j++)
		{
			src = mesh->vertices.array + 4 * j;
			vector = limat_vector_init (src[0], src[1], src[2]);
			vector = limat_vector_multiply (vector, scale);
			vector = limat_transform_transform (*transform, vector);
			tmp[4 * j + 0] = vector.x;
			tmp[4 * j + 1] = vector.y;
			tmp[4 * j + 2] = vector.z;
			tmp[4 * j + 3] = 0.0f;
		}
		ret &= private_add_vertices (self, index, 1.0f,
			(const unsigned int*) mesh->indices.array, mesh->indices.count,
			tmp, mesh->vertices.count);
		delete[] tmp;
	}

	return ret;
}

/**
 * \brief Merges triangles to the terrain.
 * \param self Terrain.
 * \param vertices Array of vertices.
 * \param count Number of vertices.
 * \param transform Shape transformation or NULL for identity.
 * \return Nonzero on success.
 */
int liphy_terrain_add_vertices (
	LIPhyTerrain*         self,
	int                   index,
	const LIMatVector*    vertices,
	int                   count,
	const LIMatTransform* transform)
{
	int i;
	int ret = 1;
	float scale = 1.0f;
	unsigned int* idx;
	btScalar* tmp;
	LIMatVector vector;

	if (count)
	{
		idx = new unsigned int[count];
		tmp = new btScalar[4 * count];
		for (i = 0 ; i < count ; i++)
			idx[i] = i;
		for (i = 0 ; i < count ; i++)
		{
			vector = vertices[i];
			vector = limat_vector_multiply (vector, scale);
			vector = limat_transform_transform (*transform, vector);
			tmp[4 * i + 0] = vector.x;
			tmp[4 * i + 1] = vector.y;
			tmp[4 * i + 2] = vector.z;
			tmp[4 * i + 3] = 0.0f;
		}
		ret = private_add_vertices (self, index, 1.0f, idx, count, tmp, count);
		delete[] tmp;
		delete[] idx;
	}

	return ret;
}

int liphy_terrain_cast_ray (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	LIPhyCollision*     result)
{
	btVector3 p;
	btVector3 a (start->x, start->y, start->z);
	btVector3 b (end->x, end->y, end->z);
	LIPhyPrivateRaycastTerrain callback (a, b);

	self->shape->performRaycast (&callback, a, b);
	if (result->fraction >= 1.0f)
		return 0;
	result->terrain_index = self->materials.array[callback.triangle_index];
	result->fraction = callback.m_hitFraction;
	p = callback.normal;
	result->normal = limat_vector_init (p[0], p[1], p[2]);
	p = result->fraction * (b - a);
	result->point = limat_vector_init (p[0], p[1], p[2]);
	result->object = NULL;
	result->terrain = (LIPhyTerrain*) self;

	return 1;
}

int liphy_terrain_cast_shape (
	const LIPhyTerrain* self,
	const LIMatTransform* start,
	const LIMatTransform* end,
	const LIPhyShape*     shape,
	LIPhyCollision*       result)
{
	/* TODO */
	return 0;
}

int liphy_terrain_cast_sphere (
	const LIPhyTerrain* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	float               radius,
	LIPhyCollision*     result)
{
	btVector3 p;
	btVector3 a (start->x, start->y, start->z);
	btVector3 b (end->x, end->y, end->z);
	btVector3 size (radius, radius, radius);
	btQuaternion rot (0.0f, 0.0f, 0.0f, 1.0f);
	btSphereShape shape (radius);
	LIPhyPrivateConvexcastTerrain callback (&shape,
		btTransform (rot, a), btTransform (rot, b),
		btTransform (rot, btVector3 (0.0f, 0.0f, 0.0f)), 0.2f);

	self->shape->performConvexcast (&callback, a, b, -size, size);
	if (result->fraction >= 1.0f)
		return 0;
	result->terrain_index = self->materials.array[callback.triangle_index];
	result->fraction = callback.m_hitFraction;
	p = callback.normal;
	result->normal = limat_vector_init (p[0], p[1], p[2]);
	p = callback.point;
	result->point = limat_vector_init (p[0], p[1], p[2]);
	result->object = NULL;
	result->terrain = (LIPhyTerrain*) self;

	return 1;
}

/**
 * \brief Gets the world coordinates of a tile by its index.
 * \param self Terrain.
 * \param index Tile index.
 * \param result Return location for the coordinates, in tiles.
 */
void liphy_terrain_get_tile (
	LIPhyTerrain* self,
	int           index,
	LIMatVector*  result)
{
	int i;

	i = index;
	result->x = self->offset[0] + (i % self->size[0]);
	i /= self->size[0];
	result->y = self->offset[1] + (i % self->size[1]);
	i /= self->size[1];
	result->z = self->offset[2] + (i % self->size[2]);
	i /= self->size[2];
}

/**
 * \brief Realizes or unrealizes the terrain.
 * \param self Terrain.
 */
void liphy_terrain_set_realized (
	LIPhyTerrain* self,
	int           value)
{
	btQuaternion r(0.0f, 0.0f, 0.0f, 1.0f);
	btVector3 p(0.0f, 0.0f, 0.0f);
	LIPhyPointer* pointer;

	if (self->realized == value)
		return;
	self->realized = value;
	if (value)
	{
		/* Prepare collision object to tile index lookup. */
		pointer = (LIPhyPointer*) lisys_calloc (1, sizeof (LIPhyPointer));
		if (pointer == NULL)
			return;
		pointer->object = 0;
		pointer->pointer = self;

		/* Create the collision shape. */
		self->vertex_array = new btTriangleIndexVertexArray (
			self->indices.count / 3, self->indices.array, 3 * sizeof (int),
			self->vertices.count, self->vertices.array, 4 * sizeof (btScalar));
		self->shape = new btBvhTriangleMeshShape (self->vertex_array, false);

		/* Create the collision object. */
		self->object = new btCollisionObject ();
		self->object->setCollisionShape (self->shape);
		self->object->setWorldTransform (btTransform (r, p));
		self->object->setUserPointer (pointer);
		self->physics->dynamics->addCollisionObject (self->object,
			self->collision_group, self->collision_mask);
	}
	else
	{
		if (self->object != NULL)
		{
			self->physics->dynamics->removeCollisionObject (self->object);
			lisys_free (self->object->getUserPointer ());
		}
		delete self->object;
		delete self->shape;
		delete self->vertex_array;
		self->object = NULL;
		self->shape = NULL;
		self->vertex_array = NULL;
	}
}

/*****************************************************************************/

static int private_add_vertices (
	LIPhyTerrain*       self,
	int                 index,
	float               scale,
	const unsigned int* index_array,
	int                 index_count,
	const btScalar*     vertex_array,
	int                 vertex_count)
{
	int i;
	int cap;
	int need;
	int* indices;
	int* materials;
	btScalar* vertices;

	/* Allocate indices. */
	cap = self->indices.capacity;
	need = self->indices.count + index_count;
	if (cap < need)
	{
		cap = LIMAT_MAX (cap, DEFAULT_INDEX_CAPACITY);
		while (cap < need)
			cap <<= 1;
		indices = (int*) lisys_realloc (self->indices.array, cap * sizeof (int));
		if (indices == NULL)
			return 0;
		self->indices.array = indices;
		self->indices.capacity = cap;
	}
	else
		indices = self->indices.array;

	/* Allocate materials. */
	cap = self->materials.capacity;
	need = self->materials.count + index_count / 3;
	if (cap < need)
	{
		cap = LIMAT_MAX (cap, DEFAULT_INDEX_CAPACITY / 3);
		while (cap < need)
			cap <<= 1;
		materials = (int*) lisys_realloc (self->materials.array, cap * sizeof (int));
		if (materials == NULL)
			return 0;
		self->materials.array = materials;
		self->materials.capacity = cap;
	}
	else
		materials = self->materials.array;

	/* Allocate vertices. */
	cap = self->vertices.capacity;
	need = self->vertices.count + vertex_count;
	if (cap < need)
	{
		cap = LIMAT_MAX (cap, DEFAULT_VERTEX_CAPACITY);
		while (cap < need)
			cap <<= 1;
		vertices = (btScalar*) lisys_realloc (self->vertices.array, cap * 4 * sizeof (btScalar));
		if (vertices == NULL)
			return 0;
		self->vertices.array = vertices;
		self->vertices.capacity = cap;
	}
	else
		vertices = self->vertices.array;

	/* Append data. */
	for (i = 0 ; i < index_count ; i++)
		indices[self->indices.count++] = self->vertices.count + index_array[i];
	for (i = 0 ; i < index_count / 3 ; i++)
		materials[self->materials.count++] = index;
	for (i = 0 ; i < vertex_count ; i++)
	{
		vertices[4 * self->vertices.count + 0] = scale * vertex_array[4 * i + 0];
		vertices[4 * self->vertices.count + 1] = scale * vertex_array[4 * i + 1];
		vertices[4 * self->vertices.count + 2] = scale * vertex_array[4 * i + 2];
		vertices[4 * self->vertices.count + 3] = 0.0f;
		self->vertices.count++;
	}

	return 1;
}

/** @} */
/** @} */
