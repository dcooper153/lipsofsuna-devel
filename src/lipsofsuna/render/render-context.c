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
 * \addtogroup LIRenContext Context
 * @{
 */

#include <string.h>
#include "render-context.h"

void liren_context_init (
	LIRenContext* self)
{
	LIRenRender* render;
	LIRenUniforms uniforms;

	render = self->render;
	uniforms = self->uniforms;
	memset (self, 0, sizeof (LIRenContext));
	self->render = render;
	self->scene = NULL;
	self->light = NULL;
	self->array = 0;
	self->incomplete = 1;
	self->blend.enable = 0;
	self->blend.blend_src = GL_SRC_ALPHA;
	self->blend.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
	self->changed.blend = 1;
	self->changed.buffer = 1;
	self->changed.cull = 1;
	self->changed.depth = 1;
	self->changed.shader = 1;
	self->cull.enable = 1;
	self->cull.front_face = GL_CCW;
	self->depth.enable_test = 1;
	self->depth.enable_write = 1;
	self->depth.depth_func = GL_LEQUAL;
	self->matrix.model = limat_matrix_identity ();
	self->matrix.modelview = limat_matrix_identity ();
	self->matrix.modelviewinverse = limat_matrix_identity ();
	self->matrix.projection = limat_matrix_identity ();
	self->matrix.projectioninverse = limat_matrix_identity ();
	self->matrix.view = limat_matrix_identity ();
	self->textures.count = 0;
	self->uniforms = uniforms;
}

void liren_context_bind (
	LIRenContext* self)
{
	/* Bind shader. */
	if (self->changed.shader)
	{
		if (self->shader != NULL && self->shader->passes[self->shader_pass].program)
		{
			glUseProgram (self->shader->passes[self->shader_pass].program);
			self->incomplete = 0;
		}
		else
		{
			glUseProgram (0);
			self->incomplete = 1;
		}
	}

	/* Bind vertex array. */
	if (self->changed.buffer)
		glBindVertexArray (self->array);

	/* Update blend, cull, and depth modes. */
	if (self->changed.blend)
	{
		if (self->blend.enable)
		{
			glEnable (GL_BLEND);
			glBlendFunc (self->blend.blend_src, self->blend.blend_dst);
		}
		else
			glDisable (GL_BLEND);
	}
	if (self->changed.cull)
	{
		if (self->cull.enable)
		{
			glEnable (GL_CULL_FACE);
			glFrontFace (self->cull.front_face);
		}
		else
			glDisable (GL_CULL_FACE);
	}
	if (self->changed.depth)
	{
		if (self->depth.enable_test)
		{
			glEnable (GL_DEPTH_TEST);
			glDepthFunc (self->depth.depth_func);
		}
		else
			glDisable (GL_DEPTH_TEST);
		if (self->depth.enable_write)
			glDepthMask (GL_TRUE);
		else
			glDepthMask (GL_FALSE);
	}

	/* Update uniforms. */
	liren_uniforms_commit (&self->uniforms);

	/* All state changes were applied. */
	self->changed.blend = 0;
	self->changed.buffer = 0;
	self->changed.cull = 0;
	self->changed.depth = 0;
	self->changed.shader = 0;
}

/**
 * \brief Renders an array of primitives.
 * \param self Rendering context.
 * \param type Primitive type.
 * \param start Starting vertex.
 * \param count Number of vertices to draw.
 */
void liren_context_render_array (
	LIRenContext* self,
	GLenum        type,
	int           start,
	int           count)
{
	if (self->incomplete)
		return;

	lisys_assert (start >= 0);

	glDrawArrays (type, start, count);
}

/**
 * \brief Renders primitives.
 * \param self Rendering context.
 * \param type Primitive type.
 * \param vertices Array of vertices.
 * \param count Number of vertices.
 */
int liren_context_render_immediate (
	LIRenContext*      self,
	GLenum             type,
	const LIRenVertex* vertices,
	int                count)
{
	int* offset;
	LIRenBuffer* buffer;

	offset = &self->render->immediate.offset;
	buffer = self->render->immediate.buffer;
	lisys_assert (count > 0);
	lisys_assert (count <= buffer->vertices.count);

	/* Make sure the right buffer is bound. */
	if (self->array != buffer->vertex_array)
	{
		liren_context_set_buffer (self, buffer);
		liren_context_bind (self);
	}

	/* Upload the data to the vertex buffer. */
	/* The array buffer is bound here because upload_vertices unbound it. */
	if (*offset > buffer->vertices.count - count)
		*offset = 0;
	liren_buffer_upload_vertices (buffer, *offset, count, vertices);
	glBindBuffer (GL_ARRAY_BUFFER, buffer->vertex_buffer);

	/* Render the primitive array. */
	liren_context_render_array (self, type, *offset, count);
	*offset += count;

	return 1;
}

/**
 * \brief Renders indexed triangles.
 * \param self Rendering context.
 * \param start Starting index.
 * \param count Number of indices to draw.
 */
void liren_context_render_indexed (
	LIRenContext* self,
	int           start,
	int           count)
{
	if (self->incomplete)
		return;

	lisys_assert (start >= 0);

	glDrawElements (GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL + start * sizeof (uint32_t));

#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.materials++;
	self->render->profiling.faces += self->buffer->indices.count / 3;
	self->render->profiling.vertices += self->buffer->indices.count;
#endif
}

void liren_context_set_blend (
	LIRenContext* self,
	int           enable,
	GLenum        blend_src,
	GLenum        blend_dst)
{
	if (self->blend.enable != enable ||
	    self->blend.blend_src != blend_src ||
	    self->blend.blend_dst != blend_dst)
	{
		self->blend.enable = enable;
		self->blend.blend_src = blend_src;
		self->blend.blend_dst = blend_dst;
		self->changed.blend = 1;
	}
}

void liren_context_set_buffer (
	LIRenContext* self,
	LIRenBuffer*  buffer)
{
	GLuint array;

	array = (buffer != NULL)? buffer->vertex_array : 0;
	if (self->array != array)
	{
		self->array = array;
		self->changed.buffer = 1;
	}
}

void liren_context_set_cull (
	LIRenContext* self,
	int           enable,
	int           front_face)
{
	if (self->cull.enable != enable ||
	    self->cull.front_face != front_face)
	{
		self->cull.enable = enable;
		self->cull.front_face = front_face;
		self->changed.cull = 1;
	}
}

int liren_context_get_deferred (
	LIRenContext* self)
{
	return self->deferred;
}

void liren_context_set_deferred (
	LIRenContext* self,
	int           value)
{
	if (self->deferred != value)
	{
		self->deferred = value;
		self->changed.blend = 1;
		self->changed.depth = 1;
	}
}

void liren_context_set_depth (
	LIRenContext* self,
	int           enable_test,
	int           enable_write,
	GLenum        depth_func)
{
	if (self->depth.enable_test != enable_test ||
	    self->depth.enable_write != enable_write ||
	    self->depth.depth_func != depth_func)
	{
		self->depth.enable_test = enable_test;
		self->depth.enable_write = enable_write;
		self->depth.depth_func = depth_func;
		self->changed.depth = 1;
	}
}

void liren_context_set_diffuse (
	LIRenContext* self,
	const float*  value)
{
	liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_DIFFUSE, value);
}

