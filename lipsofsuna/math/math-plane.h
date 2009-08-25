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
 * \addtogroup limat Math
 * @{
 * \addtogroup limatPlane Plane
 * @{
 */

#ifndef __MATH_PLANE_H__
#define __MATH_PLANE_H__

#include "math-generic.h"
#include "math-types.h"
#include "math-vector.h"

static inline void
limat_plane_get_normal (const limatPlane* self,
                        limatVector*      vector)
{
	vector->x = self->x;
	vector->y = self->y;
	vector->z = self->z;
}

static inline void
limat_plane_get_point (const limatPlane* self,
                       limatVector*      vector)
{
	vector->x = self->x * self->w;
	vector->y = self->y * self->w;
	vector->z = self->z * self->w;
}

/**
 * \brief Initializes the plane variables from a plane equation.
 *
 * \param self Plane.
 * \param a X term of the plane normal.
 * \param b Y term of the plane normal.
 * \param c Z term of the plane normal.
 * \param d Signed distance from the plane to the origin.
 */
static inline void
limat_plane_init (limatPlane* self,
                  float       a,
                  float       b,
                  float       c,
                  float       d)
{
	self->x = a;
	self->y = b;
	self->z = c;
	self->w = d;
}

/**
 * \brief Initializes the plane variables from a point and normal.
 *
 * \param self Plane.
 * \param point Point on the plane.
 * \param normal Normal of the plane.
 */
static inline void
limat_plane_init_from_point (limatPlane*        self,
                             const limatVector* point,
                             const limatVector* normal)
{
	self->x = normal->x;
	self->y = normal->y;
	self->z = normal->z;
	self->w = point->x * normal->x + point->y * normal->y + point->z * normal->z;
}

/**
 * \brief Initializes the plane variables from a triangle.
 *
 * \param self Plane.
 * \param vertex0 Vertex.
 * \param vertex1 Vertex.
 * \param vertex2 Vertex.
 */
static inline void
limat_plane_init_from_points (limatPlane*        self,
                              const limatVector* vertex0,
                              const limatVector* vertex1,
                              const limatVector* vertex2)
{
	limatVector normal;

	normal = limat_vector_normalize (limat_vector_cross (
			limat_vector_subtract (*vertex1, *vertex0),
			limat_vector_subtract (*vertex2, *vertex0)));
	self->x = normal.x;
	self->y = normal.y;
	self->z = normal.z;
	self->w = limat_vector_dot (normal, *vertex0);
}

/**
 * \brief Gets the distance to the point.
 *
 * \param self Plane.
 * \param point Point.
 * \return Distance to the point.
 */
static inline float
limat_plane_distance_to_point (limatPlane*        self,
                               const limatVector* point)
{
	return LI_ABS (self->x * point->x + self->y * point->y + self->z * point->z - self->w);
}

/**
 * \brief Gets the signed distance to the point.
 *
 * \param self Plane.
 * \param point Point.
 * \return Signed distance to the point.
 */
static inline float
limat_plane_signed_distance_to_point (const limatPlane*  self,
                                      const limatVector* point)
{
	return self->x * point->x + self->y * point->y + self->z * point->z - self->w;
}

/**
 * \brief Gets the intersection point of a plane and a line.
 *
 * Lines coinciding with the plane are not considered to intersect.
 *
 * \param self Plane.
 * \param point0 First point of the line.
 * \param point1 Second point of the line.
 * \param point Return location for the intersection point.
 * \return Nonzero if the plane and the line intersect.
 */
static inline int
limat_plane_intersects_line (const limatPlane*  self,
                             const limatVector* point0,
                             const limatVector* point1,
                             limatVector*       point)
{
	float t;
	limatVector tmp;
	limatVector line;
	limatVector offs;

	/* Line vector and plane offset. */
	limat_plane_get_point (self, &tmp);
	line = limat_vector_subtract (*point1, *point0);
	offs = limat_vector_subtract (tmp, *point0);

	/* Check if parallel. */
	t = self->x * line.x + self->y * line.y + self->z * line.z;
	if (LI_ABS (t) < LI_MATH_EPSILON)
		return 0;

	/* Find the intersection. */
	t = (self->x * offs.x + self->y * offs.y + self->z * offs.z) / t;
	*point = limat_vector_add (*point0, limat_vector_multiply (line, t));
	return 1;
}

/**
 * \brief Gets the intersection point of a plane and a line segment.
 *
 * Line segments coinciding with the plane are not considered to intersect.
 *
 * \param self Plane.
 * \param point0 First point of the line segment.
 * \param point1 First point of the line segment.
 * \param point Return location for the intersection point.
 * \return Nonzero if the plane and the line segment intersect.
 */
static inline int
limat_plane_intersects_segment (const limatPlane*  self,
                                const limatVector* point0,
                                const limatVector* point1,
                                limatVector*       point)
{
	float t;
	limatVector tmp;
	limatVector line;
	limatVector offs;

	/* Line vector and plane offset. */
	limat_plane_get_point (self, &tmp);
	line = limat_vector_subtract (*point1, *point0);
	offs = limat_vector_subtract (tmp, *point0);

	/* Check if parallel. */
	t = self->x * line.x + self->y * line.y + self->z * line.z;
	if (LI_ABS (t) <= LI_MATH_EPSILON)
		return 0;

	/* Check if intersects. */
	t = (self->x * offs.x + self->y * offs.y + self->z * offs.z) / t;
	if (t < 0.0 || t > 1.0)
		return 0;

	/* Find the intersection. */
	*point = limat_vector_add (*point0, limat_vector_multiply (line, t));
	return 1;
}

/**
 * \brief Gets the intersection line segment of a plane and a triangle.
 *
 * Triangles coinciding with the plane are not considered to intersect.
 *
 * \param self Plane.
 * \param triangle Triangle.
 * \param result0 Return location for the first end point of the intersection.
 * \param result1 Return location for the second end point of the intersection.
 * \return Nonzero if the plane and the line segment intersect.
 */
static inline int
limat_plane_intersects_triangle (const limatPlane*    self,
                                 const limatTriangle* triangle,
                                 limatVector*         result0,
                                 limatVector*         result1)
{
	limatVector p[2];

	/* Find the two edges intersecting the plane. */
	if (!limat_plane_intersects_segment (self, triangle->vertices + 0, triangle->vertices + 1, p + 0))
	{
		if (!limat_plane_intersects_segment (self, triangle->vertices + 0, triangle->vertices + 2, p + 0) ||
		    !limat_plane_intersects_segment (self, triangle->vertices + 1, triangle->vertices + 2, p + 1))
			return 0;
	}
	else
	{
		if (!limat_plane_intersects_segment (self, triangle->vertices + 0, triangle->vertices + 2, p + 1) &&
		    !limat_plane_intersects_segment (self, triangle->vertices + 1, triangle->vertices + 2, p + 1))
			return 0;
	}

	/* The intersection is the line defined by the points. */
	*result0 = p[0];
	*result1 = p[1];
	return 1;
}

/**
 * \brief Subdivides triangles intersecting with the plane.
 *
 * Triangles coinciding with the plane are not considered to intersect.
 *
 * The normals of the created triangles are guaranteed to point to the
 * same direction with the original, and the new triangles also have the
 * same clockwise orientation with the original.
 *
 * One of the original vertices is guaranteed to be the first vertex of
 * any of the new triangles so you can use it to reliably detect to which
 * side of the plane the new triangles ended up.
 *
 * \param self Plane.
 * \param triangle Triangle.
 * \param results Return location for three triangle.
 * \return Nonzero if no intersection was found.
 */
static inline int
limat_plane_subdivide_triangle (const limatPlane*    self,
                                const limatTriangle* triangle,
                                limatTriangle*       results)
{
	limatVector p[2];
	const limatVector* v = triangle->vertices;

	/* Find the two edges intersecting the plane. */
	if (!limat_plane_intersects_segment (self, v + 0, v + 1, p + 0))
	{
		if (limat_plane_intersects_segment (self, v + 0, v + 2, p + 0) &&
		    limat_plane_intersects_segment (self, v + 1, v + 2, p + 1))
		{
			/*        v2
			 *        /\
			 *   p0 -+--+- p1
			 *      / `. \
			 *     /    `.\
			 * v0 +--------+ v1
			 */
			limat_triangle_set_from_points (results + 0, v + 2, p + 0, p + 1);
			limat_triangle_set_from_points (results + 1, v + 1, p + 1, p + 0);
			limat_triangle_set_from_points (results + 2, v + 0, v + 1, p + 0);
			return 1;
		}
	}
	else
	{
		if (limat_plane_intersects_segment (self, v + 0, v + 2, p + 1))
		{
			/*        v0
			 *        /\
			 *   p0 -+--+- p1
			 *      / `. \
			 *     /    `.\
			 * v1 +--------+ v2
			 */
			limat_triangle_set_from_points (results + 0, v + 0, p + 0, p + 1);
			limat_triangle_set_from_points (results + 1, v + 2, p + 1, p + 0);
			limat_triangle_set_from_points (results + 2, v + 2, p + 0, v + 1);
			return 1;
		}
		if (limat_plane_intersects_segment (self, v + 1, v + 2, p + 1))
		{
			/*        v1
			 *        /\
			 *   p0 -+--+- p1
			 *      / `. \
			 *     /    `.\
			 * v0 +--------+ v2
			 */
			limat_triangle_set_from_points (results + 0, v + 1, p + 1, p + 0);
			limat_triangle_set_from_points (results + 1, v + 2, p + 0, p + 1);
			limat_triangle_set_from_points (results + 2, v + 0, p + 1, v + 2);
			return 1;
		}
	}
	return 0;
}

#endif

/** @} */
/** @} */
