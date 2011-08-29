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
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenRender Render
 * @{
 */

#include "render-internal.h"
#include "../render21/render-private.h"
#include "../render32/render-private.h"

void liren_render_draw_clipped_buffer (
	LIRenRender*       self,
	LIRenShader*       shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	int                start,
	int                count,
	LIRenBuffer*       buffer)
{
	if (self->v32 != NULL)
	{
		liren_render32_draw_clipped_buffer (self->v32, shader->v32, modelview,
			projection, texture, diffuse, scissor, start, count, buffer->v32);
	}
	else
	{
		liren_render21_draw_clipped_buffer (self->v21, shader->v21, modelview,
			projection, texture, diffuse, scissor, start, count, buffer->v21);
	}
}

/**
 * \brief Finds a shader by name.
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader or NULL.
 */
LIRenShader* liren_render_find_shader (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->shaders, name);
}

/**
 * \brief Finds a texture by name.
 *
 * Searches for a texture from the texture cache and returns the match, if any.
 * If no match is found, NULL is returned.
 *
 * \param self Renderer.
 * \param name Name of the texture.
 * \return Texture or NULL.
 */
LIRenImage* liren_render_find_image (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a model by ID.
 * \param self Renderer.
 * \param id Model ID.
 * \return Model.
 */
LIRenModel* liren_render_find_model (
	LIRenRender* self,
	int          id)
{
	return lialg_u32dic_find (self->models, id);
}

/**
 * \brief Renders the scene.
 * \param self Renderer.
 * \param framebuffer Render target framebuffer.
 * \param viewport Viewport array.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projeciton matrix of the camera.
 * \param frustum Frustum of the camera.
 * \param render_passes Array of render passes.
 * \param render_passes_num Number of render passes.
 * \param postproc_passes Array of post-processing passes.
 * \param postproc_passes_num Number of post-processing passes.
 */
void liren_render_render_scene (
	LIRenRender*       self,
	LIRenFramebuffer*  framebuffer,
	const GLint*       viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num)
{
	if (self->v32 != NULL)
	{
		return liren_render32_render (self->v32, framebuffer->v32, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
	else
	{
		return liren_render21_render (self->v21, framebuffer->v21, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
}

/** @} */
/** @} */
/** @} */
