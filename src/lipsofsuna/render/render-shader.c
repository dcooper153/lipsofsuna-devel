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

/**
 * \brief Creates a new shader program.
 * \param render Renderer.
 * \param name Unique name.
 * \return New shader or NULL.
 */
LIRenShader* liren_shader_new (
	LIRenRender* render,
	const char*  name)
{
	int i;
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

	/* Initialize passes. */
	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
		liren_program_init (self->passes + i, render);

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
	int i;

	if (self->name != NULL)
		lialg_strdic_remove (self->render->shaders, self->name);
	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
		liren_program_clear (self->passes + i);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Removes a pass from the shader.
 * \param self Shader.
 * \param pass Pass number.
 */
void liren_shader_clear_pass (
	LIRenShader* self,
	int          pass)
{
	liren_program_clear (self->passes + pass);
	liren_program_init (self->passes + pass, self->render);
}

/**
 * \brief Recompiles the shader out of new code.
 * \param self Shader.
 * \param pass Pass number.
 * \param vertex Vertex shader code.
 * \param geometry Geometry shader code or NULL.
 * \param fragment Fragment shader code.
 * \param feedback Nonzero to enable transform feedback.
 * \return Nonzero on success.
 */
int liren_shader_compile (
	LIRenShader* self,
	int          pass,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment,
	int          feedback)
{
	return liren_program_compile (self->passes + pass, self->name,
		vertex, geometry, fragment, feedback);
}

/** @} */
/** @} */
