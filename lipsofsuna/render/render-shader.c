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

static void private_init_attributes (
	LIRenShader* self);

static void private_init_uniforms (
	LIRenShader* self);

static int private_read_config (
	LIRenShader* self,
	LIArcReader* reader);

static int private_attribute_value (
	LIRenShader* self,
	const char*  value);

static int private_uniform_value (
	LIRenShader* self,
	const char*  value);

/****************************************************************************/

/**
 * \brief Creates a new shader program.
 *
 * \param render Renderer.
 * \return New shader or NULL.
 */
LIRenShader* liren_shader_new (
	LIRenRender* render,
	const char*  name,
	const char*  config,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment)
{
	GLint length;
	const GLchar* ptr;
	LIArcReader* reader;
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

	/* Parse configuration. */
	reader = liarc_reader_new (config, strlen (config));
	if (reader == NULL)
	{
		liren_shader_free (self);
		return NULL;
	}
	if (!private_read_config (self, reader))
	{
		liarc_reader_free (reader);
		liren_shader_free (self);
		return NULL;
	}
	liarc_reader_free (reader);

	/* Create shader objects. */
	self->vertex = glCreateShaderObjectARB (GL_VERTEX_SHADER);
	if (geometry != NULL)
		self->geometry = glCreateShaderObjectARB (GL_GEOMETRY_SHADER_ARB);
	self->fragment = glCreateShaderObjectARB (GL_FRAGMENT_SHADER);

	/* Upload shader source. */
	length = strlen (vertex);
	ptr = vertex;
	glShaderSourceARB (self->vertex, 1, &ptr, &length);
	if (geometry != NULL)
	{
		length = strlen (geometry);
		ptr = geometry;
		glShaderSourceARB (self->geometry, 1, &ptr, &length);
	}
	length = strlen (fragment);
	ptr = fragment;
	glShaderSourceARB (self->fragment, 1, &ptr, &length);

	/* Compile the vertex shader. */
	glCompileShaderARB (self->vertex);
	if (!private_check_compile (self, name, self->vertex))
	{
		liren_shader_free (self);
		return NULL;
	}

	/* Compile the geometry shader. */
	if (geometry != NULL)
	{
		glCompileShaderARB (self->geometry);
		if (!private_check_compile (self, name, self->geometry))
		{
			liren_shader_free (self);
			return NULL;
		}
	}

	/* Compile the fragment shader. */
	glCompileShaderARB (self->fragment);
	if (!private_check_compile (self, name, self->fragment))
	{
		liren_shader_free (self);
		return NULL;
	}

	/* Link the shader program. */
	self->program = glCreateProgramObjectARB ();
	glAttachObjectARB (self->program, self->vertex);
	if (geometry)
		glAttachObjectARB (self->program, self->geometry);
	glAttachObjectARB (self->program, self->fragment);
	private_init_attributes (self);
	glLinkProgramARB (self->program);
	if (!private_check_link (self, name, self->program))
	{
		liren_shader_free (self);
		return NULL;
	}
	glUseProgramObjectARB (self->program);

	/* Initialize uniforms. */
	private_init_uniforms (self);
	glUseProgramObjectARB (0);

	return self;
}

/**
 * \brief Frees the shader program.
 *
 * \param self Shader.
 */