void liren_context_set_flags (
	LIRenContext* self,
	int           value)
{
/*	self->lighting = ((value & LIREN_FLAG_LIGHTING) != 0);
	self->texturing = ((value & LIREN_FLAG_TEXTURING) != 0);*/
	self->shadows = ((value & LIREN_FLAG_SHADOW1) != 0);
}

void liren_context_set_frustum (
	LIRenContext*       self,
	const LIMatFrustum* frustum)
{
	self->frustum = *frustum;
}

void liren_context_set_light (
	LIRenContext* self,
	LIRenLight*   value)
{
	if (value != NULL && value != self->light)
	{
		self->light = value;

		/* Update uniforms. */
		liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT_AMBIENT, value->ambient);
		liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT_DIFFUSE, value->diffuse);
		liren_uniforms_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT_DIRECTION, value->cache.dir_world);
		liren_uniforms_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT_DIRECTION_PREMULT, value->cache.dir_view);
		liren_uniforms_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT_EQUATION, value->equation);
		liren_uniforms_set_mat3 (&self->uniforms, LIREN_UNIFORM_LIGHT_MATRIX, value->cache.matrix.m);
		liren_uniforms_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT_POSITION, value->cache.pos_world);
		liren_uniforms_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT_POSITION_PREMULT, value->cache.pos_view);
		liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_LIGHT_SPECULAR, value->specular);
		liren_uniforms_set_vec3 (&self->uniforms, LIREN_UNIFORM_LIGHT_SPOT, value->cache.spot);

		/* Bind shadow texture. */
		if (value->shadow.map)
		{
			if (self->shadow_texture != value->shadow.map)
			{
				self->shadow_texture = value->shadow.map;
				glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_SHADOW_TEXTURE);
				glBindTexture (GL_TEXTURE_2D, self->shadow_texture);
			}
		}
		else
		{
			if (self->shadow_texture != self->render->helpers.depth_texture_max)
			{
				self->shadow_texture = self->render->helpers.depth_texture_max;
				glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_SHADOW_TEXTURE);
				glBindTexture (GL_TEXTURE_2D, self->shadow_texture);
			}
		}
	}
}

