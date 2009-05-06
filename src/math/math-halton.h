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
 * \addtogroup limat Math
 * @{
 * \addtogroup limatHalton Halton
 * @{
 */

#ifndef __MATH_HALTON_H__
#define __MATH_HALTON_H__

typedef struct _limatHalton limatHalton;
struct _limatHalton
{
	int indices[2];
	float values[2];
};

static inline void
limat_halton_init (limatHalton* self);

static inline void
limat_halton_next (limatHalton* self);

static inline void
limat_halton_init (limatHalton* self)
{
	self->indices[0] = 0;
	self->indices[1] = 0;
	limat_halton_next (self);
}

static inline void
limat_halton_next (limatHalton* self)
{
	int i;
	int j;
	int tmp;
	int base;
	float val;
	float mult;
	const int bases[2] = { 2, 3 };

	for (j = 0 ; j < 2 ; j++)
	{
		val = 0.0f;
		base = bases[j];
		mult = 1.0f / base;
		for (i = self->indices[j] ; i ; i = (i - tmp) / base)
		{
			tmp = i % base;
			val += mult * tmp;
			mult /= base;
		}
		self->indices[j]++;
		self->values[j] = val;
	}
}

#define LI_FOREACH_HALTON(iter, index) \
	for (limat_halton_init (&(iter)) ; (iter).indices[0] <= (index) ; limat_halton_next (&(iter)))

#endif

/** @} */
/** @} */
