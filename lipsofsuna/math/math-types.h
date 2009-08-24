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

#ifndef __MATH_TYPES_H__
#define __MATH_TYPES_H__

#include "math-vector.h"

/**
 * \brief A plane defined by a point and a normal.
 */
typedef struct _liPlane liPlane;
struct _liPlane
{
	float x;
	float y;
	float z;
	float w;
};

typedef struct _liTriangle liTriangle;
struct _liTriangle
{
	liPlane plane;
	limatVector vertices[3];
};

static inline void
li_triangle_set_from_points (liTriangle*     self,
                             const limatVector* vertex0,
                             const limatVector* vertex1,
                             const limatVector* vertex2);

#endif
