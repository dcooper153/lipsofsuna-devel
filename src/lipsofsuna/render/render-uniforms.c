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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenUniforms Uniforms
 * @{
 */

#include "render-uniforms.h"

static void private_queue_mat (
	LIRenUniforms* self,
	int            uniform,
	const float*   value,
	int            size);

static void private_queue_vec (
	LIRenUniforms* self,
	int            uniform,
	const float*   value,
	int            size);

/****************************************************************************/

void liren_uniforms_init (
	LIRenUniforms* self)
{
	/* Generate objects. */
	memset (self, 0, sizeof (LIRenUniforms));
	glGenBuffers (1, &self->uniform_buffer);
	glBindBuffer (GL_UNIFORM_BUFFER, self->uniform_buffer);
	glBufferData (GL_UNIFORM_BUFFER, LIREN_UNIFORM_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);
	self->flush_range_start = LIREN_UNIFORM_BUFFER_SIZE;
	self->flush_range_end = 0;
}

void liren_uniforms_clear (
	LIRenUniforms* self)
{
	glDeleteBuffers (1, &self->uniform_buffer);
	memset (self, 0, sizeof (LIRenUniforms));
}

void liren_uniforms_setup (
	LIRenUniforms* self,
	GLuint         program)
{
	int i;
	const char* uniform_names[LIREN_UNIFORM_COUNT] = {
		"LOS.light_ambient",
		"LOS.light_diffuse",
		"LOS.light_direction",
		"LOS.light_direction_premult",
		"LOS.light_equation",
		"LOS.light_matrix",
		"LOS.light_position",
		"LOS.light_position_premult",
		"LOS.light_specular",
		"LOS.light_spot",
		"LOS.light_type",
		"LOS.material_diffuse",
		"LOS.material_param_0",
		"LOS.material_shininess",
		"LOS.material_specular",
		"LOS.matrix_model",
		"LOS.matrix_modelview",
		"LOS.matrix_modelview_inverse",
		"LOS.matrix_modelview_projection",
		"LOS.matrix_normal",
		"LOS.matrix_projection",
		"LOS.matrix_projection_inverse",
		"LOS.time"
	};

	/* Setup when called by the first shader. */
	if (self->ready)
		return;
	self->ready = 1;

	/* Setup the uniform buffer. */
	/* The uniform buffer bound here will remain bound until the graphics
	   system is reset when the game exits or a new mod is launched. */
	glBindBufferBase (GL_UNIFORM_BUFFER, 0, self->uniform_buffer);

	/* Get uniform offsets. */
	glGetUniformIndices (program, LIREN_UNIFORM_COUNT, uniform_names, self->uniform_indices);
	glGetActiveUniformsiv (program, LIREN_UNIFORM_COUNT, self->uniform_indices,
		GL_UNIFORM_OFFSET, self->uniform_offsets);
	glGetActiveUniformsiv (program, LIREN_UNIFORM_COUNT, self->uniform_indices,
		GL_UNIFORM_MATRIX_STRIDE, self->uniform_matrix_strides);

	/* Check for validity. */
	for (i = 0 ; i < LIREN_UNIFORM_COUNT ; i++)
	{
		if (self->uniform_indices[i] == GL_INVALID_INDEX)
		{
			lisys_error_set (EINVAL, "could not find uniform `%s'", uniform_names[i]);
			lisys_error_report ();
		}
	}
}

void liren_uniforms_commit (
	LIRenUniforms* self)
{
	int start;
	int end;

	start = self->flush_range_start;
	end = self->flush_range_end;
	if (!self->ready || start > end)
		return;

	glBufferSubData (GL_UNIFORM_BUFFER, start, end - start, NULL);
	glBufferSubData (GL_UNIFORM_BUFFER, start, end - start, self->data + start);
	self->flush_range_start = LIREN_UNIFORM_BUFFER_SIZE;
	self->flush_range_end = 0;
}

void liren_uniforms_set_float (
	LIRenUniforms* self,
	int            uniform,
	float          value)
{
	private_queue_vec (self, uniform, &value, 1);
}

void liren_uniforms_set_vec3 (
	LIRenUniforms* self,
	int            uniform,
	const float*   value)
{
	private_queue_vec (self, uniform, value, 3);
}

void liren_uniforms_set_vec4 (
	LIRenUniforms* self,
	int            uniform,
	const float*   value)
{
	private_queue_vec (self, uniform, value, 4);
}

void liren_uniforms_set_mat3 (
	LIRenUniforms* self,
	int            uniform,
	const float*   value)
{
	private_queue_mat (self, uniform, value, 3);
}

void liren_uniforms_set_mat4 (
	LIRenUniforms* self,
	int            uniform,
	const float*   value)
{
	private_queue_mat (self, uniform, value, 4);
}

/****************************************************************************/

static void private_queue_mat (
	LIRenUniforms* self,
	int            uniform,
	const float*   value,
	int            size)
{
	int i;
	int offset;
	int stride;

	if (memcmp (self->uniform_values[uniform], value, 16 * sizeof (float)))
	{
		/* Copy to back buffer. */
		offset = self->uniform_offsets[uniform];
		stride = self->uniform_matrix_strides[uniform] + 4 * sizeof (float);
		memcpy (self->uniform_values[uniform], value, 16 * sizeof (float));
		for (i = 0 ; i < size ; i++)
			memcpy (self->data + offset + i * stride, value + i * 4, size * sizeof (float));

		/* Recalculate dirty range. */
		self->flush_range_start = LIMAT_MIN (self->flush_range_start, offset);
		self->flush_range_end = LIMAT_MAX (self->flush_range_end, offset + size * stride);
	}
}

static void private_queue_vec (
	LIRenUniforms* self,
	int            uniform,
	const float*   value,
	int            size)
{
	int offset;

	if (memcmp (self->uniform_values[uniform], value, size * sizeof (float)))
	{
		/* Copy to back buffer. */
		offset = self->uniform_offsets[uniform];
		memcpy (self->uniform_values[uniform], value, size * sizeof (float));
		memcpy (self->data + offset, value, size * sizeof (float));

		/* Recalculate dirty range. */
		self->flush_range_start = LIMAT_MIN (self->flush_range_start, offset);
		self->flush_range_end = LIMAT_MAX (self->flush_range_end, offset + size * sizeof (float));
	}
}

/** @} */
/** @} */