void liren_shader_free (
	LIRenShader* self)
{
	int i;

	if (self->vertex)
		glDeleteObjectARB (self->vertex);
	if (self->geometry)
		glDeleteObjectARB (self->geometry);
	if (self->fragment)
		glDeleteObjectARB (self->fragment);
	if (self->program)
		glDeleteObjectARB (self->program);
	for (i = 0 ; i < self->attributes.count ; i++)
		lisys_free (self->attributes.array[i].name);
	for (i = 0 ; i < self->uniforms.count ; i++)
		lisys_free (self->uniforms.array[i].name);
	lisys_free (self->attributes.array);
	lisys_free (self->uniforms.array);
	lisys_free (self->name);
	lisys_free (self);
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
	glGetObjectParameterivARB (shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetInfoLogARB (shader, sizeof (log), &length, log);
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

	glGetObjectParameterivARB (program, GL_LINK_STATUS, &status);
	glGetInfoLogARB (program, sizeof (log), &length, log);
	if (status == GL_FALSE)
	{
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

static void private_init_attributes (
	LIRenShader* self)
{
	int i;
	LIRenAttribute* attribute;

	if (livid_features.shader_model >= 3)
	{
		for (i = 0 ; i < self->attributes.count ; i++)
		{
			attribute = self->attributes.array + i;
			if (attribute->value != LIREN_ATTRIBUTE_NONE)
				attribute->binding = attribute->value;
			else
				attribute->binding = LIREN_ATTRIBUTE_COORD;
			glBindAttribLocationARB (self->program, attribute->binding, attribute->name);
		}
	}
}

static void private_init_uniforms (
	LIRenShader* self)
{
	int i;
	int sampler = 0;
	LIRenUniform* uniform;

	if (livid_features.shader_model >= 3)
	{
		for (i = 0 ; i < self->uniforms.count ; i++)
		{
			uniform = self->uniforms.array + i;
			uniform->binding = glGetUniformLocationARB (self->program, uniform->name);
			if (liren_uniform_get_sampler (uniform))
			{
				uniform->sampler = sampler++;
				glUniform1iARB (uniform->binding, uniform->sampler);
			}
		}
	}
}

static int private_read_config (
	LIRenShader* self,
	LIArcReader* reader)
{
	char* line;
	char* name;
	char* ptr;
	char* value;
	LIRenAttribute* attribute;
	LIRenUniform* uniform;

	/* Parse options. */
	while (1)
	{
		if (!liarc_reader_get_text (reader, "\n", &line))
			break;
		if (!strlen (line))
		{
			lisys_free (line);
			continue;
		}
		if (!strncmp (line, "light-count ", 12))
		{
			self->lights.count = atoi (line + 12);
			lisys_free (line);
			if (self->lights.count < 0 ||
			    self->lights.count >= 8)
				return 0;
		}
		else if (!strncmp (line, "attribute ", 10))
		{
			/* Separate fields. */
			for (name = line + 10 ; *name != '\0' && isspace (*name) ; name++) {}
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
			attribute = lisys_realloc (self->attributes.array, (self->attributes.count + 1) * sizeof (LIRenAttribute));
			if (attribute == NULL)
			{
				lisys_free (line);
				return 0;
			}
			self->attributes.array = attribute;
			attribute += self->attributes.count;

			/* Initialize attribute. */
			attribute->binding = 0;
			attribute->value = private_attribute_value (self, value);
			attribute->name = listr_dup (name);
			if (attribute->name == NULL)
			{
				lisys_free (line);
				return 0;
			}
			self->attributes.count++;
			lisys_free (line);
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
			uniform = lisys_realloc (self->uniforms.array, (self->uniforms.count + 1) * sizeof (LIRenUniform));
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

static int private_attribute_value (
	LIRenShader* self,
	const char*  value)
{
	if (!strcmp (value, "NONE"))
		return LIREN_ATTRIBUTE_NONE;
	if (!strcmp (value, "COORD"))
		return LIREN_ATTRIBUTE_COORD;
	if (!strcmp (value, "NORMAL"))
		return LIREN_ATTRIBUTE_NORMAL;
	if (!strcmp (value, "TEXCOORD"))
		return LIREN_ATTRIBUTE_TEXCOORD;

	return LIREN_ATTRIBUTE_NONE;
}

static int private_uniform_value (
	LIRenShader* self,
	const char*  value)
{
	int index;

	if (!strcmp (value, "NONE"))
		return LIREN_UNIFORM_NONE;
	if (!strncmp (value, "CUBETEXTURE", 11))
	{
		index = atoi (value + 11);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_CUBETEXTURE0 + index;
	}
	if (!strncmp (value, "DIFFUSETEXTURE", 14))
	{
		index = atoi (value + 14);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_DIFFUSETEXTURE0 + index;
	}
	if (!strncmp (value, "LIGHTAMBIENT", 12))
	{
		index = atoi (value + 12);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTAMBIENT0 + index;
	}
	if (!strncmp (value, "LIGHTDIFFUSE", 12))
	{
		index = atoi (value + 12);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTDIFFUSE0 + index;
	}
	if (!strncmp (value, "LIGHTEQUATION", 13))
	{
		index = atoi (value + 13);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTEQUATION0 + index;
	}
	if (!strncmp (value, "LIGHTMATRIX", 11))
	{
		index = atoi (value + 11);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTMATRIX0 + index;
	}
	if (!strncmp (value, "LIGHTPOSITIONPREMULT", 20))
	{
		index = atoi (value + 20);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTPOSITIONPREMULT0 + index;
	}
	if (!strncmp (value, "LIGHTPOSITION", 13))
	{
		index = atoi (value + 13);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTPOSITION0 + index;
	}
	if (!strncmp (value, "LIGHTSPECULAR", 13))
	{
		index = atoi (value + 13);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTSPECULAR0 + index;
	}
	if (!strncmp (value, "LIGHTTYPE", 9))
	{
		index = atoi (value + 9);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_LIGHTTYPE0 + index;
	}
	if (!strcmp (value, "MATERIALDIFFUSE"))
		return LIREN_UNIFORM_MATERIALDIFFUSE;
	if (!strcmp (value, "MATERIALSHININESS"))
		return LIREN_UNIFORM_MATERIALSHININESS;
	if (!strcmp (value, "MATERIALSPECULAR"))
		return LIREN_UNIFORM_MATERIALSPECULAR;
	if (!strcmp (value, "MATRIXMODEL"))
		return LIREN_UNIFORM_MATRIXMODEL;
	if (!strcmp (value, "MATRIXMODELVIEW"))
		return LIREN_UNIFORM_MATRIXMODELVIEW;
	if (!strcmp (value, "MATRIXMODELVIEWINVERSE"))
		return LIREN_UNIFORM_MATRIXMODELVIEWINVERSE;
	if (!strcmp (value, "MATRIXNORMAL"))
		return LIREN_UNIFORM_MATRIXNORMAL;
	if (!strcmp (value, "MATRIXPROJECTION"))
		return LIREN_UNIFORM_MATRIXPROJECTION;
	if (!strcmp (value, "MATRIXPROJECTIONINVERSE"))
		return LIREN_UNIFORM_MATRIXPROJECTIONINVERSE;
	if (!strcmp (value, "NOISETEXTURE"))
		return LIREN_UNIFORM_NOISETEXTURE;
	if (!strcmp (value, "PARAM0"))
		return LIREN_UNIFORM_PARAM0;
	if (!strncmp (value, "SHADOWTEXTURE", 13))
	{
		index = atoi (value + 13);
		if (index < 0 || index > 9)
			return LIREN_UNIFORM_NONE;
		return LIREN_UNIFORM_SHADOWTEXTURE0 + index;
	}
	if (!strcmp (value, "TIME"))
		return LIREN_UNIFORM_TIME;

	return LIREN_UNIFORM_NONE;
}

/** @} */
/** @} */
