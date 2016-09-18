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

#include "lipsofsuna/math.h"
#include "number-array-2d.h"

/**
 * \brief Adds the given value to each array element.
 * \param self Array.
 * \param value Value
 */
void liext_number_array_2d_add (
	LIExtNumberArray2d* self,
	float               value)
{
	int i;

	for (i = 0 ; i < self->width * self->height ; i++)
		self->values[i] += value;
}

/**
 * \brief Gets a bilinearly interpolated value.
 * \param self Array.
 * \param fx Floating point X coordinate.
 * \param fy Floating point Y coordinate.
 * \return Value.
 */
float liext_number_array_2d_get_bilinear (
	const LIExtNumberArray2d* self,
	float                     fx,
	float                     fy)
{
	int x;
	int y;
	float b;
	float v[4];

	fx = LIMAT_CLAMP (fx, 0.0f, self->width - 1.0f);
	fy = LIMAT_CLAMP (fy, 0.0f, self->height - 1.0f);
	x = (int) fx;
	y = (int) fy;

	if (x == self->width - 1 && y == self->height - 1)
	{
		return self->values[x + y * self->width];
	}
	else if (x == self->width - 1)
	{
		b = fy - y;
		v[0] = self->values[x + y * self->width];
		v[1] = self->values[x + (y+1) * self->width];
		return (1.0f - b) * v[0] + b * v[1];
	}
	else if (y == self->height - 1)
	{
		b = fx - x;
		v[0] = self->values[x + y * self->width];
		v[1] = self->values[(x+1) + y * self->width];
		return (1.0f - b) * v[0] + b * v[1];
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
		return (1.0f - b) * v[0] + b * v[1];
	}
}

/**
 * \brief Writes the raw data to an archive.
 * \param self Array.
 * \param writer Archive writer.
 * \return One on success. Zero otherwise.
 */
int liext_number_array_2d_get_data (
	const LIExtNumberArray2d* self,
	LIArcWriter*              writer)
{
	int i;

	for (i = 0 ; i < self->width * self->height ; i++)
	{
		if (!liarc_writer_append_float (writer, self->values[i]))
			return 0;
	}

	return 1;
}

/**
 * \brief Reads the raw data from an archive.
 * \param self Array.
 * \param reader Archive reader.
 * \return One on success. Zero otherwise.
 */
int liext_number_array_2d_set_data (
	LIExtNumberArray2d* self,
	LIArcReader*        reader)
{
	int i;

	for (i = 0 ; i < self->width * self->height ; i++)
	{
		if (!liarc_reader_get_float (reader, self->values + i))
			return 0;
	}

	return 1;
}

/**
 * \brief Samples the gradient at the given index.
 * \param self Array.
 * \param x X coordinate.
 * \param y Y coordinate.
 * \param result_x Return location for the horizontal gradient.
 * \param result_y Return location for the vertical gradient.
 */
void liext_number_array_2d_get_gradient (
	const LIExtNumberArray2d* self,
	int                       x,
	int                       y,
	float*                    result_x,
	float*                    result_y)
{
	int i;
	int w;
	int h;
	float c;
	float v[3][3];

	/* Calculate the index. */
	h = self->height;
	w = self->width;
	i = x + y * w;

	/* Sample the 3x3 window. */
	c = self->values[i];
	v[0][0] = (x > 0 && y > 0)? self->values[i - 1 - w] : c;
	v[1][0] = (y > 0)? self->values[i - w] : c;
	v[2][0] = (x < w-1 && y > 0)? self->values[i + 1 - w] : c;
	v[0][1] = (x > 0)? self->values[i - 1] : c;
	v[1][1] = c;
	v[2][1] = (x < w-1)? self->values[i + 1] : c;
	v[0][2] = (x > 0 && y < h-1)? self->values[i - 1 + w] : c;
	v[1][2] = (y < h-1)? self->values[i + w] : c;
	v[2][2] = (x < w-1 && y < h-1)? self->values[i + 1 + w] : c;

	/* Calculate the gradient using Sobel. */
	*result_x = ((v[0][0] - v[2][0]) + 2 * (v[0][1] - v[2][1]) + (v[0][2] - v[2][2])) / 4.0f;
	*result_y = ((v[0][0] - v[0][2]) + 2 * (v[1][0] - v[1][2]) + (v[2][0] - v[2][2])) / 4.0f;
}

