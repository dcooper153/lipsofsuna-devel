/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlVertex Vertex
 * @{
 */

#ifndef __MODEL_VERTEX_H__
#define __MODEL_VERTEX_H__

#include <math/lips-math.h>

typedef struct _limdlVertex limdlVertex;
struct _limdlVertex
{
	float texcoord[6];
	limatVector normal;
	limatVector coord;
};

static inline void
limdl_vertex_init (limdlVertex*    self,
                   const limatVector* coord,
                   const limatVector* normal,
                   float           u,
                   float           v)
{
	self->texcoord[0] = u;
	self->texcoord[1] = v;
	self->texcoord[2] = 0.0f;
	self->texcoord[3] = 0.0f;
	self->texcoord[4] = 0.0f;
	self->texcoord[5] = 0.0f;
	self->normal = *normal;
	self->coord = *coord;
}

static inline int
limdl_vertex_compare (const limdlVertex* self,
                      const limdlVertex* vertex)
{
	if (self->texcoord[0] < vertex->texcoord[0]) return -1;
	if (self->texcoord[0] > vertex->texcoord[0]) return 1;
	if (self->texcoord[1] < vertex->texcoord[1]) return -1;
	if (self->texcoord[1] > vertex->texcoord[1]) return 1;
	if (self->texcoord[2] < vertex->texcoord[2]) return -1;
	if (self->texcoord[2] > vertex->texcoord[2]) return 1;
	if (self->texcoord[3] < vertex->texcoord[3]) return -1;
	if (self->texcoord[3] > vertex->texcoord[3]) return 1;
	if (self->texcoord[4] < vertex->texcoord[4]) return -1;
	if (self->texcoord[4] > vertex->texcoord[4]) return 1;
	if (self->texcoord[5] < vertex->texcoord[5]) return -1;
	if (self->texcoord[5] > vertex->texcoord[5]) return 1;
	if (self->normal.x < vertex->normal.x) return -1;
	if (self->normal.x > vertex->normal.x) return 1;
	if (self->normal.y < vertex->normal.y) return -1;
	if (self->normal.y > vertex->normal.y) return 1;
	if (self->normal.z < vertex->normal.z) return -1;
	if (self->normal.z > vertex->normal.z) return 1;
	if (self->coord.x < vertex->coord.x) return -1;
	if (self->coord.x > vertex->coord.x) return 1;
	if (self->coord.y < vertex->coord.y) return -1;
	if (self->coord.y > vertex->coord.y) return 1;
	if (self->coord.z < vertex->coord.z) return -1;
	if (self->coord.z > vertex->coord.z) return 1;
	return 0;
}

#endif

/** @} */
/** @} */
