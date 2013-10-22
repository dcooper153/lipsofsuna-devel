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
#include "number-array-2d.h"

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

static void NumberArray2d_get_bilinear (LIScrArgs* args)
{
	float fx;
	float fy;
	LIExtNumberArray2d* self;

	self = args->self;
	if (!liscr_args_geti_float (args, 0, &fx))
		return;
	if (!liscr_args_geti_float (args, 1, &fy))
		return;

	liscr_args_seti_float (args, liext_number_array_2d_get_bilinear (self, fx, fy));
}

static void NumberArray2d_get_data (LIScrArgs* args)
{
	LIArcPacket* packet;
	LIScrData* data;

	/* Get the packet writer. */
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);
	if (packet->writer == NULL)
		return;

	/* Read the data. */
	liscr_args_seti_bool (args, liext_number_array_2d_get_data (args->self, packet->writer));
}

static void NumberArray2d_set_data (LIScrArgs* args)
{
	LIArcPacket* packet;
	LIArcReader* reader;
	LIScrData* data;

	/* Get the packet reader. */
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_PACKET, &data))
		return;
	packet = liscr_data_get_data (data);

	/* Write the data. */
	if (packet->reader != NULL)
		liscr_args_seti_bool (args, liext_number_array_2d_set_data (args->self, packet->reader));
	else
	{
		reader = liarc_reader_new (
			liarc_writer_get_buffer (packet->writer),
			liarc_writer_get_length (packet->writer));
		if (reader == NULL)
			return;
		reader->pos = 1; /* FIXME! */
		liscr_args_seti_bool (args, liext_number_array_2d_set_data (args->self, reader));
		liarc_reader_free (reader);
	}
}

static void NumberArray2d_get_gradient (LIScrArgs* args)
{
	int x;
	int y;
	float gx;
	float gy;
	LIExtNumberArray2d* self;

	self = args->self;
	if (!liscr_args_geti_int (args, 0, &x) || x < 0 || x >= self->width)
		return;
	if (!liscr_args_geti_int (args, 1, &y) || y < 0 || y >= self->height)
		return;

	liext_number_array_2d_get_gradient (args->self, x, y, &gx, &gy);
	liscr_args_seti_float (args, gx);
	liscr_args_seti_float (args, gy);
	liscr_args_seti_float (args, sqrtf (gx * gx + gy * gy));
	liscr_args_seti_float (args, atan2f (gx, gy));
}

static void NumberArray2d_get_gradient_bilinear (LIScrArgs* args)
{
	float fx;
	float fy;
	float gx;
	float gy;

	if (!liscr_args_geti_float (args, 0, &fx))
		return;
	if (!liscr_args_geti_float (args, 1, &fy))
		return;

	liext_number_array_2d_get_gradient_bilinear (args->self, fx, fy, &gx, &gy);
	liscr_args_seti_float (args, gx);
	liscr_args_seti_float (args, gy);
	liscr_args_seti_float (args, sqrtf (gx * gx + gy * gy));
	liscr_args_seti_float (args, atan2f (gx, gy));
}

/*****************************************************************************/

void liext_script_number_array_2d (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_new", NumberArray2d_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_get", NumberArray2d_get);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_set", NumberArray2d_set);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_get_bilinear", NumberArray2d_get_bilinear);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_get_data", NumberArray2d_get_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_set_data", NumberArray2d_set_data);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_get_gradient", NumberArray2d_get_gradient);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_NUMBER_ARRAY_2D, "number_array_2d_get_gradient_bilinear", NumberArray2d_get_gradient_bilinear);
}

/** @} */
/** @} */
