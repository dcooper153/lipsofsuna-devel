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

#include "model.h"
#include "model-faces.h"

int limdl_faces_init_copy (
	LIMdlFaces* self,
	LIMdlFaces* faces)
{
	self->start = faces->start;
	self->count = faces->count;

	return 1;
}

void limdl_faces_free (
	LIMdlFaces* self)
{
}

int limdl_faces_read (
	LIMdlFaces*  self,
	LIMdlModel*  model,
	LIArcReader* reader)
{
	uint32_t i;
	uint32_t mat;
	uint32_t count;
	uint32_t index;
	LIMdlIndex* tmp;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &mat) ||
	    !liarc_reader_get_uint32 (reader, &count))
		return 0;
	self->start = model->indices.count;
	self->count = count;

	/* Read indices. */
	if (count)
	{
		tmp = lisys_realloc (model->indices.array, (model->indices.count + count) * sizeof (LIMdlIndex));
		if (tmp == NULL)
			return 0;
		model->indices.array = tmp;
		for (i = 0 ; i < count ; i++)
		{
			if (!liarc_reader_get_uint32 (reader, &index))
				return 0;
			model->indices.array[model->indices.count + i] = index;
		}
		model->indices.count += count;
	}

	return 1;
}

int limdl_faces_write (
	const LIMdlFaces* self,
	const LIMdlModel* model,
	LIArcWriter*      writer)
{
	int i;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, 0) ||
	    !liarc_writer_append_uint32 (writer, self->count))
		return 0;

	/* Write indices. */
	for (i = 0 ; i < self->count ; i++)
	{
		if (!liarc_writer_append_uint32 (writer, model->indices.array[self->start + i]))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
