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
 * \addtogroup LIRenShader Shader
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "render-shader.h"

static int private_check_compile (
	LIRenShader* self,
	const char*  name,
	GLint        shader);

static int private_check_link (
	LIRenShader* self,
	const char*  name,
	GLint        program);

static void private_clear (
	LIRenShader* self);

/****************************************************************************/

/**
 * \brief Creates a new shader program.
 * \param render Renderer.
 * \param name Unique name.
 * \param vertex Vertex shader code.
 * \param geometry Geometry shader code or NULL.
 * \param fragment Fragment shader code.
 * \param feedback Nonzero to enable transform feedback.
 * \return New shader or NULL.
 */
LIRenShader* liren_shader_new (
	LIRenRender* render,
	const char*  name,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment,
	int          feedback)
{
	LIRenShader* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenShader));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->name = listr_dup (name);
	if (self->name == NULL)
	{
		liren_shader_free (self);
		return NULL;
	}

	/* Compile the shader. */
	if (!liren_shader_compile (self, vertex, geometry, fragment, feedback))
	{
		liren_shader_free (self);
		return NULL;
	}

	/* Insert to dictionary. */
	if (!lialg_strdic_insert (render->shaders, name, self))
	{
		liren_shader_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the shader program.
 * \param self Shader.
 */
void liren_shader_free (
	LIRenShader* self)
{
	if (self->name != NULL)
		lialg_strdic_remove (self->render->shaders, self->name);
	glDeleteProgram (self->program);
	glDeleteShader (self->vertex);
	glDeleteShader (self->geometry);
	glDeleteShader (self->fragment);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Recompiles the shader out of new code.
 * \param self Shader.
 * \param vertex Vertex shader code.
 * \param geometry Geometry shader code or NULL.
 * \param fragment Fragment shader code.
 * \param feedback Nonzero to enable transform feedback.
 * \return Nonzero on success.
 */
int liren_shader_compile (
	LIRenShader* self,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment,
	int          feedback)
{
	int i;
	GLint restore;
	GLint binding;
	GLint lengths[3];
	GLint uniforms;
	const GLchar* strings[3];
	const GLchar* samplers[8] =
	{
		"LOS_diffuse_texture_0",
		"LOS_diffuse_texture_1",
		"LOS_diffuse_texture_2",
		"LOS_diffuse_texture_3",
		"LOS_buffer_texture",
		"LOS_cube_texture",
		"LOS_noise_texture",
		"LOS_shadow_texture"
	};
	const GLchar* feedbacks[4] =
	{
		"LOS_out_texcoord",
		"LOS_out_normal",
		"LOS_out_coord",
		"LOS_out_tangent"
	};
	const GLchar* headers[4] =
	{
		/* Common */
		"layout(shared) uniform LOSDATA\n{\n"
		"	vec4 light_ambient;\n"
		"	vec4 light_diffuse;\n"
		"	vec3 light_direction;\n"
		"	vec3 light_direction_premult;\n"
		"	vec3 light_equation;\n"
		"	mat4 light_matrix;\n"
		"	vec3 light_position;\n"
		"	vec3 light_position_premult;\n"
		"	vec4 light_specular;\n"
		"	vec3 light_spot;\n"
		"	float light_type;\n"
		"	vec4 material_diffuse;\n"
		"	vec4 material_param_0;\n"
		"	float material_shininess;\n"
		"	vec4 material_specular;\n"
		"	mat4 matrix_model;\n"
		"	mat4 matrix_modelview;\n"
		"	mat4 matrix_modelview_inverse;\n"
		"	mat4 matrix_modelview_projection;\n"
		"	mat3 matrix_normal;\n"
		"	mat4 matrix_projection;\n"
		"	mat4 matrix_projection_inverse;\n"
		"	float time;\n"
		"} LOS;\n"
		"uniform sampler2D LOS_diffuse_texture_0;\n"
		"uniform sampler2D LOS_diffuse_texture_1;\n"
		"uniform sampler2D LOS_diffuse_texture_2;\n"
		"uniform sampler2D LOS_diffuse_texture_3;\n"
		"uniform samplerBuffer LOS_buffer_texture;\n"
		"uniform samplerCube LOS_cube_texture;\n"
		"uniform sampler2D LOS_noise_texture;\n"
		"uniform sampler2DShadow LOS_shadow_texture;\n",
		/* Vertex */
		"#version 150\n"
		"in vec3 LOS_coord;\n"
		"in vec2 LOS_texcoord;\n"
		"in vec3 LOS_normal;\n"
		"in vec3 LOS_tangent;\n"
		"in vec4 LOS_weights1;\n"
		"in vec4 LOS_weights2;\n"
		"in vec4 LOS_bones1;\n"
		"in vec4 LOS_bones2;\n",
		/* Geometry */
		"#version 150\n"
		"#extension GL_EXT_geometry_shader4 : enable\n",
		/* Fragment */
		"#version 150\n"
	};
	LIRenShader tmp;

	/* Initialize a temporary struct so that we don't overwrite
	   the old shader if something goes wrong. */
	memset (&tmp, 0, sizeof (LIRenShader));
	tmp.render = self->render;
	tmp.name = self->name;

	/* Create shader objects. */
	tmp.vertex = glCreateShader (GL_VERTEX_SHADER);
	if (geometry != NULL)
		tmp.geometry = glCreateShader (GL_GEOMETRY_SHADER);
	tmp.fragment = glCreateShader (GL_FRAGMENT_SHADER);

	/* Upload shader source. */
	strings[0] = headers[1];
	strings[1] = headers[0];
	strings[2] = vertex;
	lengths[0] = strlen (strings[0]);
	lengths[1] = strlen (strings[1]);
	lengths[2] = strlen (strings[2]);
	glShaderSource (tmp.vertex, 3, strings, lengths);
	if (geometry != NULL)
	{
		strings[0] = headers[2];
		strings[1] = headers[0];
		strings[2] = geometry;
		lengths[0] = strlen (strings[0]);
		lengths[1] = strlen (strings[1]);
		lengths[2] = strlen (strings[2]);
		glShaderSource (tmp.geometry, 3, strings, lengths);
	}
	strings[0] = headers[3];
	strings[1] = headers[0];
	strings[2] = fragment;
	lengths[0] = strlen (strings[0]);
	lengths[1] = strlen (strings[1]);
	lengths[2] = strlen (strings[2]);
	glShaderSource (tmp.fragment, 3, strings, lengths);

	/* Compile the vertex shader. */
	glCompileShader (tmp.vertex);
	if (!private_check_compile (&tmp, tmp.name, tmp.vertex))
	{
		private_clear (&tmp);
		return 0;
	}

	/* Compile the geometry shader. */
	if (geometry != NULL)
	{
		glCompileShader (tmp.geometry);
		if (!private_check_compile (&tmp, tmp.name, tmp.geometry))
		{
			private_clear (&tmp);
			return 0;
		}
	}

	/* Compile the fragment shader. */
	glCompileShader (tmp.fragment);
	if (!private_check_compile (&tmp, tmp.name, tmp.fragment))
	{
		private_clear (&tmp);
		return 0;
	}

	/* Link the shader program. */
	tmp.program = glCreateProgram ();
	glAttachShader (tmp.program, tmp.vertex);
	if (tmp.geometry)
		glAttachShader (tmp.program, tmp.geometry);
	glAttachShader (tmp.program, tmp.fragment);
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_COORD, "LOS_coord");
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_TEXCOORD, "LOS_texcoord");
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_NORMAL, "LOS_normal");
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_TANGENT, "LOS_tangent");
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_WEIGHTS1, "LOS_weights1");
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_WEIGHTS2, "LOS_weights2");
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_BONES1, "LOS_bones1");
	glBindAttribLocation (tmp.program, LIREN_ATTRIBUTE_BONES2, "LOS_bones2");
	if (feedback)
	{
		glTransformFeedbackVaryings (tmp.program, sizeof (feedbacks) /
			sizeof (*feedbacks), feedbacks, GL_INTERLEAVED_ATTRIBS);
	}
	glLinkProgram (tmp.program);
	if (!private_check_link (&tmp, tmp.name, tmp.program))
	{
		private_clear (&tmp);
		return 0;
	}

	/* Bind the uniform buffer. */
	/* All shaders share the same uniform block so that we don't need to
	   reset our uniform data every time the shader is changed. */
	uniforms = glGetUniformBlockIndex (tmp.program, "LOSDATA");
	if (uniforms != GL_INVALID_INDEX)
	{
		glUniformBlockBinding (tmp.program, uniforms, 0);
		liren_uniforms_setup (&self->render->context->uniforms, tmp.program);
	}

	/* Bind samplers to standard indices. */
	/* All shaders use the same texture unit layout so that we don't need
	   to rebind textures every time the shader is changed. */
	glGetIntegerv (GL_CURRENT_PROGRAM, &restore);
	glUseProgram (tmp.program);
	for (i = 0 ; i < sizeof (samplers) / sizeof (*samplers) ; i++)
	{
		binding = glGetUniformLocation (tmp.program, samplers[i]);
		if (binding != GL_INVALID_INDEX)
			glUniform1i (binding, i);
	}
	glUseProgram (restore);

	/* Replace the old program with the new one. */
	private_clear (self);
	memcpy (self, &tmp, sizeof (LIRenShader));

	return 1;
}

