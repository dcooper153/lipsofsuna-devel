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
 */

#ifndef __MATH_VERTEX_H__
#define __MATH_VERTEX_H__

#include <stddef.h>
#include "math-vector.h"

/*
 * \addtogroup limatVtxops Vtxops
 * @{
 */

typedef struct _limatVtxops limatVtxops;
struct _limatVtxops
{
	size_t size;
	void (*interpolate)(const void* self, const void* other, float frac, void* result);
	void (*getcoord)(const void* self, limatVector* result);
	void (*setcoord)(void* self, const limatVector* value);
};

/** @} */

/*****************************************************************************/

/*
 * \addtogroup limatVertexV3 VertexV3
 * @{
 */

typedef struct _limatVertexV3 limatVertexV3;
struct _limatVertexV3
{
	limatVector coord;
};

static inline void
__limat_vtxops_v3_getcoord (const void*  self,
                            limatVector* result)
{
	const limatVertexV3* data = (limatVertexV3*) self;

	*result = data->coord;
}

static inline void
__limat_vtxops_v3_setcoord (void*              self,
                            const limatVector* value)
{
	limatVertexV3* data = (limatVertexV3*) self;

	data->coord = *value;
}

static inline void
__limat_vtxops_v3_interpolate (const void* self,
                               const void* other,
                               float       blend,
                               void*       result)
{
}

static const limatVtxops limat_vtxops_v3 =
{
	sizeof (limatVertexV3),
	__limat_vtxops_v3_interpolate,
	__limat_vtxops_v3_getcoord,
	__limat_vtxops_v3_setcoord
};

/** @} */

/*****************************************************************************/

/*
 * \addtogroup limatVertexT2V3N3 VertexT2V3N3
 * @{
 */

typedef struct _limatVertexT2N3V3 limatVertexT2N3V3;
struct _limatVertexT2N3V3
{
	float texcoord[2];
	limatVector normal;
	limatVector coord;
};

static inline void
__limat_vtxops_t2n3v3_getcoord (const void*  self,
                                limatVector* result)
{
	const limatVertexT2N3V3* data = (limatVertexT2N3V3*) self;

	*result = data->coord;
}

static inline void
__limat_vtxops_t2n3v3_setcoord (void*              self,
                                const limatVector* value)
{
	limatVertexT2N3V3* data = (limatVertexT2N3V3*) self;

	data->coord = *value;
}

static inline void
__limat_vtxops_t2n3v3_interpolate (const void* self,
                                   const void* other,
                                   float       blend,
                                   void*       result)
{
	const limatVertexT2N3V3* data0 = (limatVertexT2N3V3*) self;
	const limatVertexT2N3V3* data1 = (limatVertexT2N3V3*) other;
	limatVertexT2N3V3* data2 = (limatVertexT2N3V3*) result;

	data2->normal = limat_vector_normalize (limat_vector_add (
		limat_vector_multiply (data0->normal, blend),
		limat_vector_multiply (data1->normal, 1.0f - blend)));
	data2->texcoord[0] = data0->texcoord[0] * blend + data1->texcoord[0] * (1.0f - blend);
	data2->texcoord[1] = data0->texcoord[1] * blend + data1->texcoord[1] * (1.0f - blend);
}

static const limatVtxops limat_vtxops_t2n3v3 =
{
	sizeof (limatVertexT2N3V3),
	__limat_vtxops_t2n3v3_interpolate,
	__limat_vtxops_t2n3v3_getcoord,
	__limat_vtxops_t2n3v3_setcoord
};

/*****************************************************************************/

/*
 * \addtogroup limatVertexT6V3N3 VertexT6V3N3
 * @{
 */

typedef struct _limatVertexT6N3V3 limatVertexT6N3V3;
struct _limatVertexT6N3V3
{
	float texcoord[6];
	limatVector normal;
	limatVector coord;
};

static inline void
__limat_vtxops_t6n3v3_getcoord (const void*  self,
                                limatVector* result)
{
	const limatVertexT6N3V3* data = (limatVertexT6N3V3*) self;

	*result = data->coord;
}

static inline void
__limat_vtxops_t6n3v3_setcoord (void*              self,
                                const limatVector* value)
{
	limatVertexT6N3V3* data = (limatVertexT6N3V3*) self;

	data->coord = *value;
}

static inline void
__limat_vtxops_t6n3v3_interpolate (const void* self,
                                   const void* other,
                                   float       blend,
                                   void*       result)
{
	const limatVertexT6N3V3* data0 = (limatVertexT6N3V3*) self;
	const limatVertexT6N3V3* data1 = (limatVertexT6N3V3*) other;
	limatVertexT6N3V3* data2 = (limatVertexT6N3V3*) result;

	data2->normal = limat_vector_normalize (limat_vector_add (
		limat_vector_multiply (data0->normal, blend),
		limat_vector_multiply (data1->normal, 1.0f - blend)));
	data2->texcoord[0] = data0->texcoord[0] * blend + data1->texcoord[0] * (1.0f - blend);
	data2->texcoord[1] = data0->texcoord[1] * blend + data1->texcoord[1] * (1.0f - blend);
	data2->texcoord[2] = data0->texcoord[2] * blend + data1->texcoord[2] * (1.0f - blend);
	data2->texcoord[3] = data0->texcoord[3] * blend + data1->texcoord[3] * (1.0f - blend);
	data2->texcoord[4] = data0->texcoord[4] * blend + data1->texcoord[4] * (1.0f - blend);
	data2->texcoord[5] = data0->texcoord[5] * blend + data1->texcoord[5] * (1.0f - blend);
}

static const limatVtxops limat_vtxops_t6n3v3 =
{
	sizeof (limatVertexT6N3V3),
	__limat_vtxops_t6n3v3_interpolate,
	__limat_vtxops_t6n3v3_getcoord,
	__limat_vtxops_t6n3v3_setcoord
};

/** @} */

#endif

/** @} */
