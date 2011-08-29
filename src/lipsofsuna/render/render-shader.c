/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#include "lipsofsuna/system.h"
#include "render-shader.h"
#include "internal/render.h"
#include "internal/render-shader.h"

/**
 * \brief Creates a new shader program.
 * \param render Renderer.
 * \param name Unique shader name.
 * \return New shader or NULL.
 */
int liren_render_shader_new (
	LIRenRender* render,
	const char*  name)
{
	LIRenShader* self;

	self = lialg_strdic_find (render->shaders, name);
	if (self != NULL)
		return 0;

	self = liren_shader_new (render, name);

	return self != NULL;
}

/**
 * \brief Frees the shader program.
 * \param self Renderer.
 * \param shader Shader name.
 */
void liren_render_shader_free (
	LIRenRender* self,
	const char*  shader)
{
	LIRenShader* shader_;

	shader_ = lialg_strdic_find (self->shaders, shader);
	if (shader_ != NULL)
		liren_shader_free (shader_);
}

/**
 * \brief Removes a pass from the shader.
 * \param self Renderer.
 * \param shader Shader name.
 * \param pass Pass number.
 */
void liren_render_shader_clear_pass (
	LIRenRender* self,
	const char*  shader,
	int          pass)
{
	LIRenShader* shader_;

	shader_ = lialg_strdic_find (self->shaders, shader);
	if (shader_ != NULL)
		liren_shader_clear_pass (shader_, pass);
}

/**
 * \brief Recompiles the shader out of new code.
 * \param self Renderer.
 * \param shader Shader name.
 * \param pass Pass number.
 * \param vertex Vertex shader code.
 * \param geometry Geometry shader code or NULL.
 * \param fragment Fragment shader code.
 * \param animated Nonzero to enable skeletal transformation uploads.
 * \param alpha_to_coverage Nonzero to enable alpha to coverage.
 * \param blend_enable Nonzero to enable blending.
 * \param blend_src Source blend function.
 * \param blend_dst Destination blend function.
 * \param color_write Nonzero to enable color writes.
 * \param depth_test Nonzero to enable depth test.
 * \param depth_write Nonzero to enable depth writes.
 * \param depth_func Depth test function.
 * \return Nonzero on success.
 */
int liren_render_shader_compile (
	LIRenRender* self,
	const char*  shader,
	int          pass,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment,
	int          animated,
	int          alpha_to_coverage,
	int          blend_enable,
	int          blend_src,
	int          blend_dst,
	int          color_write,
	int          depth_test,
	int          depth_write,
	int          depth_func)
{
	LIRenShader* shader_;

	shader_ = lialg_strdic_find (self->shaders, shader);
	if (shader_ != NULL)
	{
		return liren_shader_compile (shader_, pass, vertex, geometry, fragment,
			animated, alpha_to_coverage, blend_enable, blend_src, blend_dst,
			color_write, depth_test, depth_write, depth_func);
	}
}

void liren_render_shader_set_sort (
	LIRenRender* self,
	const char*  shader,
	int          value)
{
	LIRenShader* shader_;

	shader_ = lialg_strdic_find (self->shaders, shader);
	if (shader_ != NULL)
		liren_shader_set_sort (shader_, value);
}

/** @} */
/** @} */
