/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphySector Sector
 * @{
 */

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <system/lips-system.h>
#include "physics-shape.h"
#include "physics-private.h"

static btMultiSphereShape*
private_create_capsule_from_aabb (liphyShape*      self,
                                  const limatAabb* aabb);

static btMultiSphereShape*
private_create_capsule_from_vertices (liphyShape* self,
                                      btScalar*   vertices,
                                      int         count);

static btBvhTriangleMeshShape*
private_create_concave_from_vertices (liphyShape* self,
                                      int*        index_array,
                                      int         index_count,
                                      btScalar*   vertex_array,
                                      int         vertex_count);

static btConvexHullShape*
private_create_convex_from_aabb (liphyShape*      self,
                                 const limatAabb* aabb);

static btConvexHullShape*
private_create_convex_from_vertices (liphyShape* self,
                                     btScalar*   vertices,
                                     int         count);

static inline int
private_init_box (liphyShape*      self,
                  const limatAabb* aabb);

static inline int
private_init_concave (liphyShape*       self,
                     const limatVector* vertices,
                     int                count);

static inline int
private_init_convex (liphyShape*        self,
                     const limatVector* vertices,
                     int                count);

static inline int
private_init_model (liphyShape*       self,
                    const limdlModel* model);

/*****************************************************************************/

/**
 * \brief Creates a collision shape from a model.
 *
 * The model is stored as a reference to the shape and must not be freed
 * before the shape.
 *
 * \param physics Physics engine.
 * \param model Referenced model.
 * \return New collision shape or NULL.
 */
liphyShape*
liphy_shape_new (liphyPhysics*     physics,
                 const limdlModel* model)
{
	liphyShape* self;

	self = (liphyShape*) lisys_calloc (1, sizeof (liphyShape));
	if (self == NULL)
		return NULL;
	self->model = model;
	try
	{
		self->physics = physics;
		if (!private_init_model (self, model))
		{
			liphy_shape_free (self);
			return NULL;
		}
	}
	catch (...)
	{
		lisys_error_set (ENOMEM, NULL);
		liphy_shape_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a collision shape from a bounding box.
 *
 * \param physics Physics engine.
 * \param aabb Axis-aligned bounding box.
 * \return New collision shape or NULL.
 */
liphyShape*
liphy_shape_new_aabb (liphyPhysics*    physics,
                      const limatAabb* aabb)
{
	liphyShape* self;

	self = (liphyShape*) lisys_calloc (1, sizeof (liphyShape));
	if (self == NULL)
		return NULL;
	try
	{
		self->physics = physics;
		if (!private_init_box (self, aabb))
		{
			liphy_shape_free (self);
			return NULL;
		}
	}
	catch (...)
	{
		lisys_error_set (ENOMEM, NULL);
		liphy_shape_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a concave collision shape from a triangle list.
 *
 * \param physics Physics engine.
 * \param vertices Vertex array.
 * \param count Number of vertices.
 * \return New collision shape or NULL.
 */
liphyShape*
liphy_shape_new_concave (liphyPhysics*      physics,
                         const limatVector* vertices,
                         int                count)
{
	liphyShape* self;

	self = (liphyShape*) lisys_calloc (1, sizeof (liphyShape));
	if (self == NULL)
		return NULL;
	try
	{
		self->physics = physics;
		if (!private_init_concave (self, vertices, count))
		{
			liphy_shape_free (self);
			return NULL;
		}
	}
	catch (...)
	{
		lisys_error_set (ENOMEM, NULL);
		liphy_shape_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a convex collision shape from a triangle list.
 *
 * \param physics Physics engine.
 * \param vertices Vertex array.
 * \param count Number of vertices.
 * \return New collision shape or NULL.
 */
liphyShape*
liphy_shape_new_convex (liphyPhysics*      physics,
                        const limatVector* vertices,
                        int                count)
{
	liphyShape* self;

	self = (liphyShape*) lisys_calloc (1, sizeof (liphyShape));
	if (self == NULL)
		return NULL;
	try
	{
		self->physics = physics;
		if (!private_init_convex (self, vertices, count))
		{
			liphy_shape_free (self);
			return NULL;
		}
	}
	catch (...)
	{
		lisys_error_set (ENOMEM, NULL);
		liphy_shape_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the collision shape.
 *
 * \param self Collision shape.
 */
void
liphy_shape_free (liphyShape* self)
{
	if (self->shapes.box != NULL)
		delete self->shapes.box;
	if (self->shapes.capsule != NULL)
		delete self->shapes.capsule;
	if (self->shapes.concave != NULL)
		delete self->shapes.concave;
	if (self->shapes.convex != NULL)
		delete self->shapes.convex;
	if (self->mesh != NULL)
		delete self->mesh;
	lisys_free (self->indices.array);
	lisys_free (self->vertices.array);
	lisys_free (self);
}

/**
 * \brief Gets the local inertia vector of the shape.
 *
 * \param self Collision shape.
 * \param mass Mass.
 * \param result Return location for the inertia vector.
 */
void
liphy_shape_get_inertia (const liphyShape* self,
                         float             mass,
                         limatVector*      result)
{
	btVector3 inertia;

	/* TODO: Could this be precalculated? */
	if (self->shapes.convex != NULL)
	{
		self->shapes.convex->calculateLocalInertia (mass, inertia);
		result->x = inertia[0];
		result->y = inertia[1];
		result->z = inertia[2];
	}
	else
		*result = limat_vector_init (0.0f, 0.0f, 0.0f);
}

/*****************************************************************************/

static btMultiSphereShape*
private_create_capsule_from_aabb (liphyShape*      self,
                                  const limatAabb* aabb)
{
	float radius;
	limatVector center;
	limatVector size;
	btMultiSphereShape* shape;

	size = limat_vector_subtract (aabb->max, aabb->min);
	center = limat_vector_add (aabb->max, aabb->min);
	center = limat_vector_multiply (center, 0.5f);
	radius = LI_MIN (size.x, size.y);
	if (size.y > 2.0 * radius)
	{
		btScalar radii[2] = { radius, radius };
		btVector3 centers[2] =
		{
			btVector3 (center.x, center.y + (0.5 * size.y - radius), center.z),
			btVector3 (center.x, center.y - (0.5 * size.y - radius), center.z)
		};
		btVector3 inertia (radius, 0.5 * size.y, radius);
		shape = new btMultiSphereShape (inertia, centers, radii, 2);
	}
	else
	{
		btScalar radii[1] = { 0.5 * size.y };
		btVector3 centers[1] = { btVector3 (center.x, center.y, center.z) };
		btVector3 inertia (0.5 * size.y, 0.5 * size.y, 0.5 * size.y);
		shape = new btMultiSphereShape (inertia, centers, radii, 1);
	}

	return shape;
}

static btMultiSphereShape*
private_create_capsule_from_vertices (liphyShape* self,
                                      btScalar*   vertices,
                                      int         count)
{
	int i;
	limatAabb aabb;
	limatVector point;

	if (!count)
	{
		aabb.min = limat_vector_init (0.0f, 0.0f, 0.0f);
		aabb.max = aabb.min;
	}
	else
	{
		aabb.min = limat_vector_init (vertices[0], vertices[1], vertices[2]);
		aabb.max = aabb.min;
		for (i = 1 ; i < count ; i++)
		{
			point = limat_vector_init (vertices[4 * i + 0], vertices[4 * i + 1], vertices[4 * i + 2]);
			if (aabb.min.x > point.x)
				aabb.min.x = point.x;
			if (aabb.min.y > point.y)
				aabb.min.y = point.y;
			if (aabb.min.z > point.z)
				aabb.min.z = point.z;
			if (aabb.max.x < point.x)
				aabb.max.x = point.x;
			if (aabb.max.y < point.y)
				aabb.max.y = point.y;
			if (aabb.max.z < point.z)
				aabb.max.z = point.z;
		}
	}

	return private_create_capsule_from_aabb (self, &aabb);
}

static btBvhTriangleMeshShape*
private_create_concave_from_vertices (liphyShape* self,
                                      int*        index_array,
                                      int         index_count,
                                      btScalar*   vertex_array,
                                      int         vertex_count)
{
	self->mesh = new btTriangleIndexVertexArray (
		index_count / 3, index_array, 3 * sizeof (int),
		vertex_count, vertex_array, 4 * sizeof (btScalar));

	return new btBvhTriangleMeshShape (self->mesh, true);
}

static btConvexHullShape*
private_create_convex_from_aabb (liphyShape*      self,
                                 const limatAabb* aabb)
{
	btScalar vertices[8*4] =
	{
		aabb->min.x, aabb->min.y, aabb->min.z, 0.0,
		aabb->min.x, aabb->min.y, aabb->max.z, 0.0,
		aabb->min.x, aabb->max.y, aabb->min.z, 0.0,
		aabb->min.x, aabb->max.y, aabb->max.z, 0.0,
		aabb->max.x, aabb->min.y, aabb->min.z, 0.0,
		aabb->max.x, aabb->min.y, aabb->max.z, 0.0,
		aabb->max.x, aabb->max.y, aabb->min.z, 0.0,
		aabb->max.x, aabb->max.y, aabb->max.z, 0.0
	};

	return new btConvexHullShape (vertices, 8, 4 * sizeof (btScalar));
}

static btConvexHullShape*
private_create_convex_from_vertices (liphyShape* self,
                                     btScalar*   vertices,
                                     int         count)
{
	btShapeHull* hull;
	btConvexHullShape* shape;
	btConvexHullShape* shape1;

	shape1 = new btConvexHullShape (vertices, count, 4 * sizeof (btScalar));
	try
	{
		hull = new btShapeHull (shape1);
		hull->buildHull (shape1->getMargin ());
		shape = new btConvexHullShape ((btScalar*) hull->getVertexPointer (), hull->numVertices ());
		delete shape1;
		delete hull;

		return shape;
	}
	catch (...)
	{
	}

	return shape1;
}

static inline int
private_init_box (liphyShape*      self,
                  const limatAabb* aabb)
{
	self->shapes.box = private_create_convex_from_aabb (self, aabb);
	self->shapes.capsule = private_create_capsule_from_aabb (self, aabb);
	self->shapes.convex = private_create_convex_from_aabb (self, aabb);

	return 1;
}

static inline int
private_init_concave (liphyShape*       self,
                     const limatVector* vertices,
                     int                count)
{
	int i;

	/* Allocate indices. */
	/* FIXME: Pretty pointless to have something like this. */
	self->indices.count = count;
	self->indices.array = (int*) lisys_calloc (self->indices.count, sizeof (int));
	if (self->indices.array == NULL)
		return 0;
	for (i = 0 ; i < self->indices.count ; i++)
		self->indices.array[i] = i;

	/* Allocate vertices. */
	self->vertices.count = count;
	self->vertices.array = (btScalar*) lisys_calloc (4 * self->vertices.count, sizeof (btScalar));
	if (self->vertices.array == NULL)
		return 0;
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		self->vertices.array[4 * i + 0] = vertices[i].x;
		self->vertices.array[4 * i + 1] = vertices[i].y;
		self->vertices.array[4 * i + 2] = vertices[i].z;
		self->vertices.array[4 * i + 3] = 0.0;
	}

	/* Create shapes. */
	self->shapes.box = private_create_convex_from_vertices (self,
		self->vertices.array, self->vertices.count);
	self->shapes.capsule = private_create_capsule_from_vertices (self,
		self->vertices.array, self->vertices.count);
	self->shapes.concave = private_create_concave_from_vertices (self,
		self->indices.array, self->indices.count,
		self->vertices.array, self->vertices.count);
	self->shapes.convex = private_create_convex_from_vertices (self,
		self->vertices.array, self->vertices.count);

	return 1;
}

static inline int
private_init_convex (liphyShape*        self,
                     const limatVector* vertices,
                     int                count)
{
	int i;
	btScalar* tmp = new btScalar[4 * count];

	for (i = 0 ; i < count ; i++)
	{
		tmp[4 * i + 0] = vertices[i].x;
		tmp[4 * i + 1] = vertices[i].y;
		tmp[4 * i + 2] = vertices[i].z;
		tmp[4 * i + 3] = 0.0;
	}
	self->shapes.box = private_create_convex_from_vertices (self, tmp, count);
	self->shapes.capsule = private_create_capsule_from_vertices (self, tmp, count);
	self->shapes.convex = private_create_convex_from_vertices (self, tmp, count);
	delete[] tmp;

	return 1;
}

static inline int
private_init_model (liphyShape*       self,
                    const limdlModel* model)
{
	int i;
	int j;
	int k;
	int count;
	limdlFaces* group;

	count = limdl_model_get_index_count (model);
	if (!count)
		return 1;

	/* Allocate indices. */
	/* FIXME: Pretty pointless to have something like this. */
	self->indices.count = count;
	self->indices.array = (int*) lisys_calloc (self->indices.count, sizeof (int));
	if (self->indices.array == NULL)
		return 0;
	for (i = 0 ; i < self->indices.count ; i++)
		self->indices.array[i] = i;

	/* Allocate vertices. */
	self->vertices.count = count;
	self->vertices.array = (btScalar*) lisys_calloc (4 * self->vertices.count, sizeof (btScalar));
	if (self->vertices.array == NULL)
		return 0;
	for (i = j = 0 ; j < model->facegroups.count ; j++)
	{
		group = model->facegroups.array + j;
		for (k = 0 ; k < group->vertices.count ; k++)
		{
			self->vertices.array[i++] = group->vertices.array[k].coord.x;
			self->vertices.array[i++] = group->vertices.array[k].coord.y;
			self->vertices.array[i++] = group->vertices.array[k].coord.z;
			self->vertices.array[i++] = 0.0;
		}
	}

	/* Create shapes. */
	self->shapes.box = private_create_convex_from_vertices (self,
		self->vertices.array, self->vertices.count);
	self->shapes.capsule = private_create_capsule_from_vertices (self,
		self->vertices.array, self->vertices.count);
	self->shapes.concave = private_create_concave_from_vertices (self,
		self->indices.array, self->indices.count,
		self->vertices.array, self->vertices.count);
	self->shapes.convex = private_create_convex_from_vertices (self,
		self->vertices.array, self->vertices.count);

	return 1;
}

/** @} */
/** @} */
