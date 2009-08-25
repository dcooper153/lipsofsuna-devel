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
 * \addtogroup lirndShader Shader
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "render-shader.h"

static int
private_check_compile (lirndShader* self,
                       const char*  name,
                       GLint        shader);

static int
private_check_link (lirndShader* self,
                    const char*  name,
                    GLint        program);

static void
private_init_uniforms (lirndShader* self);

static int
private_read_file (lirndShader* self,
                   const char*  path,
                   const char*  name);

static int
private_read_stream (lirndShader* self,
                     const char*  name,
                     liReader*    reader);

static int
private_read_config (lirndShader* self,
                     liReader*    reader);

static int
private_read_source (lirndShader* self,
                     liReader*    reader);

static int
private_uniform_value (lirndShader* self,
                       const char*  value);

/****************************************************************************/

/**
 * \brief Creates a new shader program.
 *
 * \param render Renderer.
 * \return New shader or NULL.
 */
lirndShader*
lirnd_shader_new (lirndRender* render)
{
	lirndShader* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndShader));
	if (self == NULL)
		return NULL;
	self->render = render;

	return self;
}

/**
 * \brief Loads a shader program from a stream.
 *
 * \param render Renderer.
 * \param reader Stream reader.
 * \return New shader or NULL.
 */
lirndShader*
lirnd_shader_new_from_data (lirndRender* render,
                            liReader*    reader)
{
	lirndShader* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndShader));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Load the shader. */
	if (!private_read_stream (self, "<stream>", reader))
	{
		lirnd_shader_free (self);
		return NULL;
	}

	/* Initialize uniforms. */
	private_init_uniforms (self);

	return self;
}

/**
 * \brief Loads a shader program from a file.
 *
 * \param render Renderer.
 * \param path Path to the shader file or NULL.
 * \return New shader or NULL.
 */
lirndShader*
lirnd_shader_new_from_file (lirndRender* render,
                            const char*  path)
{
	const char* name;
	lirndShader* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndShader));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Load the shader. */
	name = strrchr (path, '/');
	if (name != NULL)
		name++;
	else
		name = path;
	if (!private_read_file (self, path, name))
	{
		lirnd_shader_free (self);
		return NULL;
	}

	/* Initialize uniforms. */
	private_init_uniforms (self);

	return self;
}

/**
 * \brief Frees the shader program.
 *
 * \param self Shader.
 */
void
lirnd_shader_free (lirndShader* self)
{
	int i;

	if (self->vertex)
		glDeleteObjectARB (self->vertex);
	if (self->fragment)
		glDeleteObjectARB (self->fragment);
	if (self->program)
		glDeleteObjectARB (self->program);
	for (i = 0 ; i < self->uniforms.count ; i++)
		lisys_free (self->uniforms.array[i].name);
	lisys_free (self->uniforms.array);
	lisys_free (self->name);
	lisys_free (self);
}

/****************************************************************************/

static int
private_check_compile (lirndShader* self,
                       const char*  name,
                       GLint        shader)
{
	char* text;
	const char* type;
	GLint status;
	GLchar log[512];
	GLsizei length;
	liReader* reader;

	type = (shader == self->vertex)? "vertex" : "fragment";
	glGetObjectParameterivARB (shader, GL_COMPILE_STATUS, &status);
	glGetInfoLogARB (shader, sizeof (log), &length, log);
	if (length)
	{
		reader = li_reader_new (log, length);
		if (reader != NULL)
		{
			while (li_reader_get_text (reader, "\n", &text))
			{
				printf ("WARNING: %s:%s%s\n", name, type, text);
				lisys_free (text);
			}
			li_reader_free (reader);
		}
		if (strstr (log, "software"))
		{
			lisys_error_set (ENOTSUP, "unsupported %s shader `%s'", type, name);
			return 0;
		}
	}
	if (status == GL_FALSE)
	{
		lisys_error_set (EINVAL, "cannot compile %s shader `%s'", type, name);
		return 0;
	}

	return 1;
}

