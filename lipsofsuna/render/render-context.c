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

static void
private_bind_lights_shader (LIRenContext* self);

static void
private_bind_material (LIRenContext* self);

static void
private_bind_uniform (LIRenContext* self,
                      LIRenUniform* uniform);

static void
private_bind_vertices (LIRenContext*      self,
                       const LIRenFormat* format,
                       const void*        base);

static void
private_enable_light (LIRenContext* self,
                      int           i,
                      LIRenLight*   light);

static void
private_unbind_vertices (LIRenContext*      self,
                         const LIRenFormat* format);

/*****************************************************************************/

void
liren_context_bind (LIRenContext* self)
{
	int i;

	/* Validate state. */
	if (!self->compiled)
	{
		self->modelviewinverse = limat_matrix_invert (self->modelview);
		self->compiled = 1;
	}
	if (self->shader == NULL)
	{
		self->incomplete = 1;
		return;
	}
	self->incomplete = 0;

	/* Bind shader. */
	/*if (self->changed.shader)*/
		glUseProgramObjectARB (self->shader->program);

	/* Bind material. */
	private_bind_material (self);

	/* Bind matrices. */
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (self->projection.m);
	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (self->modelview.m);

	/* Bind lights. */
	private_bind_lights_shader (self);

	/* Bind uniforms. */
	for (i = 0 ; i < self->shader->uniforms.count ; i++)
		private_bind_uniform (self, self->shader->uniforms.array + i);
	liren_check_errors ();

	self->changed.shader = 0;
}

/**
 * \brief Renders non-indexed triangles.
 *
 * \param self Rendering context.
 * \param vertex Vertex buffer.
 */
void
liren_context_render_array (LIRenContext* self,
                            LIRenBuffer*  vertex)
{
	if (vertex->buffer)
	{
		liren_context_render_vbo_array (self, &vertex->format,
			vertex->buffer, 0, vertex->elements.count);
	}
	else
	{
		liren_context_render_vtx_array (self, &vertex->format,
			vertex->elements.array, 0, vertex->elements.count);
	}
}

/**
 * \brief Renders indexed triangles.
 *
 * \param self Rendering context.
 * \param vertex Vertex buffer.
 * \param index Index buffer.
 */
void
liren_context_render_indexed (LIRenContext* self,
                              LIRenBuffer*  vertex,
                              LIRenBuffer*  index)
{
	if (vertex->buffer)
	{
		liren_context_render_vbo_indexed (self, &vertex->format,
			vertex->buffer, index->buffer, 0, index->elements.count);
	}
	else
	{
		liren_context_render_vtx_indexed (self, &vertex->format,
			vertex->elements.array, index->elements.array, 0, index->elements.count);
	}
}

/**
 * \brief Renders triangles.
 *
 * \param self Rendering context.
 * \param format Vertex format.
 * \param vertices Vertex array.
 * \param vertex0 Vertex from which to begin rendering.
 * \param vertex1 Vertex to which to end rendering.
 */
void
liren_context_render_vbo_array (LIRenContext*      self,
                                const LIRenFormat* format,
                                GLuint             vertices,
                                int                vertex0,
                                int                vertex1)
{
	if (self->incomplete)
		return;

	glPushMatrix ();
	glMultMatrixf (self->matrix.m);

	glBindBufferARB (GL_ARRAY_BUFFER_ARB, vertices);
	private_bind_vertices (self, format, NULL);
	glDrawArraysEXT (GL_TRIANGLES, vertex0, vertex1 - vertex0);
	private_unbind_vertices (self, format);

	glPopMatrix ();

#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.materials++;
	self->render->profiling.faces += count;
	self->render->profiling.vertices += 3 * count;
#endif
}

/**
 * \brief Renders triangles.
 *
 * \param self Rendering context.
 * \param format Vertex format.
 * \param vertices Vertex array.
 * \param vertex0 Vertex from which to begin rendering.
 * \param vertex1 Vertex to which to end rendering.
 */
