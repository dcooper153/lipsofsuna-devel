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

static void private_bind_uniform (
	LIRenContext* self,
	LIRenUniform* uniform);

/*****************************************************************************/

void liren_context_init (
	LIRenContext* self)
{
	LIRenRender* render;

	render = self->render;
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
	self->changed.lights = 1;
	self->changed.material = 1;
	self->changed.matrix_model = 1;
	self->changed.matrix_projection = 1;
	self->changed.matrix_view = 1;
	self->changed.shader = 1;
	self->changed.textures = 1;
	self->changed.uniforms = 1;
	self->cull.enable = 1;
	self->cull.front_face = GL_CCW;
	self->depth.enable_test = 1;
	self->depth.enable_write = 1;
	self->depth.depth_func = GL_LEQUAL;
	self->material.shininess = 1.0f;
	self->material.diffuse[0] = 1.0f;
	self->material.diffuse[1] = 1.0f;
	self->material.diffuse[2] = 1.0f;
	self->material.diffuse[3] = 1.0f;
	self->matrix.model = limat_matrix_identity ();
	self->matrix.modelview = limat_matrix_identity ();
	self->matrix.modelviewinverse = limat_matrix_identity ();
	self->matrix.projection = limat_matrix_identity ();
	self->matrix.projectioninverse = limat_matrix_identity ();
	self->matrix.view = limat_matrix_identity ();
	self->textures.count = 0;
}

void liren_context_bind (
	LIRenContext* self)
{
	int i;

	/* Update matrices. */
	if (self->changed.matrix_model || self->changed.matrix_view)
	{
		self->matrix.modelview = limat_matrix_multiply (self->matrix.view, self->matrix.model);
		self->matrix.modelviewinverse = limat_matrix_invert (self->matrix.modelview);
	}
	if (self->changed.matrix_projection)
		self->matrix.projectioninverse = limat_matrix_invert (self->matrix.projection);

	/* Bind shader. */
	if (self->changed.shader)
	{
		if (self->shader != NULL)
			glUseProgram (self->shader->program);
		else
			glUseProgram (0);
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

	/* Bind uniforms. */
	if (self->shader != NULL)
	{
		self->incomplete = 0;
		if (self->changed.uniforms)
		{
			for (i = 0 ; i < self->shader->uniforms.count ; i++)
				private_bind_uniform (self, self->shader->uniforms.array + i);
		}
	}
	else
		self->incomplete = 1;

	/* All state changes were applied. */
	self->changed.blend = 0;
	self->changed.buffer = 0;
	self->changed.cull = 0;
	self->changed.depth = 0;
	self->changed.lights = 0;
	self->changed.material = 0;
	self->changed.matrix_model = 0;
	self->changed.matrix_projection = 0;
	self->changed.matrix_view = 0;
	self->changed.shader = 0;
	self->changed.textures = 0;
	self->changed.uniforms = 0;
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
		self->changed.uniforms = 1;
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
	if (memcpy (self->material.diffuse, value, 4 * sizeof (float)))
	{
		memcpy (self->material.diffuse, value, 4 * sizeof (float));
		self->changed.material = 1;
		self->changed.uniforms = 1;
	}
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
	if (self->light != value)
	{
		self->light = value;
		self->changed.lights = 1;
		self->changed.uniforms = 1;
	}
}

void liren_context_set_material (
	LIRenContext*        self,
	const LIRenMaterial* value)
{
	/* Set material parameters. */
	if (self->material.shininess != value->shininess ||
	    memcmp (self->material.parameters, value->parameters, 4 * sizeof (float)) ||
	    memcmp (self->material.diffuse, value->diffuse, 4 * sizeof (float)) ||
	    memcmp (self->material.specular, value->specular, 4 * sizeof (float)))
	{
		self->material.shininess = value->shininess;
		memcpy (self->material.parameters, value->parameters, 4 * sizeof (float));
		memcpy (self->material.diffuse, value->diffuse, 4 * sizeof (float));
		memcpy (self->material.specular, value->specular, 4 * sizeof (float));
		self->changed.material = 1;
		self->changed.uniforms = 1;
	}

	/* Set material flags. */
	if (value->flags & LIREN_MATERIAL_FLAG_CULLFACE)
		liren_context_set_cull (self, 1, GL_CCW);
	else
		liren_context_set_cull (self, 0, GL_CCW);
}

void liren_context_set_material_shader (
	LIRenContext*        self,
	const LIRenMaterial* value)
{
	if (self->deferred)
		liren_context_set_shader (self, value->shader_deferred);
	else
		liren_context_set_shader (self, value->shader_forward);
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
		self->changed.matrix_model = 1;
		self->changed.uniforms = 1;
	}
}

