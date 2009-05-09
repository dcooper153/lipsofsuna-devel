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
 * \addtogroup lirndContext Context
 * @{
 */

#include <string.h>
#include "render-context.h"

static void
private_bind_lights_fixed (lirndContext* self);

static void
private_bind_lights_shader (lirndContext* self);

static void
private_bind_material (lirndContext* self);

static void
private_bind_shader (lirndContext* self);

static void
private_bind_texture (lirndContext* self,
                      int           i,
                      lirndTexture* texture);

static void
private_bind_textures_fixed (lirndContext* self);

static void
private_bind_uniform (lirndContext* self,
                      lirndUniform* uniform);

static void
private_enable_light (lirndContext* self,
                      int           i,
                      lirndLight*   light);

/*****************************************************************************/

void
lirnd_context_init (lirndContext* self,
                    lirndRender*  render)
{
	memset (self, 0, sizeof (lirndContext));
	self->compiled = 1;
	self->render = render;
	self->matrix = limat_matrix_identity ();
	self->modelview = limat_matrix_identity ();
	self->modelviewinverse = limat_matrix_identity ();
	self->projection = limat_matrix_identity ();
//		limat_matrix_ortho (0.0f, 640.0f, 480.0f, 0.0f, -100.0f, 100.0f);
}

void
lirnd_context_bind (lirndContext* self)
{
	int i;

	if (!self->compiled)
	{
		self->modelviewinverse = limat_matrix_invert (self->modelview);
		self->compiled = 1;
	}
	if (!self->render->shader.enabled)
		self->fixed = 1;
	glMatrixMode (GL_TEXTURE);
	if (self->fixed || self->shader == NULL)
	{
		private_bind_lights_fixed (self);
		private_bind_material (self);
		if (livid_features.shader_model >= 3)
			glUseProgramObjectARB (0);
		private_bind_textures_fixed (self);
		glEnable (GL_LIGHTING);
	}
	else
	{
		private_bind_lights_shader (self);
		private_bind_material (self);
		private_bind_shader (self);
		for (i = 0 ; i < self->shader->uniforms.count ; i++)
			private_bind_uniform (self, self->shader->uniforms.array + i);
	}
	glMatrixMode (GL_MODELVIEW);
	glActiveTextureARB (GL_TEXTURE0);
}

/**
 * \brief Renders triangles.
 *
 * \param self Rendering context.
 * \param vertex0 Vertex from which to begin rendering.
 * \param vertex1 Vertex to which to end rendering.
 * \param vertices Vertex array.
 */
void
lirnd_context_render_buffer (lirndContext* self,
                             int           vertex0,
                             int           vertex1,
                             GLuint        vertices)
{
	int count;
	glColor3f (1.0f, 1.0f, 1.0f);

	glPushMatrix ();
	glMultMatrixf (self->matrix.m);
	glBindBufferARB (GL_ARRAY_BUFFER_ARB, vertices);

	/* Bind buffer. */
	count = vertex1 - vertex0;
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointerEXT (2, GL_FLOAT, 12 * sizeof (float), count, NULL);
	glClientActiveTextureARB (GL_TEXTURE1);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointerEXT (2, GL_FLOAT, 12 * sizeof (float), count, NULL + 2 * sizeof (float));
	glClientActiveTextureARB (GL_TEXTURE2);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointerEXT (2, GL_FLOAT, 12 * sizeof (float), count, NULL + 4 * sizeof (float));
	glEnableClientState (GL_NORMAL_ARRAY);
	glNormalPointerEXT (GL_FLOAT, 12 * sizeof (float), 0, NULL + 6 * sizeof (float));
	glEnableClientState (GL_VERTEX_ARRAY);
	glVertexPointerEXT (3, GL_FLOAT, 12 * sizeof (float), 0, NULL + 9 * sizeof (float));

	/* Render buffer. */
	glDrawArraysEXT (GL_TRIANGLES, vertex0, count);

	/* Unbind buffer. */
	glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glClientActiveTextureARB (GL_TEXTURE1);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glClientActiveTextureARB (GL_TEXTURE0);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);
	glDisableClientState (GL_NORMAL_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);

	glPopMatrix ();
}