/**
 * \brief Samples the gradient at the given index.
 * \param self Array.
 * \param fx X coordinate.
 * \param fy Y coordinate.
 * \param result_x Return location for the horizontal gradient.
 * \param result_y Return location for the vertical gradient.
 */
void liext_number_array_2d_get_gradient_bilinear (
	const LIExtNumberArray2d* self,
	float                     fx,
	float                     fy,
	float*                    result_x,
	float*                    result_y)
{
	int x;
	int y;
	float b;
	float gx[4];
	float gy[4];

	fx = LIMAT_CLAMP (fx, 0.0f, self->width - 1.0f);
	fy = LIMAT_CLAMP (fy, 0.0f, self->height - 1.0f);
	x = (int) fx;
	y = (int) fy;

	if (x == self->width - 1 && y == self->height - 1)
	{
		liext_number_array_2d_get_gradient (self, 0, 0, result_x, result_y);
	}
	else if (x == self->width - 1)
	{
		b = fy - y;
		liext_number_array_2d_get_gradient (self, x, y, gx, gy);
		liext_number_array_2d_get_gradient (self, x, y + 1, gx + 1, gy + 1);
		*result_x = (1.0f - b) * gx[0] + b * gx[1];
		*result_y = (1.0f - b) * gy[0] + b * gy[1];
	}
	else if (y == self->height - 1)
	{
		b = fx - x;
		liext_number_array_2d_get_gradient (self, x, y, gx, gy);
		liext_number_array_2d_get_gradient (self, x + 1, y, gx + 1, gy + 1);
		*result_x = (1.0f - b) * gx[0] + b * gx[1];
		*result_y = (1.0f - b) * gy[0] + b * gy[1];
	}
	else
	{
		b = fx - x;
		liext_number_array_2d_get_gradient (self, x, y, gx, gy);
		liext_number_array_2d_get_gradient (self, x + 1, y, gx + 1, gy + 1);
		liext_number_array_2d_get_gradient (self, x, y + 1, gx + 2, gy + 2);
		liext_number_array_2d_get_gradient (self, x + 1, y + 1, gx + 3, gy + 3);
		gx[0] = (1.0f - b) * gx[0] + b * gx[1];
		gx[1] = (1.0f - b) * gx[2] + b * gx[3];
		gy[0] = (1.0f - b) * gy[0] + b * gy[1];
		gy[1] = (1.0f - b) * gy[2] + b * gy[3];
		b = fy - y;
		*result_x = (1.0f - b) * gx[0] + b * gx[1];
		*result_y = (1.0f - b) * gy[0] + b * gy[1];
	}
}

/**
 * \brief Gets the maximum value of the array.
 * \param self Array.
 * \return Value
 */
float liext_number_array_2d_get_max (
	const LIExtNumberArray2d* self)
{
	int i;
	float v;

	v = self->values[0];
	for (i = 0 ; i < self->width * self->height ; i++)
		v = LIMAT_MAX (v, self->values[i]);

	return v;
}

/**
 * \brief Gets the minimum value of the array.
 * \param self Array.
 * \return Value
 */
float liext_number_array_2d_get_min (
	const LIExtNumberArray2d* self)
{
	int i;
	float v;

	v = self->values[0];
	for (i = 0 ; i < self->width * self->height ; i++)
		v = LIMAT_MIN (v, self->values[i]);

	return v;
}

/** @} */
/** @} */
