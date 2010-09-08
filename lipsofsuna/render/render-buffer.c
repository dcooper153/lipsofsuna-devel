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

int liren_buffer_init (
	LIRenBuffer*       self,
	const void*        index_data,
	int                index_count,
	const LIRenFormat* vertex_format,
	const void*        vertex_data,
	int                vertex_count,
	int                type)
{
	int size;

	lisys_assert (vertex_format->vtx_format);

	memset (self, 0, sizeof (LIRenBuffer));
	self->type = type;
	self->vertex_format = *vertex_format;
	glBindVertexArray (0);

	/* Create index buffer object. */
	if (index_count)
	{
		size = index_count * sizeof (uint32_t);
		glGenBuffers (1, &self->index_buffer);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, size, index_data, GL_STATIC_DRAW);
		self->indices.count = index_count;
	}

	/* Create vertex buffer object. */
	if (vertex_count)
	{
		size = vertex_count * vertex_format->size;
		switch (self->type)
		{
			case LIREN_BUFFER_TYPE_DYNAMIC:
				glGenBuffers (1, &self->vertex_buffer);
				glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
				glBufferData (GL_ARRAY_BUFFER, size, vertex_data, GL_DYNAMIC_DRAW);
				break;
			case LIREN_BUFFER_TYPE_STATIC:
				glGenBuffers (1, &self->vertex_buffer);
				glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
				glBufferData (GL_ARRAY_BUFFER, size, vertex_data, GL_STATIC_DRAW);
				break;
			case LIREN_BUFFER_TYPE_STREAM:
				glGenBuffers (1, &self->vertex_buffer);
				glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
				glBufferData (GL_ARRAY_BUFFER, size, vertex_data, GL_STREAM_DRAW);
				break;
			default:
				lisys_assert (0 && "invalid vbo type");
				break;
		}
		self->vertices.count = vertex_count;
	}

	/* Create vertex array object. */
	if (self->vertex_buffer)
	{
		glGenVertexArrays (1, &self->vertex_array);
		glBindVertexArray (self->vertex_array);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		glEnableVertexAttribArray (LIREN_ATTRIBUTE_COORD);
		glVertexAttribPointer (LIREN_ATTRIBUTE_COORD, 3, vertex_format->vtx_format,
			GL_FALSE, vertex_format->size, NULL + vertex_format->vtx_offset);
		if (vertex_format->nml_format)
		{
			glEnableVertexAttribArray (LIREN_ATTRIBUTE_NORMAL);
			glVertexAttribPointer (LIREN_ATTRIBUTE_NORMAL, 3, vertex_format->nml_format,
				GL_FALSE, vertex_format->size, NULL + vertex_format->nml_offset);
		}
		if (vertex_format->tex_format)
		{
			glEnableVertexAttribArray (LIREN_ATTRIBUTE_TEXCOORD);
			glVertexAttribPointer (LIREN_ATTRIBUTE_TEXCOORD, 2, vertex_format->tex_format,
				GL_FALSE, vertex_format->size, NULL + vertex_format->tex_offset);
		}
	}

	return 1;
}

void liren_buffer_free (
	LIRenBuffer* self)
{
	if (self->vertex_array)
		glDeleteVertexArrays (1, &self->vertex_array);
	if (self->index_buffer)
		glDeleteBuffers (1, &self->index_buffer);
	if (self->vertex_buffer)
		glDeleteBuffers (1, &self->vertex_buffer);
}

void* liren_buffer_lock_indices (
	LIRenBuffer* self,
	int          write)
{
	void* ret;

	if (self->index_buffer)
	{
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		if (write)
			ret = glMapBuffer (GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
		else
			ret = glMapBuffer (GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
		return ret;
	}
	else
		return NULL;
}

void* liren_buffer_lock_vertices (
	LIRenBuffer* self,
	int          write)
{
	void* ret;

	if (self->vertex_buffer)
	{
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		if (write)
			ret = glMapBuffer (GL_ARRAY_BUFFER, GL_READ_WRITE);
		else
			ret = glMapBuffer (GL_ARRAY_BUFFER, GL_READ_ONLY);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		return ret;
	}
	else
		return NULL;
}

void liren_buffer_replace_data (
	LIRenBuffer* self,
	void*        data)
{
	int size;
	GLenum mode;

	size = self->vertices.count * self->vertex_format.size;
	if (self->vertex_buffer)
	{
		glBindVertexArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		switch (self->type)
		{
			case LIREN_BUFFER_TYPE_DYNAMIC:
				mode = GL_DYNAMIC_DRAW;
				break;
			case LIREN_BUFFER_TYPE_STATIC:
				mode = GL_STATIC_DRAW;
				break;
			case LIREN_BUFFER_TYPE_STREAM:
				mode = GL_STREAM_DRAW;
				break;
			default:
				lisys_assert (0 && "invalid vbo type");
				break;
		}
		glBufferData (GL_ARRAY_BUFFER, size, data, mode);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	}
}

void liren_buffer_unlock_indices (
	LIRenBuffer* self,
	void*        data)
{
	if (self->index_buffer)
	{
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->index_buffer);
		glUnmapBuffer (GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void liren_buffer_unlock_vertices (
	LIRenBuffer* self,
	void*        data)
{
	if (self->vertex_buffer)
	{
		glBindBuffer (GL_ARRAY_BUFFER, self->vertex_buffer);
		glUnmapBuffer (GL_ARRAY_BUFFER);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	}
}

int liren_buffer_get_size (
	LIRenBuffer* self)
{
	return self->vertices.count * self->vertex_format.size;
}

/** @} */
/** @} */