void
liren_context_render_vtx_array (LIRenContext*      self,
                                const LIRenFormat* format,
                                const void*        vertices,
                                int                vertex0,
                                int                vertex1)
{
	if (self->incomplete)
		return;

	glPushMatrix ();
	glMultMatrixf (self->matrix.m);

	private_bind_vertices (self, format, vertices);
	glDrawArraysEXT (GL_TRIANGLES, vertex0, vertex1 - vertex0);
	private_unbind_vertices (self, format);

	glPopMatrix ();

#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.materials++;
	self->render->profiling.faces += vertex1 - vertex0;
	self->render->profiling.vertices += 3 * (vertex1 - vertex0);
#endif
}

/**
 * \brief Renders indexed triangles.
 *
 * \param self Rendering context.
 * \param format Vertex format.
 * \param vertices Vertex buffer.
 * \param indices Index buffer.
 * \param index0 Index from which to begin rendering.
 * \param index1 Index to which to end rendering.
 */
void
liren_context_render_vbo_indexed (LIRenContext*      self,
                                  const LIRenFormat* format,
                                  GLuint             vertices,
                                  GLuint             indices,
                                  int                index0,
                                  int                index1)
{
	if (self->incomplete)
		return;

	glPushMatrix ();
	glMultMatrixf (self->matrix.m);

	glBindBufferARB (GL_ARRAY_BUFFER_ARB, vertices);
	glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, indices);
	private_bind_vertices (self, format, NULL);
	glDrawElements (GL_TRIANGLES, index1 - index0, GL_UNSIGNED_INT, NULL + 4 * index0);
	private_unbind_vertices (self, format);
	glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glPopMatrix ();

#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.materials++;
	self->render->profiling.faces += vertex1 - vertex0;
	self->render->profiling.vertices += 3 * (vertex1 - vertex0);
#endif
}

/**
 * \brief Renders indexed triangles.
 *
 * \param self Rendering context.
 * \param format Vertex format.
 * \param vertices Vertex array.
 * \param indices Index array.
 * \param index0 Index from which to begin rendering.
 * \param index1 Index to which to end rendering.
 */
void
liren_context_render_vtx_indexed (LIRenContext*      self,
                                  const LIRenFormat* format,
                                  const void*        vertices,
                                  const void*        indices,
                                  int                index0,
                                  int                index1)
{
	if (self->incomplete)
		return;

	glPushMatrix ();
	glMultMatrixf (self->matrix.m);

	private_bind_vertices (self, format, vertices);
	glDrawElements (GL_TRIANGLES, index1 - index0, GL_UNSIGNED_INT, indices + 4 * index0);
	private_unbind_vertices (self, format);

	glPopMatrix ();

#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.materials++;
	self->render->profiling.faces += vertex1 - vertex0;
	self->render->profiling.vertices += 3 * (vertex1 - vertex0);
#endif
}

/**
 * \brief Resets the OpenGL state variables used by contexts.
 *
 * \param self Rendering context.
 */
void
liren_context_unbind (LIRenContext* self)
{
	glUseProgramObjectARB (0);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glColor3f (1.0f, 1.0f, 1.0f);
}

int
liren_context_get_deferred (LIRenContext* self)
{
	return self->deferred;
}

void
liren_context_set_deferred (LIRenContext* self,
                            int           value)
{
	self->deferred = value;
}

void
liren_context_set_flags (LIRenContext* self,
                         int           value)
{
/*	self->lighting = ((value & LIREN_FLAG_LIGHTING) != 0);
	self->texturing = ((value & LIREN_FLAG_TEXTURING) != 0);*/
	self->shadows = ((value & LIREN_FLAG_SHADOW1) != 0);
}

void
liren_context_set_frustum (LIRenContext*       self,
                           const LIMatFrustum* frustum)
{
	self->frustum = *frustum;
}

