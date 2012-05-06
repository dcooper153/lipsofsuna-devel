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

#include "lipsofsuna/paths.h"
#include "render-types.h"

struct _LIRenRender
{
	int anisotropy;
	LIAlgRandom random;
	LIAlgStrdic* fonts;
	LIAlgU32dic* lights;
	LIAlgU32dic* models;
	LIAlgU32dic* objects;
	LIAlgU32dic* overlays;
	LIPthPaths* paths;
	LIRenVideomode mode;
	LIRenRenderData* data;
};

LIAPICALL (int, liren_internal_init, (
	LIRenRender*    self,
	LIRenVideomode* mode));

LIAPICALL (void, liren_internal_deinit, (
	LIRenRender* self));

LIAPICALL (void, liren_internal_add_compositor, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (void, liren_internal_remove_compositor, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (LIRenModel*, liren_internal_find_model, (
	LIRenRender* self,
	int          id));

LIAPICALL (int, liren_internal_layout_text, (
	LIRenRender* self,
	const char*  font,
	const char*  text,
	int          width_limit,
	int**        result_glyphs,
	int*         result_glyphs_num));

LIAPICALL (int, liren_internal_load_font, (
	LIRenRender* self,
	const char*  name,
	const char*  file,
	int          size));

LIAPICALL (int, liren_internal_measure_text, (
	LIRenRender* self,
	const char*  font,
	const char*  text,
	int          width_limit,
	int*         result_width,
	int*         result_height));

LIAPICALL (void, liren_internal_project, (
	LIRenRender*       self,
	const LIMatVector* world,
	LIMatVector*       screen));

LIAPICALL (void, liren_internal_reload, (
	LIRenRender* self,
	int          pass));

LIAPICALL (void, liren_internal_render, (
	LIRenRender* self));

LIAPICALL (int, liren_internal_screenshot, (
	LIRenRender* self,
	const char*  path));

LIAPICALL (int, liren_internal_update, (
	LIRenRender* self,
	float        secs));

LIAPICALL (int, liren_internal_get_anisotropy, (
	const LIRenRender* self));

LIAPICALL (void, liren_internal_set_anisotropy, (
	LIRenRender* self,
	int          value));

LIAPICALL (void, liren_internal_set_camera_far, (
	LIRenRender* self,
	float        value));

LIAPICALL (void, liren_internal_set_camera_near, (
	LIRenRender* self,
	float        value));

LIAPICALL (void, liren_internal_set_camera_transform, (
	LIRenRender*          self,
	const LIMatTransform* value));

LIAPICALL (float, liren_internal_get_opengl_version, (
	LIRenRender* self));

LIAPICALL (void, liren_internal_set_material_scheme, (
	LIRenRender* self,
	const char*  value));

LIAPICALL (void, liren_internal_set_scene_ambient, (
	LIRenRender* self,
	const float* value));

LIAPICALL (void, liren_internal_set_skybox, (
	LIRenRender* self,
	const char*  value));

LIAPICALL (void, liren_internal_get_stats, (
	LIRenRender* self,
	LIRenStats*  result));

LIAPICALL (void, liren_internal_set_title, (
	LIRenRender* self,
	const char*  value));

LIAPICALL (int, liren_internal_set_videomode, (
	LIRenRender*    self,
	LIRenVideomode* mode));

LIAPICALL (int, liren_internal_get_videomodes, (
	LIRenRender*     self,
	LIRenVideomode** modes,
	int*             modes_num));

#endif