static int
private_check_link (lirndShader* self,
                    const char*  name,
                    GLint        program)
{
	char* text;
	GLint status;
	GLchar log[512];
	GLsizei length;
	liReader* reader;

	glGetObjectParameterivARB (program, GL_LINK_STATUS, &status);
	glGetInfoLogARB (program, sizeof (log), &length, log);
	if (length)
	{
		reader = li_reader_new (log, length);
		if (reader != NULL)
		{
			while (li_reader_get_text (reader, "\n", &text))
			{
				printf ("WARNING: %s:%s\n", name, text);
				lisys_free (text);
			}
			li_reader_free (reader);
		}
		if (strstr (log, "software"))
		{
			lisys_error_set (ENOTSUP, "unsupported program `%s'", name);
			return 0;
		}
	}
	if (status == GL_FALSE)
	{
		lisys_error_set (EINVAL, "cannot link program `%s'", name);
		return 0;
	}

	return 1;
}

static void
private_init_uniforms (lirndShader* self)
{
	int i;
	int sampler = 0;
	lirndUniform* uniform;

	if (livid_features.shader_model >= 3)
	{
		for (i = 0 ; i < self->uniforms.count ; i++)
		{
			uniform = self->uniforms.array + i;
			uniform->binding = glGetUniformLocationARB (self->program, uniform->name);
			if (lirnd_uniform_get_sampler (uniform))
			{
				uniform->sampler = sampler++;
				glUniform1iARB (uniform->binding, uniform->sampler);
			}
		}
		glUseProgramObjectARB (0);
	}
}

static int
private_read_file (lirndShader* self,
                   const char*  path,
                   const char*  name)
{
	int ret;
	liReader* reader;

	/* Open the file. */
	reader = li_reader_new_from_file (path);
	if (reader == NULL)
		return 0;

	/* Read from stream. */
	ret = private_read_stream (self, name, reader);
	li_reader_free (reader);
	return ret;
}

static int
private_read_stream (lirndShader* self,
                     const char*  name,
                     liReader*    reader)
{
	if (livid_features.shader_model >= 3)
	{
		/* Read the shaders. */
		self->vertex = glCreateShaderObjectARB (GL_VERTEX_SHADER);
		self->fragment = glCreateShaderObjectARB (GL_FRAGMENT_SHADER);
		if (!private_read_config (self, reader) ||
			!private_read_source (self, reader))
			return 0;

		/* Compile the vertex shader. */
		glCompileShaderARB (self->vertex);
		if (!private_check_compile (self, name, self->vertex))
			return 0;

		/* Compile the fragment shader. */
		glCompileShaderARB (self->fragment);
		if (!private_check_compile (self, name, self->fragment))
			return 0;

		/* Link the shader program. */
		self->program = glCreateProgramObjectARB ();
		glAttachObjectARB (self->program, self->vertex);
		glAttachObjectARB (self->program, self->fragment);
		glLinkProgramARB (self->program);
		if (!private_check_link (self, name, self->program))
			return 0;
		glUseProgramObjectARB (self->program);
	}
	else
	{
		/* Only read configuration. */
		if (!private_read_config (self, reader))
			return 0;
	}

	return 1;
}

static int
private_read_config (lirndShader* self,
                     liReader*    reader)
{
	char* line;
	char* name;
	char* ptr;
	char* value;
	lirndUniform* uniform;

	/* Find start. */
	while (1)
	{
		if (!li_reader_get_text (reader, "\n", &line))
			return 0;
		if (!strcmp (line, "[configuration]"))
		{
			lisys_free (line);
			break;
		}
		lisys_free (line);
	}

	/* Parse options. */
	while (1)
	{
		if (!li_reader_get_text (reader, "\n", &line))
			return 0;
		if (!strlen (line))
		{
			lisys_free (line);
			continue;
		}
		if (!strcmp (line, "[vertex shader]"))
		{
			lisys_free (line);
			break;
		}
		else if (!strncmp (line, "light-count ", 12))
		{
			self->lights.count = atoi (line + 12);
			lisys_free (line);
			if (self->lights.count < 0 ||
			    self->lights.count >= 8)
				return 0;
		}
		else if (!strncmp (line, "uniform ", 8))
		{
			/* Separate fields. */
			for (name = line + 8 ; *name != '\0' && isspace (*name) ; name++) {}
			for (ptr = name ; *ptr != '\0' && !isspace (*ptr) ; ptr++) {}
			if (*ptr != '\0')
			{
				*ptr = '\0';
				ptr++;
			}
			for (value = ptr ; *value != '\0' && isspace (*value) ; value++) {}
			for (ptr = value ; *ptr != '\0' && !isspace (*ptr) ; ptr++) {}
			if (*ptr != '\0')
			{
				*ptr = '\0';
				ptr++;
			}

			/* Resize array. */
			uniform = realloc (self->uniforms.array, (self->uniforms.count + 1) * sizeof (lirndUniform));
			if (uniform == NULL)
			{
				lisys_free (line);
				return 0;
			}
			self->uniforms.array = uniform;
			uniform += self->uniforms.count;

			/* Initialize uniform. */
			uniform->binding = 0;
			uniform->value = private_uniform_value (self, value);
			uniform->name = listr_dup (name);
			if (uniform->name == NULL)
			{
				lisys_free (line);
				return 0;
			}
			self->uniforms.count++;
			lisys_free (line);
		}
		else
		{
			lisys_error_set (EINVAL, NULL);
			lisys_free (line);
			return 0;
		}
	}

	return 1;
}

