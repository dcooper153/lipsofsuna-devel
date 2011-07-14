/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlFaces Faces
 * @{
 */

#include "model-faces.h"

int limdl_faces_init_copy (
	LIMdlFaces* self,
	LIMdlFaces* faces)
{
	self->material = faces->material;
	if (faces->indices.count)
	{
		self->indices.array = lisys_calloc (faces->indices.count, sizeof (LIMdlIndex));
		if (self->indices.array == NULL)
			return 0;
		self->indices.count = faces->indices.count;
		self->indices.capacity = faces->indices.count;
		memcpy (self->indices.array, faces->indices.array, faces->indices.count * sizeof (LIMdlIndex));
	}

	return 1;
}

void limdl_faces_free (
	LIMdlFaces* self)
{
	lisys_free (self->indices.array);
}

int limdl_faces_read (
	LIMdlFaces*  self,
	LIArcReader* reader)
{
	uint32_t i;
	uint32_t mat;
	uint32_t count;
	uint32_t index;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &mat) ||
	    !liarc_reader_get_uint32 (reader, &count))
		return 0;
	self->material = mat;

	/* Read indices. */
	if (count)
	{
		self->indices.array = lisys_calloc (count, sizeof (LIMdlIndex));
		if (self->indices.array == NULL)
			return 0;
		self->indices.count = count;
		self->indices.capacity = count;
		for (i = 0 ; i < count ; i++)
		{
			if (!liarc_reader_get_uint32 (reader, &index))
				return 0;
			self->indices.array[i] = index;
		}
	}

	return 1;
}

int limdl_faces_write (
	LIMdlFaces*  self,
	LIArcWriter* writer)
{
	int i;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->material) ||
	    !liarc_writer_append_uint32 (writer, self->indices.count))
		return 0;

	/* Write indices. */
	for (i = 0 ; i < self->indices.count ; i++)
	{
		if (!liarc_writer_append_uint32 (writer, self->indices.array[i]))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
