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
 * \addtogroup LIPhyModel Model
 * @{
 */

#include <lipsofsuna/system.h>
#include "physics-model.h"
#include "physics-private.h"
#include "physics-shape.h"

#define BUILT_OBJECT 0x01
#define BUILT_TERRAIN 0x02

static int private_build_object (
	LIPhyModel* self);

static int private_build_terrain (
	LIPhyModel*       self,
	const LIMdlModel* model);

static int private_build_terrain_internal (
	LIPhyModel*         self,
	int                 index,
	const unsigned int* index_array,
	int                 index_count,
	const btScalar*     vertex_array,
	int                 vertex_count);

static int private_build_terrain_reduce (
	LIPhyModel* self,
	int         index,
	btScalar*   vertex_array,
	int         vertex_count);

static int private_build_terrain_triangulate (
	LIPhyModel* self,
	int         index,
	btScalar*   vertex_array,
	int         vertex_count);

static void private_clear_object (
	LIPhyModel* self);

static void private_clear_terrain (
	LIPhyModel* self);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The caller must ensure that the model data remains valid until the physics
 * model is freed.
 *
 * \param physics Physics engine.
 * \param model Model data.
 * \param id Unique model ID.
 * \return New model or NULL.
 */
LIPhyModel* liphy_model_new (
	LIPhyPhysics* physics,
	LIMdlModel*   model,
	int           id)
{
	LIPhyModel* self;

	/* Allocate self. */
	self = (LIPhyModel*) lisys_calloc (1, sizeof (LIPhyModel));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->model = model;
	self->id = id;

	/* Add to dictionary. */
	if (!lialg_u32dic_insert (physics->models, id, self))
	{
		liphy_model_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the model.
 * \param self Model.
 */
void liphy_model_free (LIPhyModel* self)
{
	/* Remove from dictionary. */
	lialg_u32dic_remove (self->physics->models, self->id);

	/* Free self. */
	private_clear_object (self);
	private_clear_terrain (self);
	lisys_free (self);
}

/**
 * \brief Builds object or terrain data form the model.
 * \param self Model.
 * \param terrain Zero to build object data, nonzero to build terrain.
 * \return Nonzero on success.
 */
int liphy_model_build (
	LIPhyModel* self,
	int         terrain)
{
	if (terrain)
	{
		if (self->flags & BUILT_TERRAIN)
			return 1;
		if (!private_build_terrain (self, self->model))
			return 0;
		self->flags |= BUILT_TERRAIN;
	}
	else
	{
		if (self->flags & BUILT_OBJECT)
			return 1;
		if (!private_build_object (self))
			return 0;
		self->flags |= BUILT_OBJECT;
	}

	return 1;
}

int liphy_model_set_model (
	LIPhyModel* self,
	LIMdlModel* model)
{
	/* Terrain data can't be rebuilt, though we don't need that right now anyway. */
	if (self->shape != NULL)
	{
		liphy_shape_clear (self->shape);
		liphy_shape_add_model (self->shape, self->model, NULL, 1.0f);
	}
	self->model = model;

	return 1;
}

/*****************************************************************************/

static int private_build_object (
	LIPhyModel* self)
{
	LIPhyShape* shape;

	shape = liphy_shape_new (self->physics);
	if (shape == NULL)
		return 0;
	if (!liphy_shape_add_model (shape, self->model, NULL, 1.0f))
	{
		liphy_shape_free (shape);
		return 0;
	}
	self->shape = shape;

	return 1;
}

static int private_build_terrain (
	LIPhyModel*       self,
	const LIMdlModel* model)
{
	int i;
	int j;
	int num;
	int ret = 1;
	btScalar* tmp;
	LIMatVector vector;
	LIMdlShape* shape;

	if (model->shapes.count)
	{
		/* Count convex hulls in the model. */
		shape = model->shapes.array + 0;
		for (i = num = 0 ; i < shape->parts.count ; i++)
		{
			if (shape->parts.array[i].vertices.count)
				num++;
		}
		if (!num)
			return 1;

		/* Allocate space for meshes. */
		self->meshes.array = (LIPhyModelMesh*) lisys_calloc (num, sizeof (LIPhyModelMesh));
		if (self->meshes.array == NULL)
			return 0;

		/* Triangulate each convex hull. */
		for (i = 0 ; i < shape->parts.count ; i++)
		{
			if (!shape->parts.array[i].vertices.count)
				continue;
			tmp = new btScalar[4 * shape->parts.array[i].vertices.count];
			for (j = 0 ; j < shape->parts.array[i].vertices.count ; j++)
			{
				vector = shape->parts.array[i].vertices.array[j];
				tmp[4 * j + 0] = vector.x;
				tmp[4 * j + 1] = vector.y;
				tmp[4 * j + 2] = vector.z;
				tmp[4 * j + 3] = 0.0f;
			}
			ret = private_build_terrain_triangulate (self, self->meshes.count,
				tmp, shape->parts.array[i].vertices.count);
			delete[] tmp;
			if (ret)
				self->meshes.count++;
			else
				break;
		}

		/* Handle errors. */
		if (!ret)
		{
			private_clear_terrain (self);
			return 0;
		}
	}
	else if (model->vertices.count)
	{
		/* Allocate terrain meshes. */
		self->meshes.array = (LIPhyModelMesh*) lisys_calloc (1, sizeof (LIPhyModelMesh));
		if (self->meshes.array == NULL)
			return 0;

		/* Create a convex hull and triangulate it. */
		tmp = new btScalar[model->vertices.count];
		for (j = 0 ; j < model->vertices.count ; j++)
		{
			vector = model->vertices.array[j].coord;
			tmp[4 * j + 0] = vector.x;
			tmp[4 * j + 1] = vector.y;
			tmp[4 * j + 2] = vector.z;
			tmp[4 * j + 3] = 0.0f;
		}
		if (model->vertices.count < 42)
			ret = private_build_terrain_triangulate (self, 0, tmp, model->vertices.count);
		else
			ret = private_build_terrain_reduce (self, 0, tmp, model->vertices.count);
		delete[] tmp;

		/* Handle errors. */
		if (!ret)
		{
			lisys_free (self->meshes.array);
			self->meshes.array = NULL;
		}
		else
			self->meshes.count = 1;
	}

	return ret;
}

static int private_build_terrain_internal (
	LIPhyModel*         self,
	int                 index,
	const unsigned int* index_array,
	int                 index_count,
	const btScalar*     vertex_array,
	int                 vertex_count)
{
	int i;
	int* indices;
	btScalar* vertices;
	LIPhyModelMesh* mesh;

	mesh = self->meshes.array + index;

	/* Allocate data. */
	indices = (int*) lisys_calloc (mesh->indices.count + index_count, sizeof (int));
	if (indices == NULL)
		return 0;
	mesh->indices.array = indices;
	vertices = (btScalar*) lisys_calloc (mesh->vertices.count + vertex_count, 4 * sizeof (btScalar));
	if (vertices == NULL)
	{
		lisys_free (mesh->indices.array);
		mesh->indices.array = NULL;
		return 0;
	}
	mesh->vertices.array = vertices;

	/* Append data. */
	for (i = 0 ; i < index_count ; i++)
		indices[mesh->indices.count++] = index_array[i];
	for (i = 0 ; i < vertex_count ; i++)
	{
		vertices[4 * mesh->vertices.count + 0] = vertex_array[4 * i + 0];
		vertices[4 * mesh->vertices.count + 1] = vertex_array[4 * i + 1];
		vertices[4 * mesh->vertices.count + 2] = vertex_array[4 * i + 2];
		vertices[4 * mesh->vertices.count + 3] = 0.0f;
		mesh->vertices.count++;
	}

	return 1;
}

static int private_build_terrain_reduce (
	LIPhyModel* self,
	int         index,
	btScalar*   vertex_array,
	int         vertex_count)
{
	int ret = 0;
	btShapeHull* hull = NULL;

	try
	{
		btConvexHullShape shape(vertex_array, vertex_count, 4 * sizeof (btScalar));
		hull = new btShapeHull (&shape);
		hull->buildHull (shape.getMargin ());
		ret = private_build_terrain_internal (self, index,
			hull->getIndexPointer (), hull->numIndices (),
			(btScalar*) hull->getVertexPointer (), hull->numVertices ());
		delete hull;
	}
	catch (...)
	{
		delete hull;
	}

	return ret;
}

static int private_build_terrain_triangulate (
	LIPhyModel* self,
	int         index,
	btScalar*   vertex_array,
	int         vertex_count)
{
	int i;
	int ret;
	int* idx;
	btScalar* tmp;
	HullDesc desc;
	HullLibrary lib;
	HullResult hull;

	/* Triangulate the shape. */
	desc.mFlags = QF_TRIANGLES;
	desc.mVcount = vertex_count;
	desc.mVertices = (btVector3*) vertex_array;
	desc.mVertexStride = 4 * sizeof (btScalar);
	if (lib.CreateConvexHull (desc, hull) == QE_FAIL)
	{
		lisys_error_set (EINVAL, "creating the convex hull failed");
		return 0;
	}

	/* Add the triangles. */
	idx = new int[hull.mNumIndices];
	for (i = 0 ; i < (int) hull.mNumIndices ; i++)
		idx[i] = hull.m_Indices[i];
	tmp = new btScalar[4 * hull.mNumOutputVertices];
	for (i = 0 ; i < (int) hull.mNumOutputVertices ; i++)
	{
		tmp[4 * i + 0] = hull.m_OutputVertices[i][0];
		tmp[4 * i + 1] = hull.m_OutputVertices[i][1];
		tmp[4 * i + 2] = hull.m_OutputVertices[i][2];
		tmp[4 * i + 3] = 0.0f;
	}
	ret = private_build_terrain_internal (self, index,
		(unsigned int*) idx, hull.mNumIndices, tmp, hull.mNumOutputVertices);
	lib.ReleaseResult (hull);
	delete[] tmp;
	delete[] idx;

	return 1;
}

static void private_clear_object (
	LIPhyModel* self)
{
	if (self->shape != NULL)
	{
		liphy_shape_free (self->shape);
		self->shape = NULL;
	}
}

static void private_clear_terrain (
	LIPhyModel* self)
{
	int i;
	LIPhyModelMesh* mesh;

	for (i = 0 ; i < self->meshes.count ; i++)
	{
		mesh = self->meshes.array + i;
		lisys_free (mesh->indices.array);
		lisys_free (mesh->vertices.array);
	}
	lisys_free (self->meshes.array);
	self->meshes.count = 0;
	self->meshes.array = NULL;
}

/** @} */
/** @} */
