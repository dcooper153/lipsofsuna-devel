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
 * \addtogroup limatGeneric Generic
 * @{
 */

#ifndef __MATH_GENERIC_H__
#define __MATH_GENERIC_H__

#include <math.h>

#define LI_ABS(a) (((a)<0)?-(a):(a))
#define LI_MIN(a,b) ((a)<(b)?(a):(b))
#define LI_MAX(a,b) ((a)>(b)?(a):(b))
#define LI_CLAMP(val,min,max) ((val)<(min)?(min):(val)>(max)?(max):(val))
#define LI_MATH_EPSILON 1E-20f
#define LI_MATH_INFINITE (1.0f/0.0f)

static inline float
limat_quantize (float value,
                float resolution)
{
	float tmp;

	if (resolution <= LI_MATH_EPSILON)
		return value;
	tmp = fmod (value, resolution);
	if (tmp >= 0.5f * resolution)
		return value - tmp + resolution;
	else if (tmp <= -0.5f * resolution)
		return value - tmp - resolution;
	else
		return value - tmp;
}

#endif

/** @} */
/** @} */

