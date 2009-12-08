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

static int
private_init (lirndBuffer*       self,
              GLenum             target,
              const lirndFormat* format,
              const void*        data,
              int                count);

/*****************************************************************************/

int
lirnd_buffer_init_index (lirndBuffer* self,
                         const void*  data,
                         int          count)
{
	static const lirndFormat format =
	{
		sizeof (uint32_t), 0,
		{ GL_FLOAT, GL_FLOAT, GL_FLOAT },
		{ 0, 0, 0 }, 
		GL_FLOAT, 0,
		GL_FLOAT, 0
	};

	return private_init (self, GL_ELEMENT_ARRAY_BUFFER_ARB, &format, data, count);
}

int
lirnd_buffer_init_vertex (lirndBuffer*       self,
                          const lirndFormat* format,
                          const void*        data,
                          int                count)
{
	return private_init (self, GL_ARRAY_BUFFER_ARB, format, data, count);
}

void
lirnd_buffer_free (lirndBuffer* self)
{
	if (GLEW_ARB_vertex_buffer_object)
		glDeleteBuffersARB (1, &self->buffer);
	lisys_free (self->elements.array);
}

void*
lirnd_buffer_lock (lirndBuffer* self,
                   int          write)
{
	int size;
	void* ret;

	size = self->elements.count * self->format.size;
	if (self->buffer != 0)
	{
		glBindBufferARB (self->target, self->buffer);
		if (write)
			ret = glMapBufferARB (self->target, GL_READ_WRITE_ARB);
		else
			ret = glMapBufferARB (self->target, GL_READ_ONLY_ARB);
		glBindBufferARB (self->target, 0);
		return ret;
	}
	else
		return self->elements.array;
}

void
lirnd_buffer_unlock (lirndBuffer* self,
                     void*        data)
{
	if (self->buffer != 0)
	{
		glBindBufferARB (self->target, self->buffer);
		glUnmapBufferARB (self->target);
		glBindBufferARB (self->target, 0);
	}
}

/*****************************************************************************/

static int
private_init (lirndBuffer*       self,
              GLenum             target,
              const lirndFormat* format,
              const void*        data,
              int                count)
{
	int size;

	self->target = target;
	self->format = *format;
	if (count)
	{
		size = count * format->size;
		if (GLEW_ARB_vertex_buffer_object)
		{
			glGenBuffersARB (1, &self->buffer);
			glBindBufferARB (target, self->buffer);
			glBufferDataARB (target, size, data, GL_STREAM_DRAW_ARB);
			glBindBufferARB (target, 0);
		}
		else
		{
			self->elements.array = lisys_malloc (count * format->size);
			if (self->elements.array == NULL)
				return 0;
			memcpy (self->elements.array, data, size);
		}
		self->elements.count = count;
	}

	return 1;
}

/** @} */
/** @} */
