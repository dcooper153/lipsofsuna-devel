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
 * \addtogroup limdl Model
 * @{
 * \addtogroup LIMdlVertex Vertex
 * @{
 */

#ifndef __MODEL_VERTEX_H__
#define __MODEL_VERTEX_H__

#include <string.h>
#include <lipsofsuna/math.h>

typedef struct _LIMdlVertex LIMdlVertex;
struct _LIMdlVertex
{
	float texcoord[6];
	LIMatVector normal;
	LIMatVector coord;
};

static inline void
limdl_vertex_init (LIMdlVertex*       self,
                   const LIMatVector* coord,
                   const LIMatVector* normal,
                   float              u,
                   float              v)
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
limdl_vertex_compare (const LIMdlVertex* self,
                      const LIMdlVertex* vertex)
{
	return memcmp (self, vertex, sizeof (LIMdlVertex));
}

#endif

/** @} */
/** @} */
