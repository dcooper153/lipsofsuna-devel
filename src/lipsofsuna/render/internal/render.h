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

#ifndef __RENDER_INTERNAL_RENDER_H__
#define __RENDER_INTERNAL_RENDER_H__

#include "SDL.h"
#include "lipsofsuna/paths.h"
#include "render-types.h"
#include "../render21/render.h"
#include "../render32/render.h"

struct _LIRenRender
{
	LIAlgRandom random;
	LIAlgStrdic* fonts;
	LIAlgStrdic* images;
	LIAlgStrdic* shaders;
	LIAlgU32dic* lights;
	LIAlgU32dic* models;
	LIAlgU32dic* objects;
	LIAlgU32dic* overlays;
	LIPthPaths* paths;
	LIRenOverlay* root_overlay;
	LIRenRender21* v21;
	LIRenRender32* v32;
	LIRenVideomode mode;
	SDL_Surface* screen;
};

LIAPICALL (LIRenImage*, liren_internal_find_image, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (LIRenModel*, liren_internal_find_model, (
	LIRenRender* self,
	int          id));

LIAPICALL (LIRenShader*, liren_internal_find_shader, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (int, liren_internal_load_font, (
	LIRenRender* self,
	const char*  name,
	const char*  file,
	int          size));

LIAPICALL (int, liren_internal_load_image, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (int, liren_internal_measure_text, (
	LIRenRender* self,
	const char*  font,
	const char*  text,
	int          width_limit,
	int*         result_width,
	int*         result_height));

LIAPICALL (void, liren_internal_reload, (
	LIRenRender* self,
	int          pass));

LIAPICALL (void, liren_internal_render, (
	LIRenRender* self));

LIAPICALL (void, liren_internal_render_scene, (
	LIRenRender*       self,
	LIRenFramebuffer*  framebuffer,
	const GLint*       viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num));

LIAPICALL (int, liren_internal_screenshot, (
	LIRenRender* self,
	const char*  path));

LIAPICALL (void, liren_internal_update, (
	LIRenRender* self,
	float        secs));

LIAPICALL (int, liren_internal_get_anisotropy, (
	const LIRenRender* self));

LIAPICALL (void, liren_internal_set_anisotropy, (
	LIRenRender* self,
	int          value));

LIAPICALL (int, liren_internal_get_image_size, (
	LIRenRender* self,
	const char*  name,
	int*         result));

LIAPICALL (float, liren_internal_get_opengl_version, (
	LIRenRender* self));

LIAPICALL (int, liren_internal_set_videomode, (
	LIRenRender*    self,
	LIRenVideomode* mode));

LIAPICALL (int, liren_internal_get_videomodes, (
	LIRenRender*     self,
	LIRenVideomode** modes,
	int*             modes_num));

#endif
