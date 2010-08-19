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
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhySector Sector
 * @{
 */

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <lipsofsuna/system.h>
#include "physics-shape.h"
#include "physics-private.h"

#define VERTEX_WELD_EPSILON 0.05

static int private_add_shape (
	LIPhyShape*           self,
	btConvexShape*        shape,
	const LIMatTransform* transform);

static btConvexHullShape* private_create_convex (
	LIPhyShape* self,
	btScalar*   vertices,
	int         count);

static btConvexHullShape* private_create_from_box (
	LIPhyShape*      self,
	const LIMatAabb* aabb);

static btConvexHullShape* private_create_from_empty (
	LIPhyShape* self);

static btConvexHullShape* private_create_from_model (
	LIPhyShape*       self,
	const LIMdlModel* model);

static btConvexHullShape* private_create_from_vertices (
	LIPhyShape*        self,
	const LIMatVector* vertices,
	int                count);

/*****************************************************************************/

/**
 * \brief Creates an empty collision shape.
 * \param physics Physics engine.
 * \return New collision shape or NULL.
 */
LIPhyShape* liphy_shape_new (
	LIPhyPhysics* physics)
{
	LIPhyShape* self;

	self = (LIPhyShape*) lisys_calloc (1, sizeof (LIPhyShape));
	if (self == NULL)
		return NULL;
	try
	{
		self->physics = physics;
		self->shape = new btCompoundShape ();
		self->shape->setUserPointer (self);
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
 * \brief Frees or unreferences the collision shape.
 *
 * \param self Collision shape.
 */
void liphy_shape_free (
	LIPhyShape* self)
{
	delete self->shape;
	lisys_free (self);
}

int liphy_shape_add_aabb (
	LIPhyShape*           self,
	const LIMatAabb*      aabb,
	const LIMatTransform* transform)
{
	btConvexShape* shape;

	shape = private_create_from_box (self, aabb);
	if (shape == NULL)
	{
		liphy_shape_free (self);
		return 0;
	}
	private_add_shape (self, shape, transform);

	return 1;
}

int liphy_shape_add_convex (
	LIPhyShape*           self,
	const LIMatVector*    vertices,
	int                   count,
	const LIMatTransform* transform)
{
	btConvexShape* shape;

	shape = private_create_from_vertices (self, vertices, count);
	if (shape == NULL)
	{
		liphy_shape_free (self);
		return 0;
	}
	private_add_shape (self, shape, transform);

	return 1;
}

int liphy_shape_add_model (
	LIPhyShape*           self,
	const LIMdlModel*     model,
	const LIMatTransform* transform)
{
	btConvexShape* shape;

	shape = private_create_from_model (self, model);
	if (shape == NULL)
	{
		liphy_shape_free (self);
		return 0;
	}
	private_add_shape (self, shape, transform);

	return 1;
}

/**
 * \brief Merges a collision shape to this one.
 * \param self Object.
 * \param shape Collision shape.
 * \param transform Shape transformation or NULL for identity.
 */
int liphy_shape_add_shape (
	LIPhyShape*           self,
	LIPhyShape*           shape,
	const LIMatTransform* transform)
{
	btConvexHullShape* child;

	while (shape->shape->getNumChildShapes ())
	{
		child = (btConvexHullShape*) shape->shape->getChildShape (0);
		child = new btConvexHullShape (*child);
		// FIXME: Take child transformation into account.
		private_add_shape (self, child, transform);
	}

	return 1;
}

/**
 * \brief Clears the shape.
 * \param self Shape.
 */
void liphy_shape_clear (
	LIPhyShape* self)
{
	while (self->shape->getNumChildShapes ())
		self->shape->removeChildShapeByIndex (0);
}

/**
 * \brief Gets the local inertia vector of the shape.
 *
 * \param self Collision shape.
 * \param mass Mass.
 * \param result Return location for the inertia vector.
 */
void
liphy_shape_get_inertia (const LIPhyShape* self,
                         float             mass,
                         LIMatVector*      result)
{
	btVector3 inertia;

	/* TODO: Could this be precalculated? */
	self->shape->calculateLocalInertia (mass, inertia);
	result->x = inertia[0];
	result->y = inertia[1];
	result->z = inertia[2];
}

int liphy_shape_set_model (
	LIPhyShape*       self,
	const LIMdlModel* model)
{
	btConvexShape* shape;

	shape = private_create_from_model (self, model);
	if (shape == NULL)
		return 0;
	liphy_shape_clear (self);
	private_add_shape (self, shape, NULL);

	return 1;
}

/*****************************************************************************/

static int private_add_shape (
	LIPhyShape*           self,
	btConvexShape*        shape,
	const LIMatTransform* transform)
{
	btVector3 p(0.0f, 0.0f, 0.0f);
	btQuaternion r(0.0f, 0.0f, 0.0f, 1.0f);

	if (transform != NULL)
	{
		p = btVector3 (transform->position.x, transform->position.y,
		               transform->position.z);
		r = btQuaternion(transform->rotation.x, transform->rotation.y,
		                 transform->rotation.z, transform->rotation.w);
	}
	self->shape->addChildShape (btTransform (r, p), shape);

	return 1;
}

static btConvexHullShape* private_create_convex (
	LIPhyShape* self,
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

static btConvexHullShape* private_create_from_box (
	LIPhyShape*      self,
	const LIMatAabb* aabb)
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

	return new btConvexHullShape (tmp, 8, 4 * sizeof (btScalar));
}

static btConvexHullShape* private_create_from_empty (
	LIPhyShape* self)
{
	const float w = 0.1f;
	const btScalar tmp[32] =
	{
		-w, -w, -w, 0.0,
		-w, -w, w, 0.0,
		-w, w, -w, 0.0,
		-w, w, w, 0.0,
		w, -w, -w, 0.0,
		w, -w, w, 0.0,
		w, w, -w, 0.0,
		w, w, w, 0.0
	};

	return new btConvexHullShape (tmp, 8, 4 * sizeof (btScalar));
}

static btConvexHullShape* private_create_from_model (
	LIPhyShape*       self,
	const LIMdlModel* model)
{
	int i;
	btConvexHullShape* ret;
	LIMatVector* tmp;

	/* Use precalculated convex shapes when possible. */
	/* FIXME: One model can have multiple shapes. */
	if (model->shapes.count)
	{
		if (!model->shapes.array[0].vertices.count)
			return private_create_from_empty (self);
		return private_create_from_vertices (self,
			model->shapes.array[0].vertices.array,
			model->shapes.array[0].vertices.count);
	}

	/* Extract coordinates from vertices. */
	if (model->vertices.count)
	{
		tmp = (LIMatVector*) lisys_calloc (model->vertices.count, sizeof (LIMatVector));
		if (tmp == NULL)
			return NULL;
		for (i = 0 ; i < model->vertices.count ; i++)
			tmp[i] = model->vertices.array[i].coord;
	}
	else
		tmp = NULL;

	/* Create the shape from the vertex coordinates. */
	ret = private_create_from_vertices (self, tmp, model->vertices.count);
	lisys_free (tmp);

	return ret;
}

static btConvexHullShape* private_create_from_vertices (
	LIPhyShape*        self,
	const LIMatVector* vertices,
	int                count)
{
	int i;
	int j;
	int num;
	btConvexHullShape* ret;
	btScalar* tmp;
	LIMatVector vertex;

	/* Use a default empty model if there are no vertices. */
	if (!count)
		return private_create_from_empty (self);

	/* Remove duplicate vertices from the model. */
	num = 0;
	tmp = (btScalar*) lisys_calloc (4 * count, sizeof (btScalar));
	if (tmp == NULL)
		return NULL;
	for (i = 0 ; i < count ; i++)
	{
		vertex = vertices[i];
		for (j = 0 ; j < num ; j++)
		{
			if (LIMAT_ABS (vertex.x - tmp[4 * j + 0]) < VERTEX_WELD_EPSILON &&
			    LIMAT_ABS (vertex.y - tmp[4 * j + 1]) < VERTEX_WELD_EPSILON &&
			    LIMAT_ABS (vertex.z - tmp[4 * j + 2]) < VERTEX_WELD_EPSILON)
				break;
		}
		if (j == num)
		{
			tmp[4 * num + 0] = vertex.x;
			tmp[4 * num + 1] = vertex.y;
			tmp[4 * num + 2] = vertex.z;
			tmp[4 * num + 3] = 0.0;
			num++;
		}
	}

	/* Create a convex shape. */
	ret = private_create_convex (self, tmp, num);
	lisys_free (tmp);

	return ret;
}

/** @} */
/** @} */
