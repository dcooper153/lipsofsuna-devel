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
 * \addtogroup LIMdlShape Shape
 * @{
 */

#include <lipsofsuna/system.h>
#include "model-shape.h"

int
limdl_shape_read (LIMdlShape*  self,
                  LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_get_text (reader, "", &self->name) ||
	    !liarc_reader_get_uint32 (reader, &tmp))
		return 0;

	/* Allocate vertices. */
	if (tmp)
	{
		self->vertices.count = tmp;
		self->vertices.array = lisys_calloc (3 * tmp, sizeof (LIMatVector));
		if (self->vertices.array == NULL)
			return 0;
	}

	/* Read vertices. */
	for (i = 0 ; i < tmp ; i++)
	{
		if (!liarc_reader_get_float (reader, &self->vertices.array[i].x) ||
		    !liarc_reader_get_float (reader, &self->vertices.array[i].y) ||
		    !liarc_reader_get_float (reader, &self->vertices.array[i].z))
			return 0;
	}

	return 1;
}

int
limdl_shape_write (const LIMdlShape* self,
                   LIArcWriter*      writer)
{
	int i;

	if (!liarc_writer_append_string (writer, self->name) ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->vertices.count))
		return 0;
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		if (!liarc_writer_append_float (writer, self->vertices.array[i].x) ||
		    !liarc_writer_append_float (writer, self->vertices.array[i].y) ||
		    !liarc_writer_append_float (writer, self->vertices.array[i].z))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