/**
 * \brief Renders triangles.
 *
 * \param self Rendering context.
 * \param vertex0 Vertex from which to begin rendering.
 * \param vertex1 Vertex to which to end rendering.
 * \param vertices Vertex array.
 */
void
lirnd_context_render_indexed (lirndContext*      self,
                              int                vertex0,
                              int                vertex1,
                              const limdlVertex* vertices)
{
	int i;
	limatVector n[3];
	limatVector v[3];
	const float* t[3];
	glColor3f (1.0f, 1.0f, 1.0f);

	glPushMatrix ();
	glMultMatrixf (self->matrix.m);
	glBegin (GL_TRIANGLES);
	for (i = vertex0 ; i < vertex1 ; i += 3)
	{
		/* FIXME: No multitexturing. */
		v[0] = vertices[i + 0].coord;
		v[1] = vertices[i + 1].coord;
		v[2] = vertices[i + 2].coord;
		n[0] = vertices[i + 0].normal;
		n[1] = vertices[i + 1].normal;
		n[2] = vertices[i + 2].normal;
		t[0] = vertices[i + 0].texcoord;
		t[1] = vertices[i + 1].texcoord;
		t[2] = vertices[i + 2].texcoord;
		glTexCoord2fv (t[0]);
		glNormal3f (n[0].x, n[0].y, n[0].z);
		glVertex3f (v[0].x, v[0].y, v[0].z);
		glTexCoord2fv (t[1]);
		glNormal3f (n[1].x, n[1].y, n[1].z);
		glVertex3f (v[1].x, v[1].y, v[1].z);
		glTexCoord2fv (t[2]);
		glNormal3f (n[2].x, n[2].y, n[2].z);
		glVertex3f (v[2].x, v[2].y, v[2].z);
	}
	glEnd ();
	glPopMatrix ();
}

void
lirnd_context_set_flags (lirndContext* self,
                         int           value)
{
	self->fixed = ((value & LIRND_FLAG_FIXED) != 0);
/*	self->lighting = ((value & LIRND_FLAG_LIGHTING) != 0);
	self->texturing = ((value & LIRND_FLAG_TEXTURING) != 0);*/
	self->shadows = ((value & LIRND_FLAG_SHADOW1) != 0);
}

void
lirnd_context_set_lights (lirndContext* self,
                          lirndLight**  value,
                          int           count)
{
	self->lights.array = value;
	self->lights.count = count;
}

void
lirnd_context_set_material (lirndContext*        self,
                            const lirndMaterial* value)
{
	self->material.flags = value->flags;
	self->material.shininess = value->shininess;
	memcpy (self->material.parameters, value->parameters, 4 * sizeof (float));
	memcpy (self->material.diffuse, value->diffuse, 4 * sizeof (float));
	memcpy (self->material.specular, value->specular, 4 * sizeof (float));
}

void
lirnd_context_set_matrix (lirndContext*      self,
                          const limatMatrix* value)
{
	self->matrix = *value;
}

void
lirnd_context_set_modelview (lirndContext*      self,
                             const limatMatrix* value)
{
	self->modelview = *value;
	self->compiled = 0;
}

void
lirnd_context_set_projection (lirndContext*      self,
                              const limatMatrix* value)
{
	self->projection = *value;
}

void
lirnd_context_set_shader (lirndContext* self,
                          lirndShader*  value)
{
	self->shader = value;
}

void
lirnd_context_set_textures (lirndContext* self,
                            lirndTexture* value,
                            int           count)
{
	self->textures.array = value;
	self->textures.count = count;
}

/*****************************************************************************/

static void
private_bind_lights_fixed (lirndContext* self)
{
	int i;

	for (i = 0 ; i < self->lights.count && i < 8 ; i++)
		private_enable_light (self, i, self->lights.array[i]);
	for ( ; i < 8 ; i++)
		glDisable (GL_LIGHT0 + i);
}