void liren_context_set_param (
	LIRenContext* self,
	const float*  value)
{
	if (memcmp (self->material.parameters, value, 4 * sizeof (float)))
	{
		self->changed.material = 1;
		memcpy (self->material.parameters, value, 4 * sizeof (float));
	}
}

void liren_context_set_viewmatrix (
	LIRenContext*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.view, value, sizeof (LIMatMatrix)))
	{
		self->matrix.view = *value;
		self->changed.matrix_view = 1;
		self->changed.uniforms = 1;
	}
}

void liren_context_set_projection (
	LIRenContext*      self,
	const LIMatMatrix* value)
{
	if (memcmp (&self->matrix.projection, value, sizeof (LIMatMatrix)))
	{
		self->matrix.projection = *value;
		self->changed.matrix_projection = 1;
		self->changed.uniforms = 1;
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
	self->changed.uniforms = 1;
}

void liren_context_set_shader (
	LIRenContext* self,
	LIRenShader*  value)
{
	if (value != self->shader)
	{
		self->shader = value;
		self->changed.lights = 1;
		self->changed.material = 1;
		self->changed.matrix_model = 1;
		self->changed.matrix_projection = 1;
		self->changed.matrix_view = 1;
		self->changed.shader = 1;
		self->changed.textures = 1;
		self->changed.uniforms = 1;
	}
}

void liren_context_set_textures (
	LIRenContext* self,
	LIRenTexture* value,
	int           count)
{
	int c;
	int i;

	c = LIMAT_MIN (count, 9);
	if (self->textures.count == c)
	{
		for (i = 0 ; i < c ; i++)
		{
			if (self->textures.array[i].texture != value[i].texture)
				break;
		}
		if (i == c)
			return;
	}

	for (i = 0 ; i < c ; i++)
		self->textures.array[i].texture = value[i].texture;
	self->textures.count = c;
	self->changed.textures = 1;
	self->changed.uniforms = 1;
}

void liren_context_set_textures_raw (
	LIRenContext* self,
	GLuint*       value,
	int           count)
{
	int c;
	int i;

	c = LIMAT_MIN (count, 9);
	if (self->textures.count == c)
	{
		for (i = 0 ; i < c ; i++)
		{
			if (self->textures.array[i].texture != value[i])
				break;
		}
		if (i == c)
			return;
	}

	for (i = 0 ; i < c ; i++)
		self->textures.array[i].texture = value[i];
	self->textures.count = c;
	self->changed.textures = 1;
	self->changed.uniforms = 1;
}

/*****************************************************************************/

static void private_bind_uniform (
	LIRenContext* self,
	LIRenUniform* uniform)
{
	int index;
	GLint map;
	GLfloat mat3[9];
	LIRenContextTexture* texture;
	LIMatVector vector;
	LIMatMatrix matrix;
	LIMatMatrix bias =
	{{
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	}};

	switch (uniform->value)
	{
		case LIREN_UNIFORM_NONE:
			break;
		case LIREN_UNIFORM_CUBETEXTURE0:
		case LIREN_UNIFORM_CUBETEXTURE1:
		case LIREN_UNIFORM_CUBETEXTURE2:
		case LIREN_UNIFORM_CUBETEXTURE3:
		case LIREN_UNIFORM_CUBETEXTURE4:
		case LIREN_UNIFORM_CUBETEXTURE5:
		case LIREN_UNIFORM_CUBETEXTURE6:
		case LIREN_UNIFORM_CUBETEXTURE7:
		case LIREN_UNIFORM_CUBETEXTURE8:
		case LIREN_UNIFORM_CUBETEXTURE9:
			if (self->changed.textures)
			{
				index = uniform->value - LIREN_UNIFORM_CUBETEXTURE0;
				if (index < self->textures.count)
				{
					texture = self->textures.array + index;
					glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
					glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, texture->texture);
				}
				else
				{
					glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
					glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, 0);
				}
			}
			break;
		case LIREN_UNIFORM_DIFFUSETEXTURE0:
		case LIREN_UNIFORM_DIFFUSETEXTURE1:
		case LIREN_UNIFORM_DIFFUSETEXTURE2:
		case LIREN_UNIFORM_DIFFUSETEXTURE3:
		case LIREN_UNIFORM_DIFFUSETEXTURE4:
		case LIREN_UNIFORM_DIFFUSETEXTURE5:
		case LIREN_UNIFORM_DIFFUSETEXTURE6:
		case LIREN_UNIFORM_DIFFUSETEXTURE7:
		case LIREN_UNIFORM_DIFFUSETEXTURE8:
		case LIREN_UNIFORM_DIFFUSETEXTURE9:
			if (self->changed.textures)
			{
				index = uniform->value - LIREN_UNIFORM_DIFFUSETEXTURE0;
				if (index < self->textures.count)
				{
					texture = self->textures.array + index;
					glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
					glBindTexture (GL_TEXTURE_2D, texture->texture);
				}
				else
				{
					glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
					glBindTexture (GL_TEXTURE_2D, self->render->helpers.empty_image->texture->texture);
				}
				glActiveTextureARB (GL_TEXTURE0);
			}
			break;
		case LIREN_UNIFORM_LIGHTAMBIENT0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
					glUniform4fv (uniform->binding, 1, self->light->ambient);
				else
					glUniform4f (uniform->binding, 0.0f, 0.0f, 0.0f, 1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTDIFFUSE0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
					glUniform4fv (uniform->binding, 1, self->light->diffuse);
				else
					glUniform4f (uniform->binding, 1.0f, 1.0f, 1.0f, 1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTDIRECTION0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
				{
					liren_light_get_direction (self->light, &vector);
					glUniform3f (uniform->binding, vector.x, vector.y, vector.z);
				}
				else
					glUniform3f (uniform->binding, 0.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
				{
					matrix = limat_matrix_get_rotation (self->matrix.modelview);
					liren_light_get_direction (self->light, &vector);
					vector = limat_matrix_transform (matrix, vector);
					glUniform3f (uniform->binding, vector.x, vector.y, vector.z);
				}
				else
					glUniform3f (uniform->binding, 0.0f, 0.0f, -1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTEQUATION0:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTEQUATION0;
				if (self->light != NULL)
					glUniform3fv (uniform->binding, 1, self->light->equation);
				else
					glUniform3f (uniform->binding, 1.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTMATRIX0:
			if (self->changed.lights || self->changed.matrix_model || self->changed.matrix_view)
			{
				if (self->light != NULL)
				{
					matrix = limat_matrix_multiply (bias, self->light->projection);
					matrix = limat_matrix_multiply (matrix, self->light->modelview);
					matrix = limat_matrix_multiply (matrix, self->matrix.modelviewinverse);
					glUniformMatrix4fv (uniform->binding, 1, GL_FALSE, matrix.m);
				}
				else
					glUniformMatrix4fv (uniform->binding, 1, GL_FALSE, bias.m);
			}
			break;
		case LIREN_UNIFORM_LIGHTPOSITION0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
				{
					glUniform3f (uniform->binding,
						self->light->transform.position.x,
						self->light->transform.position.y,
						self->light->transform.position.z);
				}
				else
					glUniform3f (uniform->binding, 0.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
				{
					vector = limat_matrix_transform (self->matrix.view, self->light->transform.position);
					glUniform3f (uniform->binding, vector.x, vector.y, vector.z);
				}
				else
					glUniform3f (uniform->binding, 0.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTSPECULAR0:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTSPECULAR0;
				if (self->light != NULL)
					glUniform4fv (uniform->binding, 1, self->light->specular);
				else
					glUniform4f (uniform->binding, 1.0f, 1.0f, 1.0f, 1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTSPOT0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
					glUniform3f (uniform->binding, self->light->cutoff, cos (self->light->cutoff), self->light->exponent);
				else
					glUniform3f (uniform->binding, M_PI, -1.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTTYPE0:
			if (self->changed.lights)
			{
				if (self->light != NULL)
					glUniform1i (uniform->binding, liren_light_get_type (self->light));
				else
					glUniform1i (uniform->binding, LIREN_UNIFORM_LIGHTTYPE_DISABLED);
			}
			break;
		case LIREN_UNIFORM_MATERIALDIFFUSE:
			if (self->changed.material)
				glUniform4fvARB (uniform->binding, 1, self->material.diffuse);
			break;
		case LIREN_UNIFORM_MATERIALSHININESS:
			if (self->changed.material)
				glUniform1fARB (uniform->binding, LIMAT_CLAMP (self->material.shininess, 1.0f, 127.0f));
			break;
		case LIREN_UNIFORM_MATERIALSPECULAR:
			if (self->changed.material)
				glUniform4fvARB (uniform->binding, 1, self->material.specular);
			break;
		case LIREN_UNIFORM_MATRIXMODEL:
			if (self->changed.matrix_model)
				glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->matrix.model.m);
			break;
		case LIREN_UNIFORM_MATRIXMODELVIEW:
			if (self->changed.matrix_model || self->changed.matrix_view)
				glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->matrix.modelview.m);
			break;
		case LIREN_UNIFORM_MATRIXMODELVIEWINVERSE:
			if (self->changed.matrix_model || self->changed.matrix_view)
				glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->matrix.modelviewinverse.m);
			break;
		case LIREN_UNIFORM_MATRIXNORMAL:
			if (self->changed.matrix_model || self->changed.matrix_view)
			{
				mat3[0] = self->matrix.modelview.m[0];
				mat3[1] = self->matrix.modelview.m[1];
				mat3[2] = self->matrix.modelview.m[2];
				mat3[3] = self->matrix.modelview.m[4];
				mat3[4] = self->matrix.modelview.m[5];
				mat3[5] = self->matrix.modelview.m[6];
				mat3[6] = self->matrix.modelview.m[8];
				mat3[7] = self->matrix.modelview.m[9];
				mat3[8] = self->matrix.modelview.m[10];
				glUniformMatrix3fvARB (uniform->binding, 1, GL_FALSE, mat3);
			}
			break;
		case LIREN_UNIFORM_MATRIXPROJECTION:
			if (self->changed.matrix_projection)
				glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->matrix.projection.m);
			break;
		case LIREN_UNIFORM_MATRIXPROJECTIONINVERSE:
			if (self->changed.matrix_projection)
				glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->matrix.projectioninverse.m);
			break;
		case LIREN_UNIFORM_NOISETEXTURE:
			if (self->changed.shader)
			{
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_2D, self->render->helpers.noise);
			}
			break;
		case LIREN_UNIFORM_PARAM0:
			if (self->changed.material)
				glUniform4fvARB (uniform->binding, 1, self->material.parameters);
			break;
		case LIREN_UNIFORM_SHADOWTEXTURE0:
			if (self->changed.lights)
			{
				map = self->render->helpers.depth_texture_max;
				if (self->light != NULL)
				{
					if (self->light->shadow.map)
						map = self->light->shadow.map;
				}
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_2D, map);
			}
			break;
		case LIREN_UNIFORM_TIME:
			if (self->changed.shader)
				glUniform1fARB (uniform->binding, self->render->helpers.time);
			break;
	}
}

/** @} */
/** @} */