/****************************************************************************/

static int private_check_compile (
	LIRenShader* self,
	const char*  name,
	GLint        shader)
{
	char* text;
	const char* type;
	GLint status;
	GLchar log[512];
	GLsizei length;
	LIArcReader* reader;

	if (shader == self->vertex)
		type = "vertex";
	else if (shader == self->fragment)
		type = "fragment";
	else
		type = "geometry";
	glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderInfoLog (shader, sizeof (log), &length, log);
		if (length)
		{
			reader = liarc_reader_new (log, length);
			if (reader != NULL)
			{
				while (liarc_reader_get_text (reader, "\n", &text))
				{
					printf ("WARNING: %s:%s %s\n", name, type, text);
					lisys_free (text);
				}
				liarc_reader_free (reader);
			}
		}
		lisys_error_set (EINVAL, "cannot compile %s shader `%s'", type, name);
		return 0;
	}

	return 1;
}

static int private_check_link (
	LIRenShader* self,
	const char*  name,
	GLint        program)
{
	char* text;
	GLint status;
	GLchar log[512];
	GLsizei length;
	LIArcReader* reader;

	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramInfoLog (program, sizeof (log), &length, log);
		reader = liarc_reader_new (log, length);
		if (reader != NULL)
		{
			while (liarc_reader_get_text (reader, "\n", &text))
			{
				printf ("WARNING: %s:%s\n", name, text);
				lisys_free (text);
			}
			liarc_reader_free (reader);
		}
		lisys_error_set (EINVAL, "cannot link program `%s'", name);
		return 0;
	}

	return 1;
}

static void private_clear (
	LIRenShader* self)
{
	glDeleteProgram (self->program);
	glDeleteShader (self->vertex);
	glDeleteShader (self->geometry);
	glDeleteShader (self->fragment);
}

/** @} */
/** @} */
