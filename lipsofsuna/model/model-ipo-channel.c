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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlIpoChannel Ipo Channel
 * @{
 */

#include <system/lips-system.h>
#include "model-ipo.h"

#define LIMDL_BEZIER_SOLVE_ITERATIONS 10

static inline void
li_bezier_subdivide_4 (limatVector* self,
                       limatVector* p0,
                       limatVector* p1,
                       limatVector* p2,
                       limatVector* p3,
                       float        u);

static float
private_solve_const_chan (const limdlIpoChannel* self,
                          float                  time);
static float
private_solve_linear_chan (const limdlIpoChannel* self,
                           float                  time);
static float
private_solve_bezier_chan (const limdlIpoChannel* self,
                           float                  time);
static float
private_solve_const (const float* curve,
                     float        time);
static float
private_solve_linear (const float* curve,
                      float        time);
static float
private_solve_bezier (const float* curve,
                      float        time);

/*****************************************************************************/

float
limdl_ipo_channel_get_duration (const limdlIpoChannel* self)
{
	if (self->length == 0)
		return 0.0f;
	switch (self->type)
	{
		case LIMDL_IPO_TYPE_CONSTANT:
			return self->nodes[self->length - 2];
		case LIMDL_IPO_TYPE_LINEAR:
			return self->nodes[self->length - 2];
		case LIMDL_IPO_TYPE_BEZIER:
			return self->nodes[self->length - 4];
		default:
			assert (0);
			return 0.0f;
	}
}

float
limdl_ipo_channel_get_value (const limdlIpoChannel* self,
                             float                  time)
{
	if (self->length == 0)
	{
		/* FIXME */
//		if (channel == LIMDL_IPO_QUATT)
//			return 1.0f;
//		else
			return 0.0f;
	}

	switch (self->type)
	{
		case LIMDL_IPO_TYPE_CONSTANT:
			return private_solve_const_chan (self, time);
		case LIMDL_IPO_TYPE_LINEAR:
			return private_solve_linear_chan (self, time);
		case LIMDL_IPO_TYPE_BEZIER:
			return private_solve_bezier_chan (self, time);
		default:
			assert (0);
			return 0.0f;
	}
}

/*****************************************************************************/

/* FIXME: Create math-bezier.h or something? */
static inline void li_bezier_subdivide_4 (limatVector* self,
                                          limatVector* p0,
                                          limatVector* p1,
                                          limatVector* p2,
                                          limatVector* p3,
                                          float     u)
{
	float s0 = pow (u, 3);
	float s1 = pow (u, 2) * 3 * (1 - u);
	float s2 = pow (1 - u, 2) * 3 * u;
	float s3 = pow (1 - u, 3);

	self->x = s0 * p0->x + s1 * p1->x + s2 * p2->x + s3 * p3->x;
	self->y = s0 * p0->y + s1 * p1->y + s2 * p2->y + s3 * p3->y;
	self->z = s0 * p0->z + s1 * p1->z + s2 * p2->z + s3 * p3->z;
}

static float
private_solve_const_chan (const limdlIpoChannel* self,
                          float                  time)
{
	int i;
	float prev;
	float next;

	/* Clamp to range. */
	if (time <= self->nodes[0])
		return self->nodes[1];
	if (time >= self->nodes[self->length - 2])
		return self->nodes[self->length - 1];

	/* Find the segment of interest. */
	for (i = 0 ; i < self->length ; i += 2)
	{
		prev = self->nodes[i + 0];
		next = self->nodes[i + 2];
		if (prev <= time && time <= next)
			break;
	}
	assert (i < self->length);

	/* Solve the point from the constant curve. */
	return private_solve_const (self->nodes + i, time);
}

static float
private_solve_linear_chan (const limdlIpoChannel* self,
                           float                  time)
{
	int i;
	float prev;
	float next;

	/* Clamp to range. */
	if (time <= self->nodes[0])
		return self->nodes[1];
	if (time >= self->nodes[self->length - 2])
		return self->nodes[self->length - 1];

	/* Find the segment of interest. */
	for (i = 0 ; i < self->length ; i += 2)
	{
		prev = self->nodes[i + 0];
		next = self->nodes[i + 2];
		if (prev <= time && time <= next)
			break;
	}
	assert (i < self->length);

	/* Solve the point from the linear curve. */
	return private_solve_linear (self->nodes + i, time);
}

static float
private_solve_bezier_chan (const limdlIpoChannel* self,
                           float                  time)
{
	int i;
	float prev;
	float next;

	/* Clamp to range. */
	if (time <= self->nodes[2])
		return self->nodes[3];
	if (time >= self->nodes[self->length - 4])
		return self->nodes[self->length - 3];

	/* Find the curve of interest. */
	for (i = 2 ; i < self->length ; i += 6)
	{
		prev = self->nodes[i + 0];
		next = self->nodes[i + 6];
		if (prev <= time && time <= next)
			break;
	}
	assert (i < self->length);

	/* Solve the point from the bezier curve. */
	return private_solve_bezier (self->nodes + i, time);
}

static float
private_solve_const (const float* curve,
                     float        time)
{
	(void) time;
	return curve[1];
}

static float
private_solve_linear (const float* curve,
                      float        time)
{
	float t;
	float min;
	float max;

	min = curve[0];
	max = curve[2];
	t = (time - min) / (max - min);
	return curve[1] * (1.0f - t) +
	       curve[3] * t;
}

static float
private_solve_bezier (const float* curve,
                      float        time)
{
	int i;
	float t;
	float l;
/*	float min;
	float max;*/
	limatVector p[5];

	l = 0.25;
	t = 0.5f;
	p[0] = limat_vector_init (curve[0], curve[1], 0.0f);
	p[1] = limat_vector_init (curve[2], curve[3], 0.0f);
	p[2] = limat_vector_init (curve[4], curve[5], 0.0f);
	p[3] = limat_vector_init (curve[6], curve[7], 0.0f);
	for (i = 0 ; i < LIMDL_BEZIER_SOLVE_ITERATIONS ; i++)
	{
		li_bezier_subdivide_4 (p + 4, p + 0, p + 1, p + 2, p + 3, t);
		if (p[4].x < time)
			t -= l;
		else
			t += l;
		l *= 0.5;
	}

	return p[4].y;

#if 0
	/* FIXME: This approximation is coarse. */
	min = curve[0];
	max = curve[6];
	if (max == min)
		return curve[1];
	t = 1.0f - (time - min) / (max - min);
	p[0] = limat_vector_init (curve[0], curve[1], 0.0f);
	p[1] = limat_vector_init (curve[2], curve[3], 0.0f);
	p[2] = limat_vector_init (curve[4], curve[5], 0.0f);
	p[3] = limat_vector_init (curve[6], curve[7], 0.0f);
	li_bezier_subdivide_4 (p + 4, p + 0, p + 1, p + 2, p + 3, t);
	return p[4].y;
#endif
}

/** @} */
/** @} */
