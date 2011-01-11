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
 * \param blend_enable Nonzero to enable blending.
 * \param blend_src Source blend function.
 * \param blend_dst Destination blend function.
 * \param color_write Nonzero to enable color writes.
 * \param depth_test Nonzero to enable depth test.
 * \param depth_write Nonzero to enable depth writes.
 * \param depth_func Depth test function.
 * \return Nonzero on success.
 */
int liren_shader_compile (
	LIRenShader* self,
	int          pass,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment,
	int          feedback,
	int          alpha_to_coverage,
	int          blend_enable,
	GLenum       blend_src,
	GLenum       blend_dst,
	int          color_write,
	int          depth_test,
	int          depth_write,
	GLenum       depth_func)
{
	liren_program_set_alpha_to_coverage (self->passes + pass, alpha_to_coverage);
	liren_program_set_blend (self->passes + pass, blend_enable, blend_src, blend_dst);
	liren_program_set_color (self->passes + pass, color_write);
	liren_program_set_depth (self->passes + pass, depth_test, depth_write, depth_func);
	return liren_program_compile (self->passes + pass, self->name,
		vertex, geometry, fragment, feedback);
}

int liren_shader_get_sort (
	LIRenShader* self)
{
	return self->sort;
}

void liren_shader_set_sort (
	LIRenShader* self,
	int          value)
{
	self->sort = value;
}

/** @} */
/** @} */