void liren_context_set_material (
	LIRenContext*        self,
	const LIRenMaterial* value)
{
	liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_DIFFUSE, value->diffuse);
	liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_PARAM0, value->parameters);
	liren_uniforms_set_float (&self->uniforms, LIREN_UNIFORM_MATERIAL_SHININESS, value->shininess);
	liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_SPECULAR, value->specular);

	/* Set material flags. */
	if (value->flags & LIREN_MATERIAL_FLAG_CULLFACE)
		liren_context_set_cull (self, 1, GL_CCW);
	else
		liren_context_set_cull (self, 0, GL_CCW);
}

void liren_context_set_mesh (
	LIRenContext* self,
	LIRenMesh*    mesh)
{
	if (self->array != mesh->arrays[1])
	{
		self->array = mesh->arrays[1];
		self->changed.buffer = 1;
	}
}

void liren_context_set_modelmatrix (
	LIRenContext*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.model, value, sizeof (LIMatMatrix)))
	{
		self->matrix.model = *value;
		self->matrix.modelview = limat_matrix_multiply (self->matrix.view, self->matrix.model);
		self->matrix.modelviewinverse = limat_matrix_invert (self->matrix.modelview);
		liren_uniforms_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW, self->matrix.modelview.m);
		liren_uniforms_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW_INVERSE, self->matrix.modelviewinverse.m);
		liren_uniforms_set_mat3 (&self->uniforms, LIREN_UNIFORM_MATRIX_NORMAL, self->matrix.modelview.m);
	}
}

void liren_context_set_param (
	LIRenContext* self,
	const float*  value)
{
	liren_uniforms_set_vec4 (&self->uniforms, LIREN_UNIFORM_MATERIAL_PARAM0, value);
}

void liren_context_set_viewmatrix (
	LIRenContext*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.view, value, sizeof (LIMatMatrix)))
	{
		self->matrix.view = *value;
		self->matrix.modelview = limat_matrix_multiply (self->matrix.view, self->matrix.model);
		self->matrix.modelviewinverse = limat_matrix_invert (self->matrix.modelview);
		liren_uniforms_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW, self->matrix.modelview.m);
		liren_uniforms_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_MODELVIEW_INVERSE, self->matrix.modelviewinverse.m);
		liren_uniforms_set_mat3 (&self->uniforms, LIREN_UNIFORM_MATRIX_NORMAL, self->matrix.modelview.m);
	}
}

void liren_context_set_projection (
	LIRenContext*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.projection, value, sizeof (LIMatMatrix)))
	{
		self->matrix.projection = *value;
		self->matrix.projectioninverse = limat_matrix_invert (self->matrix.projection);
		liren_uniforms_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_PROJECTION, self->matrix.projection.m);
		liren_uniforms_set_mat4 (&self->uniforms, LIREN_UNIFORM_MATRIX_PROJECTION_INVERSE, self->matrix.projectioninverse.m);
	}
}

void liren_context_set_scene (
	LIRenContext* self,
	LIRenScene*   scene)
{
	self->scene = scene;
	/* FIXME */
	self->textures.count = 0;
	self->light = NULL;
}

void liren_context_set_shader (
	LIRenContext* self,
	int           pass,
	LIRenShader*  value)
{
	if (pass != self->shader_pass || value != self->shader)
	{
		self->shader_pass = pass;
		self->shader = value;
		self->changed.shader = 1;
	}
}

void liren_context_set_textures (
	LIRenContext* self,
	LIRenTexture* value,
	int           count)
{
	int c;
	int i;
	LIRenContextTexture* texture;

	c = LIMAT_MIN (count, 4);
	for (i = 0 ; i < c ; i++)
	{
		texture = self->textures.array + i;
		if (texture->texture != value[i].texture)
		{
			texture->texture = value[i].texture;
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (GL_TEXTURE_2D, texture->texture);
		}
	}
	for ( ; i < 4 ; i++)
	{
		texture = self->textures.array + i;
		if (texture->texture != self->render->helpers.empty_image->texture->texture)
		{
			texture->texture = self->render->helpers.empty_image->texture->texture;
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (GL_TEXTURE_2D, texture->texture);
		}
	}
}

void liren_context_set_textures_raw (
	LIRenContext* self,
	GLuint*       value,
	int           count)
{
	int c;
	int i;
	LIRenContextTexture* texture;

	c = LIMAT_MIN (count, 4);
	for (i = 0 ; i < c ; i++)
	{
		texture = self->textures.array + i;
		if (texture->texture != value[i])
		{
			texture->texture = value[i];
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (GL_TEXTURE_2D, texture->texture);
		}
	}
	for ( ; i < 4 ; i++)
	{
		texture = self->textures.array + i;
		if (texture->texture != self->render->helpers.empty_image->texture->texture)
		{
			texture->texture = self->render->helpers.empty_image->texture->texture;
			glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_DIFFUSE_TEXTURE_0 + i);
			glBindTexture (GL_TEXTURE_2D, texture->texture);
		}
	}
}

/** @} */
/** @} */