static int
private_read_source (lirndShader* self,
                     liReader*    reader)
{
	GLint length;
	GLint vert_start;
	GLint vert_end;
	GLint frag_start;
	GLint frag_end;
	char* line;
	const GLchar* ptr;

	/* Split into parts. */
	vert_start = reader->pos;
	while (1)
	{
		vert_end = reader->pos;
		if (!li_reader_get_text (reader, "\n", &line))
		{
			li_reader_free (reader);
			return 0;
		}
		if (!strcmp (line, "[fragment shader]"))
		{
			frag_start = reader->pos;
			frag_end = reader->length;
			lisys_free (line);
			break;
		}
		lisys_free (line);
	}

	/* Upload shader source. */
	length = vert_end - vert_start;
	ptr = reader->buffer + vert_start;
	glShaderSourceARB (self->vertex, 1, &ptr, &length);
	length = frag_end - frag_start;
	ptr = reader->buffer + frag_start;
	glShaderSourceARB (self->fragment, 1, &ptr, &length);

	return 1;
}

static int
private_uniform_value (lirndShader* self,
                       const char*  value)
{
	int index;

	if (!strcmp (value, "NONE"))
		return LIRND_UNIFORM_NONE;
	if (!strncmp (value, "CUBETEXTURE", 11))
	{
		index = atoi (value + 11);
		if (index < 0 || index > 9)
			return LIRND_UNIFORM_NONE;
		return LIRND_UNIFORM_CUBETEXTURE0 + index;
	}
	if (!strncmp (value, "DIFFUSETEXTURE", 14))
	{
		index = atoi (value + 14);
		if (index < 0 || index > 9)
			return LIRND_UNIFORM_NONE;
		return LIRND_UNIFORM_DIFFUSETEXTURE0 + index;
	}
	if (!strncmp (value, "LIGHTTYPE", 9))
	{
		index = atoi (value + 9);
		if (index < 0 || index > 9)
			return LIRND_UNIFORM_NONE;
		return LIRND_UNIFORM_LIGHTTYPE0 + index;
	}
	if (!strncmp (value, "LIGHTMATRIX", 11))
	{
		index = atoi (value + 11);
		if (index < 0 || index > 9)
			return LIRND_UNIFORM_NONE;
		return LIRND_UNIFORM_LIGHTMATRIX0 + index;
	}
	if (!strcmp (value, "MODELMATRIX"))
		return LIRND_UNIFORM_MODELMATRIX;
	if (!strcmp (value, "MODELVIEWINVERSE"))
		return LIRND_UNIFORM_MODELVIEWINVERSE;
	if (!strcmp (value, "NOISETEXTURE"))
		return LIRND_UNIFORM_NOISETEXTURE;
	if (!strncmp (value, "SHADOWTEXTURE", 13))
	{
		index = atoi (value + 13);
		if (index < 0 || index > 9)
			return LIRND_UNIFORM_NONE;
		return LIRND_UNIFORM_SHADOWTEXTURE0 + index;
	}
	if (!strcmp (value, "TIME"))
		return LIRND_UNIFORM_TIME;

	return LIRND_UNIFORM_NONE;
}

/** @} */
/** @} */