void
liren_context_set_lights (LIRenContext* self,
                          LIRenLight**  value,
                          int           count)
{
	self->lights.array = value;
	self->lights.count = count;
}

void
liren_context_set_material (LIRenContext*        self,
                            const LIRenMaterial* value)
{
	self->material.flags = value->flags;
	self->material.shininess = value->shininess;
	memcpy (self->material.parameters, value->parameters, 4 * sizeof (float));
	memcpy (self->material.diffuse, value->diffuse, 4 * sizeof (float));
	memcpy (self->material.specular, value->specular, 4 * sizeof (float));
}

void
liren_context_set_material_shader (LIRenContext*        self,
                                   const LIRenMaterial* value)
{
	if (self->deferred)
		liren_context_set_shader (self, value->shader_deferred);
	else
		liren_context_set_shader (self, value->shader_forward);
}

void
liren_context_set_matrix (LIRenContext*      self,
                          const LIMatMatrix* value)
{
	self->matrix = *value;
}

void
liren_context_set_modelview (LIRenContext*      self,
                             const LIMatMatrix* value)
{
	self->modelview = *value;
	self->compiled = 0;
}

void
liren_context_set_projection (LIRenContext*      self,
                              const LIMatMatrix* value)
{
	self->projection = *value;
}

void
liren_context_set_scene (LIRenContext* self,
                         LIRenScene*   scene)
{
	self->scene = scene;
	/* FIXME */
	self->textures.count = 0;
	self->lights.count = 0;
}

void
liren_context_set_shader (LIRenContext* self,
                          LIRenShader*  value)
{
	if (value != self->shader)
	{
		self->shader = value;
		self->changed.shader = 1;
	}
}

void
liren_context_set_textures (LIRenContext* self,
                            LIRenTexture* value,
                            int           count)
{
	self->textures.array = value;
	self->textures.count = count;
}

/*****************************************************************************/

