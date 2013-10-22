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

#include "number-array-2d.h"

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

/** @} */
/** @} */
