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
 * \addtogroup LIRenRender Render
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-private.h"
#include "render-scene.h"
#include "render21/render-private.h"
#include "render21/render-scene.h"
#include "render32/render-private.h"
#include "render32/render-scene.h"

LIRenObject* liren_render_find_object (
	LIRenRender* self,
	int          id)
{
	return lialg_u32dic_find (self->objects, id);
}

void liren_render_remove_model (
	LIRenRender* self,
	LIRenModel*  model)
{
	if (self->v32 != NULL)
		return liren_render32_remove_model (self->v32, model->v32);
	else
		return liren_render21_remove_model (self->v21, model->v21);
}

/**
 * \brief Renders the scene.
 * \param self Scene.
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
void liren_render_render (
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