static void
private_bind_lights_shader (LIRenContext* self)
{
	int i;
	int count;
	const GLfloat none[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	LIRenLight* light;

	count = LIMAT_MIN (self->lights.count, self->shader->lights.count);
	for (i = 0 ; i < count ; i++)
	{
		light = self->lights.array[i];
		private_enable_light (self, i, light);
	}
	for ( ; i < self->shader->lights.count ; i++)
	{
		glLightfv (GL_LIGHT0 + i, GL_POSITION, none);
		glLightfv (GL_LIGHT0 + i, GL_SPOT_DIRECTION, none);
		glLightf (GL_LIGHT0 + i, GL_SPOT_CUTOFF, 0.0f);
		glLightf (GL_LIGHT0 + i, GL_SPOT_EXPONENT, 0.0f);
		glLightfv (GL_LIGHT0 + i, GL_AMBIENT, none);
		glLightfv (GL_LIGHT0 + i, GL_DIFFUSE, none);
		glLightfv (GL_LIGHT0 + i, GL_SPECULAR, none);
	}
}

static void
private_bind_material (LIRenContext* self)
{
	/* TODO: Billboard support. */
	if (self->material.flags & LIREN_MATERIAL_FLAG_CULLFACE)
		glEnable (GL_CULL_FACE);
	else
		glDisable (GL_CULL_FACE);
	if (!self->deferred && (self->material.flags & LIREN_MATERIAL_FLAG_TRANSPARENCY))
	{
		glEnable (GL_BLEND);
		glDepthMask (GL_FALSE);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable (GL_BLEND);
		glDepthMask (GL_TRUE);
	}
}

static void
private_bind_uniform (LIRenContext* self,
                      LIRenUniform* uniform)
{
	int index;
	GLint map;
	LIRenLight* light;
	LIRenTexture* texture;
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
			index = uniform->value - LIREN_UNIFORM_CUBETEXTURE0;
			if (index < self->textures.count)
			{
				texture = self->textures.array + index;
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, texture->texture);
/*				glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texture->params.minfilter);
				glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texture->params.minfilter);
				glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texture->params.wraps);
				glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texture->params.wrapt);*/
			}
			else
			{
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, 0);
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
			index = uniform->value - LIREN_UNIFORM_DIFFUSETEXTURE0;
			if (index < self->textures.count)
			{
				texture = self->textures.array + index;
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_2D, texture->texture);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->params.magfilter);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->params.minfilter);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->params.wraps);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->params.wrapt);
			}
			else
			{
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_2D, self->render->helpers.empty_image->texture->texture);
			}
			glActiveTextureARB (GL_TEXTURE0);
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
			index = uniform->value - LIREN_UNIFORM_LIGHTTYPE0;
			if (index < self->lights.count)
			{
				light = self->lights.array[index];
				if (light->directional)
					glUniform1iARB (uniform->binding, LIREN_UNIFORM_LIGHTTYPE_DIRECTIONAL);
				else if (LIMAT_ABS (light->cutoff - M_PI) < 0.001)
					glUniform1iARB (uniform->binding, LIREN_UNIFORM_LIGHTTYPE_POINT);
				else if (light->shadow.map)
					glUniform1iARB (uniform->binding, LIREN_UNIFORM_LIGHTTYPE_SPOTSHADOW);
				else
					glUniform1iARB (uniform->binding, LIREN_UNIFORM_LIGHTTYPE_SPOT);
			}
			else
				glUniform1iARB (uniform->binding, LIREN_UNIFORM_LIGHTTYPE_DISABLED);
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
			index = uniform->value - LIREN_UNIFORM_LIGHTMATRIX0;
			if (index < self->lights.count)
			{
				light = self->lights.array[index];
				matrix = limat_matrix_multiply (bias, light->projection);
				matrix = limat_matrix_multiply (matrix, light->modelview);
				matrix = limat_matrix_multiply (matrix, self->modelviewinverse);
				glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, matrix.m);
			}
			else
				glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, bias.m);
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
			break;
		case LIREN_UNIFORM_MATERIALDIFFUSE:
			glUniform4fvARB (uniform->binding, 1, self->material.diffuse);
			break;
		case LIREN_UNIFORM_MATERIALSHININESS:
			glUniform1fARB (uniform->binding,
				LIMAT_CLAMP (self->material.shininess, 1.0f, 127.0f));
			break;
		case LIREN_UNIFORM_MATERIALSPECULAR:
			glUniform4fvARB (uniform->binding, 1, self->material.specular);
			break;
		case LIREN_UNIFORM_MODELMATRIX:
			glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->matrix.m);
			break;
		case LIREN_UNIFORM_MODELVIEWINVERSE:
			glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->modelviewinverse.m);
			break;
		case LIREN_UNIFORM_NOISETEXTURE:
			glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
			glBindTexture (GL_TEXTURE_2D, self->render->helpers.noise);
			break;
		case LIREN_UNIFORM_PARAM0:
			glUniform4fARB (uniform->binding,
				self->material.parameters[0],
				self->material.parameters[1],
				self->material.parameters[2],
				self->material.parameters[3]);
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
			break;
		case LIREN_UNIFORM_TIME:
			glUniform1fARB (uniform->binding, self->render->helpers.time);
			break;
	}
}

