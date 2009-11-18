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

#define VERTEX_WELD_EPSILON 0.05

static btConvexHullShape*
private_create_convex (liphyShape* self,
                       btScalar*   vertices,
                       int         count);

static inline int
private_init_box (liphyShape*      self,
                  const limatAabb* aabb);

static inline int
private_init_convex (liphyShape*        self,
                     const limatVector* vertices,
                     int                count);

static inline int
private_init_model (liphyShape*       self,
                    const limdlModel* model);

static btScalar*
private_weld_vertices (liphyShape*        self,
                       const limatVector* vertices,
                       int                count_in,
                       int*               count_out);

/*****************************************************************************/

/**
 * \brief Creates a collision shape from a model.
 *
 * The model is stored to the shape as a reference and must not be freed
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
	if (self->shape != NULL)
		delete self->shape;
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
	self->shape->calculateLocalInertia (mass, inertia);
	result->x = inertia[0];
	result->y = inertia[1];
	result->z = inertia[2];
}

/*****************************************************************************/

static btConvexHullShape*
private_create_convex (liphyShape* self,
                       btScalar*   vertices,
                       int         count)
{
	btShapeHull* hull;
	btConvexHullShape* shape;
	btConvexHullShape* shape1;

	shape1 = new btConvexHullShape (vertices, count, 4 * sizeof (btScalar));
	try
	{
		if (count > 42)
		{
			hull = new btShapeHull (shape1);
			hull->buildHull (shape1->getMargin ());
			shape = new btConvexHullShape ((btScalar*) hull->getVertexPointer (), hull->numVertices ());
			delete shape1;
			delete hull;
			return shape;
		}
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
	btScalar tmp[32] =
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

	self->shape = new btConvexHullShape (tmp, 8, 4 * sizeof (btScalar));

	return 1;
}

static inline int
private_init_convex (liphyShape*        self,
                     const limatVector* vertices,
                     int                count)
{
	int num;
	btScalar* tmp;

	tmp = private_weld_vertices (self, vertices, count, &num);
	if (tmp == NULL)
		return 0;
	self->shape = private_create_convex (self, tmp, num);
	lisys_free (tmp);

	return 1;
}

static inline int
private_init_model (liphyShape*       self,
                    const limdlModel* model)
{
	int i;
	int j;
	int k;
	int ret;
	int count;
	limdlFaces* group;
	limatVector* vertices;

	/* FIXME: One model can have multiple shapes. */
	/* FIXME: All shapes should be precalculated. */
	if (model->shapes.count)
	{
		/* Create predefined shape. */
		ret = private_init_convex (self,
			model->shapes.array[0].vertices.array,
			model->shapes.array[0].vertices.count);
	}
	else
	{
		/* Count vertices. */
		count = limdl_model_get_index_count (model);
		if (!count)
			return 1;

		/* Allocate vertices. */
		vertices = (limatVector*) lisys_calloc (count, sizeof (limatVector));
		if (vertices == NULL)
			return 0;
		for (i = j = 0 ; j < model->facegroups.count ; j++)
		{
			group = model->facegroups.array + j;
			for (k = 0 ; k < group->vertices.count ; k++)
				vertices[i++] = group->vertices.array[k].coord;
		}

		/* Create shape. */
		ret = private_init_convex (self, vertices, count);
		lisys_free (vertices);
	}

	return ret;
}

static btScalar*
private_weld_vertices (liphyShape*        self,
                       const limatVector* vertices,
                       int                count_in,
                       int*               count_out)
{
	int i;
	int j;
	int num;
	btScalar* ret;

	num = 0;
	ret = (btScalar*) lisys_calloc (4 * count_in, sizeof (btScalar));
	if (ret == NULL)
		return NULL;
	for (i = 0 ; i < count_in ; i++)
	{
		for (j = 0 ; j < num ; j++)
		{
			if (LI_ABS (vertices[i].x - ret[4 * j + 0]) < VERTEX_WELD_EPSILON &&
			    LI_ABS (vertices[i].y - ret[4 * j + 1]) < VERTEX_WELD_EPSILON &&
			    LI_ABS (vertices[i].z - ret[4 * j + 2]) < VERTEX_WELD_EPSILON)
				break;
		}
		if (j == num)
		{
			ret[4 * num + 0] = vertices[i].x;
			ret[4 * num + 1] = vertices[i].y;
			ret[4 * num + 2] = vertices[i].z;
			ret[4 * num + 3] = 0.0;
			num++;
		}
	}
	*count_out = num;

	return ret;
}

/** @} */
/** @} */
