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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenBuffer Buffer
 * @{
 */

#include "render-buffer.h"

static int private_init (
	LIRenBuffer*       self,
	GLenum             target,
	const LIRenFormat* format,
	const void*        data,
	int                count,
	int                type);

/*****************************************************************************/

int liren_buffer_init_index (
	LIRenBuffer* self,
	const void*  data,
	int          count,
	int          type)
{
	static const LIRenFormat format =
	{
		sizeof (uint32_t),
		GL_FLOAT, 0,
		GL_FLOAT, 0,
		GL_FLOAT, 0,
		GL_FLOAT, 0
	};

	return private_init (self, GL_ELEMENT_ARRAY_BUFFER_ARB, &format, data, count, type);
}

int liren_buffer_init_vertex (
	LIRenBuffer*       self,
	const LIRenFormat* format,
	const void*        data,
	int                count,
	int                type)
{
	return private_init (self, GL_ARRAY_BUFFER_ARB, format, data, count, type);
}

void liren_buffer_free (
	LIRenBuffer* self)
{
	if (self->buffer)
		glDeleteBuffersARB (1, &self->buffer);
	lisys_free (self->elements.array);
}

void* liren_buffer_lock (
	LIRenBuffer* self,
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

void liren_buffer_replace_data (
	LIRenBuffer* self,
	void*        data)
{
	int size;
	GLenum mode;

	size = self->elements.count * self->format.size;
	if (self->buffer)
	{
		glBindBufferARB (self->target, self->buffer);
		switch (self->type)
		{
			case LIREN_BUFFER_TYPE_DYNAMIC:
				mode = GL_DYNAMIC_DRAW_ARB;
				break;
			case LIREN_BUFFER_TYPE_STATIC:
				mode = GL_STATIC_DRAW_ARB;
				break;
			case LIREN_BUFFER_TYPE_STREAM:
				mode = GL_STREAM_DRAW_ARB;
				break;
			default:
				lisys_assert (0 && "invalid vbo type");
				break;
		}
		glBufferDataARB (self->target, size, data, mode);
		glBindBufferARB (self->target, 0);
	}
	else
		memcpy (self->elements.array, data, size);
}

void liren_buffer_unlock (
	LIRenBuffer* self,
	void*        data)
{
	if (self->buffer)
	{
		glBindBufferARB (self->target, self->buffer);
		glUnmapBufferARB (self->target);
		glBindBufferARB (self->target, 0);
	}
}

int liren_buffer_get_size (
	LIRenBuffer* self)
{
	return self->elements.count * self->format.size;
}

/*****************************************************************************/

static int private_init (
	LIRenBuffer*       self,
	GLenum             target,
	const LIRenFormat* format,
	const void*        data,
	int                count,
	int                type)
{
	int size;

	memset (self, 0, sizeof (LIRenBuffer));
	if (GLEW_ARB_vertex_buffer_object)
		self->type = type;
	else
		self->type = LIREN_BUFFER_TYPE_MEMORY;
	self->target = target;
	self->format = *format;
	if (count)
	{
		size = count * format->size;
		switch (self->type)
		{
			case LIREN_BUFFER_TYPE_DYNAMIC:
				glGenBuffersARB (1, &self->buffer);
				glBindBufferARB (target, self->buffer);
				glBufferDataARB (target, size, data, GL_DYNAMIC_DRAW_ARB);
				glBindBufferARB (target, 0);
				break;
			case LIREN_BUFFER_TYPE_STATIC:
				glGenBuffersARB (1, &self->buffer);
				glBindBufferARB (target, self->buffer);
				glBufferDataARB (target, size, data, GL_STATIC_DRAW_ARB);
				glBindBufferARB (target, 0);
				break;
			case LIREN_BUFFER_TYPE_STREAM:
				glGenBuffersARB (1, &self->buffer);
				glBindBufferARB (target, self->buffer);
				glBufferDataARB (target, size, data, GL_STREAM_DRAW_ARB);
				glBindBufferARB (target, 0);
				break;
			default:
				self->elements.array = lisys_malloc (count * format->size);
				if (self->elements.array == NULL)
					return 0;
				memcpy (self->elements.array, data, size);
				break;
		}
		self->elements.count = count;
	}

	return 1;
}

/** @} */
/** @} */