static void
private_bind_lights_shader (lirndContext* self)
{
	int i;
	int count;
	const GLfloat none[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	lirndLight* light;

	count = LI_MIN (self->lights.count, self->shader->lights.count);
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
private_bind_material (lirndContext* self)
{
	glMaterialf (GL_FRONT, GL_SHININESS, self->material.shininess);
	glMaterialfv (GL_FRONT, GL_SPECULAR, self->material.specular);
	glColor4fv (self->material.diffuse);
	/* TODO: Billboard support. */
	if (self->material.flags & LIRND_MATERIAL_FLAG_CULLFACE)
		glEnable (GL_CULL_FACE);
	else
		glDisable (GL_CULL_FACE);
	if (self->material.flags & LIRND_MATERIAL_FLAG_TRANSPARENCY)
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
private_bind_shader (lirndContext* self)
{
	if (livid_features.shader_model >= 3)
	{
		if (!self->fixed && self->shader != NULL)
			glUseProgramObjectARB (self->shader->program);
		else
			glUseProgramObjectARB (0);
	}
}

static void
private_bind_texture (lirndContext* self,
                      int           i,
                      lirndTexture* texture)
{
	glActiveTextureARB (GL_TEXTURE0 + i);
	glBindTexture (GL_TEXTURE_2D, texture->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->params.minfilter);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->params.minfilter);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->params.wraps);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->params.wrapt);
	glMatrixMode (GL_TEXTURE);
	glLoadIdentity ();
}

static void
private_bind_textures_fixed (lirndContext* self)
{
	int i;

	for (i = 0 ; i < self->textures.count ; i++)
		private_bind_texture (self, i, self->textures.array + i);
	for ( ; i < 8 ; i++)
	{
		glActiveTextureARB (GL_TEXTURE0 + i);
		glBindTexture (GL_TEXTURE_2D, 0);
		glMatrixMode (GL_TEXTURE);
		glLoadIdentity ();
	}
}

