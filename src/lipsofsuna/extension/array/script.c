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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtArray Array
 * @{
 */

#include "module.h"

typedef struct _LIExtNumberArray2d LIExtNumberArray2d;
struct _LIExtNumberArray2d
{
	int width;
	int height;
	float values[1];
};

/*****************************************************************************/

static void NumberArray2d_new (LIScrArgs* args)
{
	int width;
	int height;
	LIExtNumberArray2d* array;
	LIScrData* data;

	/* Get the size. */
	if (!liscr_args_geti_int (args, 0, &width) || width <= 0)
		return;
	if (!liscr_args_geti_int (args, 1, &height) || height <= 0)
		return;

	/* Check for size overflow. */
	if (height >= 0x8FFFFFFF / width / sizeof (float))
		return;

	/* Allocate the array. */
	array = lisys_calloc (1, sizeof (LIExtNumberArray2d) + width * height * sizeof (float));
	if (array == NULL)
		return;
	array->width = width;
	array->height = height;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, array, LIEXT_SCRIPT_NUMBER_ARRAY_2D, lisys_free);
	if (data == NULL)
	{
		lisys_free (array);
		return;
	}
	liscr_args_seti_stack (args);
}

static void NumberArray2d_get (LIScrArgs* args)
{
	int x;
	int y;
	LIExtNumberArray2d* self;

	self = args->self;
	if (!liscr_args_geti_int (args, 0, &x) || x < 0 || x >= self->width)
		return;
	if (!liscr_args_geti_int (args, 1, &y) || y < 0 || y >= self->height)
		return;

	liscr_args_seti_float (args, self->values[x + y * self->width]);
}

static void NumberArray2d_get_bilinear (LIScrArgs* args)
{
	int x;
	int y;
	float b;
	float fx;
	float fy;
	float v[4];
	LIExtNumberArray2d* self;

	self = args->self;
	if (!liscr_args_geti_float (args, 0, &fx))
		return;
	if (!liscr_args_geti_float (args, 1, &fy))
		return;
	fx = LIMAT_CLAMP (fx, 0.0f, self->width - 1.0f);
	fy = LIMAT_CLAMP (fy, 0.0f, self->height - 1.0f);
	x = (int) fx;
	y = (int) fy;

	if (x == self->width - 1 && y == self->height - 1)
	{
		liscr_args_seti_float (args, self->values[x + y * self->width]);
	}
	else if (x == self->width - 1)
	{
		b = fy - y;
		v[0] = self->values[x + y * self->width];
		v[1] = self->values[x + (y+1) * self->width];
		liscr_args_seti_float (args, (1.0f - b) * v[0] + b * v[1]);
	}
	else if (y == self->height - 1)
	{
		b = fx - x;
		v[0] = self->values[x + y * self->width];
		v[1] = self->values[(x+1) + y * self->width];
		liscr_args_seti_float (args, (1.0f - b) * v[0] + b * v[1]);
	}
	else
	{
		b = fx - x;
		v[0] = self->values[x + y * self->width];
		v[1] = self->values[(x+1) + y * self->width];
		v[2] = self->values[x + (y+1) * self->width];
		v[3] = self->values[(x+1) + (y+1) * self->width];
		v[0] = (1.0f - b) * v[0] + b * v[1];
		v[1] = (1.0f - b) * v[2] + b * v[3];
		b = fy - y;
		liscr_args_seti_float (args, (1.0f - b) * v[0] + b * v[1]);
	}
}

static void NumberArray2d_set (LIScrArgs* args)
{
	int x;
	int y;
	float value;
	LIExtNumberArray2d* self;

	self = args->self;
	if (!liscr_args_geti_int (args, 0, &x) || x < 0 || x >= self->width)
		return;
	if (!liscr_args_geti_int (args, 1, &y) || y < 0 || y >= self->height)
		return;
	if (!liscr_args_geti_float (args, 2, &value))
		return;

	self->values[x + y * self->width] = value;
}

/*****************************************************************************/

void liext_script_number_array_2d (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_new", NumberArray2d_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_get", NumberArray2d_get);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_get_bilinear", NumberArray2d_get_bilinear);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_set", NumberArray2d_set);
}

/** @} */
/** @} */
