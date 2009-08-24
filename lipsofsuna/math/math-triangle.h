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
 * \addtogroup limatTriangle Triangle
 * @{
 */

#ifndef __MATH_TRIANGLE_H__
#define __MATH_TRIANGLE_H__

#include <stdio.h>
#include "math-plane.h"
#include "math-rectangle.h"
#include "math-types.h"
#include "math-vector.h"

/**
 * \brief Sets the triangle variables from vertices.
 *
 * \param self Triangle.
 * \param vertex0 Vertex.
 * \param vertex1 Vertex.
 * \param vertex2 Vertex.
 */
static inline void
li_triangle_set_from_points (liTriangle*     self,
                             const limatVector* vertex0,
                             const limatVector* vertex1,
                             const limatVector* vertex2)
{
	li_plane_init_from_points (&self->plane, vertex0, vertex1, vertex2);
	self->vertices[0] = *vertex0;
	self->vertices[1] = *vertex1;
	self->vertices[2] = *vertex2;
}

/**
 * \brief Checks if the triangle encloses the point.
 *
 * \param vertex0 Vertex of the triangle.
 * \param vertex1 Vertex of the triangle.
 * \param vertex2 Vertex of the triangle.
 * \param point Point.
 * \return Nonzero if enclosed.
 */
static inline int
li_triangle_intersects_point (const limatVector* vertex0,
                              const limatVector* vertex1,
                              const limatVector* vertex2,
                              const limatVector* point)
{
#if 0
#include <compat/lips-compat.h>
	limatVector e0 = limat_vector_subtract (*vertex1, *vertex0);
	limatVector e1 = limat_vector_subtract (*vertex2, *vertex0);
	limatVector v = limat_vector_subtract (*point, *vertex0);
	float a = limat_vector_dot (e0, e0);
	float b = limat_vector_dot (e0, e1);
	float c = limat_vector_dot (e1, e1);
	float d = limat_vector_dot (v, e0);
	float e = limat_vector_dot (v, e1);
	float x = d * c - e * b;
	float y = e * a - d * b;
	float z = x + y - a * c + b * b;
	return (((uint32_t) z) & (((uint32_t) x) | ((uint32_t) y))) & 0x80000000;
#endif
	limatVector p;
	limatVector a;
	limatVector b;

	/* First edge. */
	p = limat_vector_subtract (*point, *vertex0);
	a = limat_vector_subtract (*vertex1, *vertex0);
	b = limat_vector_subtract (*vertex2, *vertex0);
	if (limat_vector_dot (
		limat_vector_cross (b, a),
		limat_vector_cross (b, p)) < 0)
		return 0;

	/* Second edge. */
	p = limat_vector_subtract (*point, *vertex1);
	a = limat_vector_subtract (*vertex2, *vertex1);
	b = limat_vector_subtract (*vertex0, *vertex1);
	if (limat_vector_dot (
		limat_vector_cross (b, a),
		limat_vector_cross (b, p)) < 0)
		return 0;

	/* Third edge. */
	p = limat_vector_subtract (*point, *vertex2);
	a = limat_vector_subtract (*vertex0, *vertex2);
	b = limat_vector_subtract (*vertex1, *vertex2);
	if (limat_vector_dot (
		limat_vector_cross (b, a),
		limat_vector_cross (b, p)) < 0)
		return 0;

	return 1;
}

#if 0
/**
 * \brief Checks if the triangle intersects with a line segment on its plane.
 *
 * \param vertex0 A vertex of the triangle.
 * \param vertex1 A vertex of the triangle.
 * \param vertex2 A vertex of the triangle.
 * \param point0 An end point of the line segment.
 * \param point1 An end point of the line segment.
 * \return Nonzero if intersects.
 */
static inline int li_triangle_intersects_line_segment (const limatVector* vertex0,
                                                       const limatVector* vertex1,
                                                       const limatVector* vertex2,
                                                       const limatVector* point0,
                                                       const limatVector* point1)
{
}
#endif

/**
 * \brief Checks if the triangle intersects with another.
 *
 * \param self Triangle.
 * \param triangle Triangle.
 * \param point0 Return location for the intersection or NULL.
 * \param point1 Return location for the intersection or NULL.
 * \return Nonzero if intersects.
 */
static inline int
li_triangle_intersects_triangle (const liTriangle* self,
                                 const liTriangle* triangle,
                                 limatVector*         point0,
                                 limatVector*         point1)
{
	float t0;
	float t1;
	float dot0;
	float dot1;
	limatVector ref;
	limatVector i00;
	limatVector i01;
	limatVector i10;
	limatVector i11;

	/* Get two coinciding intersection line segments. */
	if (!li_plane_intersects_triangle (&self->plane, triangle, &i00, &i01) ||
	    !li_plane_intersects_triangle (&triangle->plane, self, &i10, &i11))
		return 0;

	/* Find the intersecting portion of the coinciding segments. */
	ref = limat_vector_subtract (i01, i00);
	dot0 = limat_vector_dot (ref, limat_vector_subtract (i10, i00));
	dot1 = limat_vector_dot (ref, limat_vector_subtract (i11, i00));
	t0 = LI_MAX (0.0f, LI_MIN (1.0f, dot0));
	t1 = LI_MIN (1.0f, LI_MAX (0.0f, dot1));
	if (t0 > t1)
		return 0;
	if (point0 != NULL)
		*point0 = limat_vector_add (i00, limat_vector_multiply (ref, t0));
	if (point1 != NULL)
		*point1 = limat_vector_add (i00, limat_vector_multiply (ref, t1));
	return 1;
}

/**
 * \brief Checks if the triangle intersects an axis-aligned bounding box.
 *
 * \param self Triangle.
 * \param origin Origin on the box.
 * \param size Side lengths of the box.
 * \return Nonzero if intersects.
 */
static inline int
li_triangle_intersects_aabb (const liTriangle* self,
                             const limatVector*   origin,
                             const limatVector*   size)
{
	float x0 = origin->x;
	float x1 = origin->x + size->x;
	float y0 = origin->y;
	float y1 = origin->y + size->y;
	float z0 = origin->z;
	float z1 = origin->z + size->z;
	limatVector l[2];
	liPlane p[6] =
	{
		{ 1, 0, 0, x0 }, /* Left. */
		{ 1, 0, 0, x1 }, /* Right. */
		{ 0, 1, 0, y0 }, /* Bottom. */
		{ 0, 1, 0, y1 }, /* Top. */
		{ 0, 0, 1, z0 }, /* Front. */
		{ 0, 0, 1, z1 }, /* Back. */
	};
	liRectangle r[3] =
	{
		{ y0, z0, size->y, size->z }, /* Left and right. */
		{ x0, z0, size->x, size->z }, /* Bottom and top. */
		{ x0, y0, size->x, size->y }, /* Front and back. */
	};

	/* Test if a vertex is inside. */
	if (x0 <= self->vertices[0].x && self->vertices[0].x <= x1 &&
	    y0 <= self->vertices[0].y && self->vertices[0].y <= y1 &&
	    z0 <= self->vertices[0].z && self->vertices[0].z <= z1)
		return 1;
	if (x0 <= self->vertices[1].x && self->vertices[1].x <= x1 &&
	    y0 <= self->vertices[1].y && self->vertices[1].y <= y1 &&
	    z0 <= self->vertices[1].z && self->vertices[1].z <= z1)
		return 1;
	if (x0 <= self->vertices[2].x && self->vertices[2].x <= x1 &&
	    y0 <= self->vertices[2].y && self->vertices[2].y <= y1 &&
	    z0 <= self->vertices[2].z && self->vertices[2].z <= z1)
		return 1;

	/* Left and right. */
	if (li_plane_intersects_triangle (p + 0, self, l + 0, l + 1) &&
	    li_rectangle_intersects_segment (r + 0, l[0].y, l[0].z, l[1].y, l[1].z))
		return 1;
	if (li_plane_intersects_triangle (p + 1, self, l + 0, l + 1) &&
	    li_rectangle_intersects_segment (r + 0, l[0].y, l[0].z, l[1].y, l[1].z))
		return 1;

	/* Bottom and top. */
	if (li_plane_intersects_triangle (p + 2, self, l + 0, l + 1) &&
	    li_rectangle_intersects_segment (r + 1, l[0].x, l[0].z, l[1].x, l[1].z))
		return 1;
	if (li_plane_intersects_triangle (p + 3, self, l + 0, l + 1) &&
	    li_rectangle_intersects_segment (r + 1, l[0].x, l[0].z, l[1].x, l[1].z))
		return 1;

	/* Front and back. */
	if (li_plane_intersects_triangle (p + 4, self, l + 0, l + 1) &&
	    li_rectangle_intersects_segment (r + 2, l[0].x, l[0].y, l[1].x, l[1].y))
		return 1;
	if (li_plane_intersects_triangle (p + 5, self, l + 0, l + 1) &&
	    li_rectangle_intersects_segment (r + 2, l[0].x, l[0].y, l[1].x, l[1].y))
		return 1;

	return 0;
}

#endif

/** @} */
/** @} */
