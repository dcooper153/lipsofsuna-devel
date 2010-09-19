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
	self->buffer = NULL;
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
	self->lights.count = 0;
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
	{
		if (self->buffer != NULL)
			glBindVertexArray (self->buffer->vertex_array);
		else
			glBindVertexArray (0);
	}

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
	lisys_assert (start + count <= self->buffer->indices.count);

	glDrawRangeElements (GL_TRIANGLES, 0, self->buffer->vertices.count,
		count, GL_UNSIGNED_INT, NULL + start * sizeof (uint32_t));

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
	if (self->buffer != buffer)
	{
		self->buffer = buffer;
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

void liren_context_set_lights (
	LIRenContext* self,
	LIRenLight**  value,
	int           count)
{
	int i;
	int c;

	c = LIMAT_MIN (count, 9);
	if (self->lights.count == c)
	{
		for (i = 0 ; i < c ; i++)
		{
			if (memcmp (self->lights.array[i], value[i], sizeof (LIRenLight)))
				break;
		}
		if (i == c)
			return;
	}

	memcpy (self->lights.array, value, c * sizeof (LIRenLight*));
	self->lights.count = c;
	self->changed.lights = 1;
	self->changed.uniforms = 1;
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
	if (value->flags & LIREN_MATERIAL_FLAG_TRANSPARENCY)
	{
		liren_context_set_blend (self, 1, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		liren_context_set_depth (self, 1, 0, GL_LEQUAL);
	}
	else
	{
		liren_context_set_blend (self, 0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		liren_context_set_depth (self, 1, 1, GL_LEQUAL);
	}
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
	self->lights.count = 0;
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
			if (self->textures.array[i].texture != value[i].texture ||
			    self->textures.array[i].sampler != value[i].sampler)
				break;
		}
		if (i == c)
			return;
	}

	for (i = 0 ; i < c ; i++)
	{
		self->textures.array[i].texture = value[i].texture;
		self->textures.array[i].sampler = value[i].sampler;
	}
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
	{
		self->textures.array[i].texture = value[i];
		self->textures.array[i].sampler = 0;
	}
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
	LIRenLight* light;
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
/*					glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texture->magfilter);
					glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texture->minfilter);
					glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texture->wraps);
					glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texture->wrapt);*/
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
					glBindSampler (uniform->sampler, texture->sampler);
				}
				else
				{
					glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
					glBindTexture (GL_TEXTURE_2D, self->render->helpers.empty_image->texture->texture);
					glBindSampler (uniform->sampler, 0);
				}
				glActiveTextureARB (GL_TEXTURE0);
			}
			break;
		case LIREN_UNIFORM_LIGHTAMBIENT0:
		case LIREN_UNIFORM_LIGHTAMBIENT1:
		case LIREN_UNIFORM_LIGHTAMBIENT2:
		case LIREN_UNIFORM_LIGHTAMBIENT3:
		case LIREN_UNIFORM_LIGHTAMBIENT4:
		case LIREN_UNIFORM_LIGHTAMBIENT5:
		case LIREN_UNIFORM_LIGHTAMBIENT6:
		case LIREN_UNIFORM_LIGHTAMBIENT7:
		case LIREN_UNIFORM_LIGHTAMBIENT8:
		case LIREN_UNIFORM_LIGHTAMBIENT9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTAMBIENT0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					glUniform4fvARB (uniform->binding, 1, light->ambient);
				}
				else
					glUniform4fARB (uniform->binding, 0.0f, 0.0f, 0.0f, 1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTDIFFUSE0:
		case LIREN_UNIFORM_LIGHTDIFFUSE1:
		case LIREN_UNIFORM_LIGHTDIFFUSE2:
		case LIREN_UNIFORM_LIGHTDIFFUSE3:
		case LIREN_UNIFORM_LIGHTDIFFUSE4:
		case LIREN_UNIFORM_LIGHTDIFFUSE5:
		case LIREN_UNIFORM_LIGHTDIFFUSE6:
		case LIREN_UNIFORM_LIGHTDIFFUSE7:
		case LIREN_UNIFORM_LIGHTDIFFUSE8:
		case LIREN_UNIFORM_LIGHTDIFFUSE9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTDIFFUSE0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					glUniform4fvARB (uniform->binding, 1, light->diffuse);
				}
				else
					glUniform4fARB (uniform->binding, 1.0f, 1.0f, 1.0f, 1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTDIRECTION0:
		case LIREN_UNIFORM_LIGHTDIRECTION1:
		case LIREN_UNIFORM_LIGHTDIRECTION2:
		case LIREN_UNIFORM_LIGHTDIRECTION3:
		case LIREN_UNIFORM_LIGHTDIRECTION4:
		case LIREN_UNIFORM_LIGHTDIRECTION5:
		case LIREN_UNIFORM_LIGHTDIRECTION6:
		case LIREN_UNIFORM_LIGHTDIRECTION7:
		case LIREN_UNIFORM_LIGHTDIRECTION8:
		case LIREN_UNIFORM_LIGHTDIRECTION9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTDIRECTION0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					liren_light_get_direction (light, &vector);
					glUniform3fARB (uniform->binding, vector.x, vector.y, vector.z);
				}
				else
					glUniform3fARB (uniform->binding, 0.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT0:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT1:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT2:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT3:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT4:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT5:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT6:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT7:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT8:
		case LIREN_UNIFORM_LIGHTDIRECTIONPREMULT9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTDIRECTIONPREMULT0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					matrix = limat_matrix_get_rotation (self->matrix.modelview);
					liren_light_get_direction (light, &vector);
					vector = limat_matrix_transform (matrix, vector);
					glUniform3fARB (uniform->binding, vector.x, vector.y, vector.z);
				}
				else
					glUniform3fARB (uniform->binding, 0.0f, 0.0f, -1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTEQUATION0:
		case LIREN_UNIFORM_LIGHTEQUATION1:
		case LIREN_UNIFORM_LIGHTEQUATION2:
		case LIREN_UNIFORM_LIGHTEQUATION3:
		case LIREN_UNIFORM_LIGHTEQUATION4:
		case LIREN_UNIFORM_LIGHTEQUATION5:
		case LIREN_UNIFORM_LIGHTEQUATION6:
		case LIREN_UNIFORM_LIGHTEQUATION7:
		case LIREN_UNIFORM_LIGHTEQUATION8:
		case LIREN_UNIFORM_LIGHTEQUATION9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTEQUATION0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					glUniform3fvARB (uniform->binding, 1, light->equation);
				}
				else
					glUniform3fARB (uniform->binding, 1.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTMATRIX0:
		case LIREN_UNIFORM_LIGHTMATRIX1:
		case LIREN_UNIFORM_LIGHTMATRIX2:
		case LIREN_UNIFORM_LIGHTMATRIX3:
		case LIREN_UNIFORM_LIGHTMATRIX4:
		case LIREN_UNIFORM_LIGHTMATRIX5:
		case LIREN_UNIFORM_LIGHTMATRIX6:
		case LIREN_UNIFORM_LIGHTMATRIX7:
		case LIREN_UNIFORM_LIGHTMATRIX8:
		case LIREN_UNIFORM_LIGHTMATRIX9:
			if (self->changed.lights || self->changed.matrix_model || self->changed.matrix_view)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTMATRIX0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					matrix = limat_matrix_multiply (bias, light->projection);
					matrix = limat_matrix_multiply (matrix, light->modelview);
					matrix = limat_matrix_multiply (matrix, self->matrix.modelviewinverse);
					glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, matrix.m);
				}
				else
					glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, bias.m);
			}
			break;
		case LIREN_UNIFORM_LIGHTPOSITION0:
		case LIREN_UNIFORM_LIGHTPOSITION1:
		case LIREN_UNIFORM_LIGHTPOSITION2:
		case LIREN_UNIFORM_LIGHTPOSITION3:
		case LIREN_UNIFORM_LIGHTPOSITION4:
		case LIREN_UNIFORM_LIGHTPOSITION5:
		case LIREN_UNIFORM_LIGHTPOSITION6:
		case LIREN_UNIFORM_LIGHTPOSITION7:
		case LIREN_UNIFORM_LIGHTPOSITION8:
		case LIREN_UNIFORM_LIGHTPOSITION9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTPOSITION0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					glUniform3fARB (uniform->binding,
						light->transform.position.x,
						light->transform.position.y,
						light->transform.position.z);
				}
				else
					glUniform3fARB (uniform->binding, 0.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT0:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT1:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT2:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT3:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT4:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT5:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT6:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT7:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT8:
		case LIREN_UNIFORM_LIGHTPOSITIONPREMULT9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTPOSITIONPREMULT0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					vector = limat_matrix_transform (self->matrix.view, light->transform.position);
					glUniform3fARB (uniform->binding, vector.x, vector.y, vector.z);
				}
				else
					glUniform3fARB (uniform->binding, 0.0f, 0.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTSPECULAR0:
		case LIREN_UNIFORM_LIGHTSPECULAR1:
		case LIREN_UNIFORM_LIGHTSPECULAR2:
		case LIREN_UNIFORM_LIGHTSPECULAR3:
		case LIREN_UNIFORM_LIGHTSPECULAR4:
		case LIREN_UNIFORM_LIGHTSPECULAR5:
		case LIREN_UNIFORM_LIGHTSPECULAR6:
		case LIREN_UNIFORM_LIGHTSPECULAR7:
		case LIREN_UNIFORM_LIGHTSPECULAR8:
		case LIREN_UNIFORM_LIGHTSPECULAR9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTSPECULAR0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					glUniform4fvARB (uniform->binding, 1, light->specular);
				}
				else
					glUniform4fARB (uniform->binding, 1.0f, 1.0f, 1.0f, 1.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTSPOT0:
		case LIREN_UNIFORM_LIGHTSPOT1:
		case LIREN_UNIFORM_LIGHTSPOT2:
		case LIREN_UNIFORM_LIGHTSPOT3:
		case LIREN_UNIFORM_LIGHTSPOT4:
		case LIREN_UNIFORM_LIGHTSPOT5:
		case LIREN_UNIFORM_LIGHTSPOT6:
		case LIREN_UNIFORM_LIGHTSPOT7:
		case LIREN_UNIFORM_LIGHTSPOT8:
		case LIREN_UNIFORM_LIGHTSPOT9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTSPOT0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					glUniform3fARB (uniform->binding, light->cutoff, cos (light->cutoff), light->exponent);
				}
				else
					glUniform3fARB (uniform->binding, M_PI, -1.0f, 0.0f);
			}
			break;
		case LIREN_UNIFORM_LIGHTTYPE0:
		case LIREN_UNIFORM_LIGHTTYPE1:
		case LIREN_UNIFORM_LIGHTTYPE2:
		case LIREN_UNIFORM_LIGHTTYPE3:
		case LIREN_UNIFORM_LIGHTTYPE4:
		case LIREN_UNIFORM_LIGHTTYPE5:
		case LIREN_UNIFORM_LIGHTTYPE6:
		case LIREN_UNIFORM_LIGHTTYPE7:
		case LIREN_UNIFORM_LIGHTTYPE8:
		case LIREN_UNIFORM_LIGHTTYPE9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_LIGHTTYPE0;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					glUniform1iARB (uniform->binding, liren_light_get_type (light));
				}
				else
					glUniform1iARB (uniform->binding, LIREN_UNIFORM_LIGHTTYPE_DISABLED);
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
		case LIREN_UNIFORM_SHADOWTEXTURE1:
		case LIREN_UNIFORM_SHADOWTEXTURE2:
		case LIREN_UNIFORM_SHADOWTEXTURE3:
		case LIREN_UNIFORM_SHADOWTEXTURE4:
		case LIREN_UNIFORM_SHADOWTEXTURE5:
		case LIREN_UNIFORM_SHADOWTEXTURE6:
		case LIREN_UNIFORM_SHADOWTEXTURE7:
		case LIREN_UNIFORM_SHADOWTEXTURE8:
		case LIREN_UNIFORM_SHADOWTEXTURE9:
			if (self->changed.lights)
			{
				index = uniform->value - LIREN_UNIFORM_SHADOWTEXTURE0;
				map = self->render->helpers.depth_texture_max;
				if (index < self->lights.count)
				{
					light = self->lights.array[index];
					if (light->shadow.map)
						map = light->shadow.map;
				}
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_2D, map);
				glBindSampler (uniform->sampler, 0);
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
