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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndBuffer Buffer
 * @{
 */

#include "render-buffer.h"

int
lirnd_buffer_init (lirndBuffer*   self,
                   lirndMaterial* material,
                   limdlVertex*   data,
                   int            count)
{
	int size;

	self->material = material;
	if (count)
	{
		size = count * sizeof (limdlVertex);
		self->vertices.array = malloc (count * sizeof (limdlVertex));
		if (self->vertices.array == NULL)
			return 0;
		self->vertices.count = count;
		memcpy (self->vertices.array, data, size);
		if (GLEW_ARB_vertex_buffer_object)
		{
			glGenBuffersARB (1, &self->buffer);
			glBindBufferARB (GL_ARRAY_BUFFER_ARB, self->buffer);
			glBufferDataARB (GL_ARRAY_BUFFER_ARB, size, data, GL_STREAM_DRAW_ARB);
			glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
		}
	}

	return 1;
}

void
lirnd_buffer_free (lirndBuffer* self)
{
	if (GLEW_ARB_vertex_buffer_object)
		glDeleteBuffersARB (1, &self->buffer);
	free (self->vertices.array);
}

/** @} */
/** @} */
