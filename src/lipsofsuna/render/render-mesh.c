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

#include "render-attribute.h"
#include "render-mesh.h"
#include "render.h"

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenMesh Mesh
 * @{
 */

int liren_mesh_init (
	LIRenMesh* self,
	void*      index_data,
	int        index_count,
	void*      vertex_data,
	int        vertex_count)
{
	GLint restore;
	const int vertex_size[2] = {
		16 * sizeof (float) + 8 * sizeof (char),
		11 * sizeof (float) };
	const int vertex_tex_offset = 0 * sizeof (float);
	const int vertex_nml_offset = 2 * sizeof (float);
	const int vertex_vtx_offset = 5 * sizeof (float);
	const int vertex_tan_offset = 8 * sizeof (float);
	const int vertex_we1_offset = 8 * sizeof (float);
	const int vertex_we2_offset = 12 * sizeof (float);
	const int vertex_bo1_offset = 16 * sizeof (float);
	const int vertex_bo2_offset = 16 * sizeof (float) + 4 * sizeof (char);

	/* Allocate objects. */
	memset (self, 0, sizeof (LIRenMesh));
	if (!index_count || !vertex_count)
		return 1;
	glGenBuffers (3, self->buffers);
	glGenVertexArrays (2, self->arrays);
	self->counts[0] = index_count;
	self->counts[1] = vertex_count;
	self->counts[2] = index_count;
	self->sizes[0] = self->counts[0] * sizeof (GLuint);
	self->sizes[1] = self->counts[1] * vertex_size[0];
	self->sizes[2] = self->counts[2] * vertex_size[1];

	/* Don't break the active vertex array. */
	glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
	glBindVertexArray (0);

	/* Setup source index buffer. */
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->buffers[0]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, self->sizes[0], index_data, GL_STATIC_DRAW);

	/* Setup source vertex buffer. */
	glBindBuffer (GL_ARRAY_BUFFER, self->buffers[1]);
	glBufferData (GL_ARRAY_BUFFER, self->sizes[1], vertex_data, GL_STATIC_DRAW);

	/* Setup destination vertex buffer. */
	glBindBuffer (GL_ARRAY_BUFFER, self->buffers[2]);
	glBufferData (GL_ARRAY_BUFFER, self->sizes[2], NULL, GL_STATIC_READ);

	/* Setup vertex array object for transform feedback. */
	glBindVertexArray (self->arrays[0]);
	glBindBuffer (GL_ARRAY_BUFFER, self->buffers[1]);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, self->buffers[0]);
	glBindBufferBase (GL_TRANSFORM_FEEDBACK_BUFFER, 0, self->buffers[2]);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_TEXCOORD);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_NORMAL);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_COORD);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_WEIGHTS1);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_WEIGHTS2);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_BONES1);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_BONES2);
	glVertexAttribPointer (LIREN_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, vertex_size[0], NULL + vertex_tex_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, vertex_size[0], NULL + vertex_nml_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_COORD, 3, GL_FLOAT, GL_FALSE, vertex_size[0], NULL + vertex_vtx_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_WEIGHTS1, 4, GL_FLOAT, GL_FALSE, vertex_size[0], NULL + vertex_we1_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_WEIGHTS2, 4, GL_FLOAT, GL_FALSE, vertex_size[0], NULL + vertex_we2_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_BONES1, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertex_size[0], NULL + vertex_bo1_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_BONES2, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertex_size[0], NULL + vertex_bo2_offset);

	/* Setup vertex array object for rendering. */
	glBindVertexArray (self->arrays[1]);
	glBindBuffer (GL_ARRAY_BUFFER, self->buffers[2]);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_TEXCOORD);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_NORMAL);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_COORD);
	glEnableVertexAttribArray (LIREN_ATTRIBUTE_TANGENT);
	glVertexAttribPointer (LIREN_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, vertex_size[1], NULL + vertex_tex_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, vertex_size[1], NULL + vertex_nml_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_COORD, 3, GL_FLOAT, GL_FALSE, vertex_size[1], NULL + vertex_vtx_offset);
	glVertexAttribPointer (LIREN_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, vertex_size[1], NULL + vertex_tan_offset);

	/* Don't break the active vertex array. */
	glBindVertexArray (restore);

	return 1;
}

void liren_mesh_clear (
	LIRenMesh* self)
{
	glDeleteVertexArrays (2, self->arrays);
	glDeleteBuffers (3, self->buffers);
	memset (self, 0, sizeof (LIRenMesh));
}

void liren_mesh_deform (
	LIRenMesh* self)
{
	GLint restore;

	if (!self->arrays[0])
		return;
	glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
	glBindVertexArray (self->arrays[0]);
	glBindBufferBase (GL_TRANSFORM_FEEDBACK_BUFFER, 0, self->buffers[2]);
	glBeginTransformFeedback (GL_TRIANGLES);
	glEnable (GL_RASTERIZER_DISCARD);
	glDrawElements (GL_TRIANGLES, self->counts[0], GL_UNSIGNED_INT, NULL);
	glDisable (GL_RASTERIZER_DISCARD);
	glEndTransformFeedback ();
	glBindVertexArray (restore);
}

/**
 * \brief Locks the deformed vertex array for reading.
 * \param self Mesh.
 * \return Pointer to the locked array, or NULL.
 */
void* liren_mesh_lock_vertices (
	const LIRenMesh* self)
{
	void* ret;
	GLint restore;

	if (self->buffers[2])
	{
		glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
		glBindVertexArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, self->buffers[2]);
		ret = glMapBuffer (GL_ARRAY_BUFFER, GL_READ_ONLY);
		glBindVertexArray (restore);
		return ret;
	}
	else
		return NULL;
}

void liren_mesh_unlock_vertices (
	const LIRenMesh* self)
{
	GLint restore;

	if (self->buffers[2])
	{
		glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
		glBindVertexArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, self->buffers[2]);
		glUnmapBuffer (GL_ARRAY_BUFFER);
		glBindVertexArray (restore);
	}
}

void liren_mesh_get_format (
	const LIRenMesh* self,
	LIRenFormat*     value)
{
	const LIRenFormat format =
	{
		11 * sizeof (float),
		GL_FLOAT, 0 * sizeof (float),
		GL_FLOAT, 2 * sizeof (float),
		GL_FLOAT, 5 * sizeof (float)
	};
	*value = format;
}

/** @} */
/** @} */