static void
private_bind_vertices (LIRenContext*      self,
                       const LIRenFormat* format,
                       const void*        base)
{
	int i;
	LIRenAttribute* attr;

	if (self->shader != NULL)
	{
		for (i = 0 ; i < self->shader->attributes.count ; i++)
		{
			attr = self->shader->attributes.array + i;
			switch (attr->value)
			{
				case LIREN_ATTRIBUTE_COORD:
					glEnableVertexAttribArrayARB (attr->binding);
					glVertexAttribPointerARB (attr->binding, 3, format->vtx_format,
						GL_FALSE, format->size, base + format->vtx_offset);
					break;
				case LIREN_ATTRIBUTE_NORMAL:
					glEnableVertexAttribArrayARB (attr->binding);
					glVertexAttribPointerARB (attr->binding, 3, format->nml_format,
						GL_FALSE, format->size, base + format->nml_offset);
					break;
				case LIREN_ATTRIBUTE_TANGENT:
					glEnableVertexAttribArrayARB (attr->binding);
					glVertexAttribPointerARB (attr->binding, 3, format->tan_format,
						GL_FALSE, format->size, base + format->tan_offset);
					break;
				case LIREN_ATTRIBUTE_TEXCOORD:
					glEnableVertexAttribArrayARB (attr->binding);
					glVertexAttribPointerARB (attr->binding, 2, format->tex_format,
						GL_FALSE, format->size, base + format->tex_offset);
					break;
			}
		}
		glEnableClientState (GL_VERTEX_ARRAY);
		glVertexPointerEXT (3, format->vtx_format, format->size, 0, base + format->vtx_offset);
	}
	else
	{
		glEnableClientState (GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer (2, format->tex_format, format->size, base + format->tex_offset);
		glEnableClientState (GL_NORMAL_ARRAY);
		glNormalPointerEXT (format->nml_format, format->size, 0, base + format->nml_offset);
		glEnableClientState (GL_VERTEX_ARRAY);
		glVertexPointerEXT (3, format->vtx_format, format->size, 0, base + format->vtx_offset);
	}
}

static void
private_enable_light (LIRenContext* self,
                      int           i,
                      LIRenLight*   light)
{
	LIMatVector tmp;
	GLfloat position[4];
	GLfloat direction[3];

	/* Calculate orientation. */
	liren_light_get_direction (light, &tmp);
	direction[0] = tmp.x;
	direction[1] = tmp.y;
	direction[2] = tmp.z;
	if (light->directional)
	{
		position[0] = -tmp.x;
		position[1] = -tmp.y;
		position[2] = -tmp.z;
		position[3] = 0.0f;
	}
	else
	{
		tmp = light->transform.position;
		position[0] = tmp.x;
		position[1] = tmp.y;
		position[2] = tmp.z;
		position[3] = 1.0f;
	}

	/* Set standard parameters. */
	glLightfv (GL_LIGHT0 + i, GL_POSITION, position);
	glLightfv (GL_LIGHT0 + i, GL_SPOT_DIRECTION, direction);
	glLightf (GL_LIGHT0 + i, GL_SPOT_CUTOFF, light->cutoff / M_PI * 180.0f);
	glLightf (GL_LIGHT0 + i, GL_SPOT_EXPONENT, light->exponent);
	glLightfv (GL_LIGHT0 + i, GL_DIFFUSE, light->diffuse);
	glLightfv (GL_LIGHT0 + i, GL_SPECULAR, light->specular);
	glLightfv (GL_LIGHT0 + i, GL_AMBIENT, light->ambient);
	glLightf (GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, light->equation[0]);
	glLightf (GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, light->equation[1]);
	glLightf (GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, light->equation[2]);
}

static void
private_unbind_vertices (LIRenContext*      self,
                         const LIRenFormat* format)
{
	int i;
	LIRenAttribute* attr;

	if (self->shader != NULL)
	{
		for (i = 0 ; i < self->shader->attributes.count ; i++)
		{
			attr = self->shader->attributes.array + i;
			switch (attr->value)
			{
				case LIREN_ATTRIBUTE_COORD:
				case LIREN_ATTRIBUTE_NORMAL:
				case LIREN_ATTRIBUTE_TANGENT:
				case LIREN_ATTRIBUTE_TEXCOORD:
					glDisableVertexAttribArrayARB (attr->binding);
					break;
			}
		}
		glDisableClientState (GL_VERTEX_ARRAY);
	}
	else
	{
		glDisableClientState (GL_TEXTURE_COORD_ARRAY);
		glDisableClientState (GL_NORMAL_ARRAY);
		glDisableClientState (GL_VERTEX_ARRAY);
	}
}

/** @} */
/** @} */