static void
private_bind_uniform (lirndContext* self,
                      lirndUniform* uniform)
{
	int index;
	int shadow[2];
	GLint map;
	lirndLight* light;
	lirndTexture* texture;
	limatMatrix matrix;
	limatMatrix bias =
	{{
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	}};

	/* Shader settings. */
	/* FIXME */
	shadow[0] = 0;//((flags & LIRND_FLAG_SHADOW0) != 0);
	shadow[1] = self->shadows;

	switch (uniform->value)
	{
		case LIRND_UNIFORM_NONE:
			break;
		case LIRND_UNIFORM_CUBETEXTURE0:
		case LIRND_UNIFORM_CUBETEXTURE1:
		case LIRND_UNIFORM_CUBETEXTURE2:
		case LIRND_UNIFORM_CUBETEXTURE3:
		case LIRND_UNIFORM_CUBETEXTURE4:
		case LIRND_UNIFORM_CUBETEXTURE5:
		case LIRND_UNIFORM_CUBETEXTURE6:
		case LIRND_UNIFORM_CUBETEXTURE7:
		case LIRND_UNIFORM_CUBETEXTURE8:
		case LIRND_UNIFORM_CUBETEXTURE9:
			index = uniform->value - LIRND_UNIFORM_CUBETEXTURE0;
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
				glMatrixMode (GL_TEXTURE);
				glLoadIdentity ();
			}
			break;
		case LIRND_UNIFORM_DIFFUSETEXTURE0:
		case LIRND_UNIFORM_DIFFUSETEXTURE1:
		case LIRND_UNIFORM_DIFFUSETEXTURE2:
		case LIRND_UNIFORM_DIFFUSETEXTURE3:
		case LIRND_UNIFORM_DIFFUSETEXTURE4:
		case LIRND_UNIFORM_DIFFUSETEXTURE5:
		case LIRND_UNIFORM_DIFFUSETEXTURE6:
		case LIRND_UNIFORM_DIFFUSETEXTURE7:
		case LIRND_UNIFORM_DIFFUSETEXTURE8:
		case LIRND_UNIFORM_DIFFUSETEXTURE9:
			index = uniform->value - LIRND_UNIFORM_DIFFUSETEXTURE0;
			if (index < self->textures.count)
			{
				texture = self->textures.array + index;
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_2D, texture->texture);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->params.minfilter);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->params.minfilter);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->params.wraps);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->params.wrapt);
			}
			else
			{
				glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
				glBindTexture (GL_TEXTURE_2D, 0);
				glMatrixMode (GL_TEXTURE);
				glLoadIdentity ();
			}
			break;
		case LIRND_UNIFORM_LIGHTTYPE0:
		case LIRND_UNIFORM_LIGHTTYPE1:
		case LIRND_UNIFORM_LIGHTTYPE2:
		case LIRND_UNIFORM_LIGHTTYPE3:
		case LIRND_UNIFORM_LIGHTTYPE4:
		case LIRND_UNIFORM_LIGHTTYPE5:
		case LIRND_UNIFORM_LIGHTTYPE6:
		case LIRND_UNIFORM_LIGHTTYPE7:
		case LIRND_UNIFORM_LIGHTTYPE8:
		case LIRND_UNIFORM_LIGHTTYPE9:
			index = uniform->value - LIRND_UNIFORM_LIGHTTYPE0;
			if (index < self->lights.count)
			{
				light = self->lights.array[index];
				if (light->directional)
					glUniform1iARB (uniform->binding, LIRND_UNIFORM_LIGHTTYPE_DIRECTIONAL);
				else if (LI_ABS (light->cutoff - M_PI) < 0.001)
					glUniform1iARB (uniform->binding, LIRND_UNIFORM_LIGHTTYPE_POINT);
				else if (light->shadow.map && shadow[1])
					glUniform1iARB (uniform->binding, LIRND_UNIFORM_LIGHTTYPE_SPOTSHADOW);
				else
					glUniform1iARB (uniform->binding, LIRND_UNIFORM_LIGHTTYPE_SPOT);
			}
			else
				glUniform1iARB (uniform->binding, LIRND_UNIFORM_LIGHTTYPE_DISABLED);
			break;
		case LIRND_UNIFORM_LIGHTMATRIX0:
		case LIRND_UNIFORM_LIGHTMATRIX1:
		case LIRND_UNIFORM_LIGHTMATRIX2:
		case LIRND_UNIFORM_LIGHTMATRIX3:
		case LIRND_UNIFORM_LIGHTMATRIX4:
		case LIRND_UNIFORM_LIGHTMATRIX5:
		case LIRND_UNIFORM_LIGHTMATRIX6:
		case LIRND_UNIFORM_LIGHTMATRIX7:
		case LIRND_UNIFORM_LIGHTMATRIX8:
		case LIRND_UNIFORM_LIGHTMATRIX9:
			index = uniform->value - LIRND_UNIFORM_LIGHTMATRIX0;
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
		case LIRND_UNIFORM_MODELMATRIX:
			glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->matrix.m);
			break;
		case LIRND_UNIFORM_MODELVIEWINVERSE:
			glUniformMatrix4fvARB (uniform->binding, 1, GL_FALSE, self->modelviewinverse.m);
			break;
		case LIRND_UNIFORM_NOISETEXTURE:
			glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
			glBindTexture (GL_TEXTURE_2D, self->render->helpers.noise);
			break;
		case LIRND_UNIFORM_SHADOWTEXTURE0:
		case LIRND_UNIFORM_SHADOWTEXTURE1:
		case LIRND_UNIFORM_SHADOWTEXTURE2:
		case LIRND_UNIFORM_SHADOWTEXTURE3:
		case LIRND_UNIFORM_SHADOWTEXTURE4:
		case LIRND_UNIFORM_SHADOWTEXTURE5:
		case LIRND_UNIFORM_SHADOWTEXTURE6:
		case LIRND_UNIFORM_SHADOWTEXTURE7:
		case LIRND_UNIFORM_SHADOWTEXTURE8:
		case LIRND_UNIFORM_SHADOWTEXTURE9:
			index = uniform->value - LIRND_UNIFORM_SHADOWTEXTURE0;
			map = self->render->lighting->lights.depth_texture_max;
			if (index < self->lights.count)
			{
				light = self->lights.array[index];
				if (light->shadow.map && shadow[index != 0])
					map = light->shadow.map;
			}
			glActiveTextureARB (GL_TEXTURE0 + uniform->sampler);
			glBindTexture (GL_TEXTURE_2D, map);
			break;
		case LIRND_UNIFORM_TIME:
			glUniform1fARB (uniform->binding, self->render->helpers.time);
			break;
	}
}

static void
private_enable_light (lirndContext* self,
                      int           i,
                      lirndLight*   light)
{
	limatVector tmp;
	GLfloat position[4];
	GLfloat direction[3];

	/* Calculate orientation. */
	lirnd_light_get_direction (light, &tmp);
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
	glEnable (GL_LIGHT0 + i);
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

/** @} */
/** @} */
