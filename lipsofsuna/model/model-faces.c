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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlFaces Faces
 * @{
 */

#include "model-faces.h"

static int
private_read_weights (limdlFaces*   self,
                      limdlWeights* weights,
                      liReader*     reader);

/*****************************************************************************/

void
limdl_faces_free (limdlFaces* self)
{
	int i;

	for (i = 0 ; i < self->weights.count ; i++)
		free (self->weights.array[i].weights);
	free (self->vertices.array);
	free (self->weights.array);
}

int
limdl_faces_read (limdlFaces* self,
                  liReader*   reader)
{
	uint32_t i;
	uint32_t mat;
	uint32_t count;
	limdlVertex* vertex;

	/* Read header. */
	if (!li_reader_get_uint32 (reader, &mat) ||
	    !li_reader_get_uint32 (reader, &count))
		goto error;
	self->material = mat;

	/* Read vertices. */
	if (count)
	{
		self->vertices.array = calloc (count, sizeof (limdlVertex));
		if (self->vertices.array == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		self->vertices.count = count;
		for (i = 0 ; i < count ; i++)
		{
			vertex = self->vertices.array + i;
			if (!li_reader_get_float (reader, vertex->texcoord + 0) ||
				!li_reader_get_float (reader, vertex->texcoord + 1) ||
				!li_reader_get_float (reader, vertex->texcoord + 2) ||
				!li_reader_get_float (reader, vertex->texcoord + 3) ||
				!li_reader_get_float (reader, vertex->texcoord + 4) ||
				!li_reader_get_float (reader, vertex->texcoord + 5) ||
				!li_reader_get_float (reader, &vertex->normal.x) ||
				!li_reader_get_float (reader, &vertex->normal.y) ||
				!li_reader_get_float (reader, &vertex->normal.z) ||
				!li_reader_get_float (reader, &vertex->coord.x) ||
				!li_reader_get_float (reader, &vertex->coord.y) ||
				!li_reader_get_float (reader, &vertex->coord.z))
				goto error;
		}
	}

	/* Read weights. */
	if (count)
	{
		self->weights.array = calloc (count, sizeof (limdlWeights));
		if (self->weights.array == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		self->weights.count = count;
		for (i = 0 ; i < self->weights.count ; i++)
		{
			if (!private_read_weights (self, self->weights.array + i, reader))
				goto error;
		}
	}

	return 1;

error:
	return 0;
}

int
limdl_faces_write (limdlFaces*  self,
                   liarcWriter* writer)
{
	int i;
	int j;
	limdlVertex* vertex;
	limdlWeights* weights;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->material) ||
	    !liarc_writer_append_uint32 (writer, self->vertices.count))
		return 0;

	/* Write vertices. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		vertex = self->vertices.array + i;
		if (!liarc_writer_append_float (writer, vertex->texcoord[0]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[1]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[2]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[3]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[4]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[5]) ||
		    !liarc_writer_append_float (writer, vertex->normal.x) ||
		    !liarc_writer_append_float (writer, vertex->normal.y) ||
		    !liarc_writer_append_float (writer, vertex->normal.z) ||
		    !liarc_writer_append_float (writer, vertex->coord.x) ||
		    !liarc_writer_append_float (writer, vertex->coord.y) ||
		    !liarc_writer_append_float (writer, vertex->coord.z))
			return 0;
	}

	/* Write weights. */
	for (i = 0 ; i < self->weights.count ; i++)
	{
		weights = self->weights.array + i;
		if (!liarc_writer_append_uint32 (writer, weights->count))
			return 0;
		for (j = 0 ; j < weights->count ; j++)
		{
			if (!liarc_writer_append_uint32 (writer, weights->weights[j].group) ||
			    !liarc_writer_append_float (writer, weights->weights[j].weight))
				return 0;
		}
	}

	return 1;
}

/*****************************************************************************/

static int
private_read_weights (limdlFaces*  self,
                      limdlWeights* weights,
                      liReader*     reader)
{
	uint32_t i;
	uint32_t count;
	uint32_t group;

	/* Read header. */
	if (!li_reader_get_uint32 (reader, &count))
		return 0;

	/* Allocate weights. */
	weights->count = count;
	weights->weights = calloc (count, sizeof (limdlWeight));
	if (weights->weights == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	/* Read weights. */
	for (i = 0 ; i < count ; i++)
	{
		if (!li_reader_get_uint32 (reader, &group) ||
			!li_reader_get_float (reader, &weights->weights[i].weight))
			return 0;
		weights->weights[i].group = group;
	}

	return 1;
}

/** @} */
/** @} */
