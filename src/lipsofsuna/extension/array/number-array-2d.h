/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_ARRAY__NUMBER_ARRAY_2D_H__
#define __EXT_ARRAY__NUMBER_ARRAY_2D_H__

#include "lipsofsuna/archive.h"

typedef struct _LIExtNumberArray2d LIExtNumberArray2d;
struct _LIExtNumberArray2d
{
	int width;
	int height;
	float values[1];
};

LIAPICALL (int, liext_number_array_2d_get_data, (
	const LIExtNumberArray2d* self,
	LIArcWriter*              writer));

LIAPICALL (int, liext_number_array_2d_set_data, (
	LIExtNumberArray2d* self,
	LIArcReader*        reader));

LIAPICALL (void, liext_number_array_2d_get_gradient, (
	const LIExtNumberArray2d* self,
	int                       x,
	int                       y,
	float*                    result_x,
	float*                    result_y));

#endif
