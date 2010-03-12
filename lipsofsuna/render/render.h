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
 * \addtogroup LIRenRender Render
 * @{
 */

#ifndef __RENDER_H__
#define __RENDER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/image.h>
#include <lipsofsuna/system.h>
#include "render-image.h"
#include "render-light.h"
#include "render-lighting.h"
#include "render-material.h"
#include "render-object.h"
#include "render-shader.h"
#include "render-types.h"

/* #define LIREN_ENABLE_PROFILING */

struct _LIRenRender
{
	LIAlgPtrdic* scenes;
	LIAlgStrdic* shaders;
	LIAlgStrdic* images;
	LIAlgStrdic* models;
	LIAlgPtrdic* models_inst;
	struct
	{
		char* dir;
		int global_shadows;
		int local_shadows;
		int light_count;
	} config;
	struct
	{
		float time;
		GLuint noise;
		GLuint depth_texture_max;
		LIRenImage* empty_image;
	} helpers;
	struct
	{
		int enabled;
		LIRenShader* shader;
		LIRenShader* shadowmap;
		LIRenShader* fixed;
	} shader;
#ifdef LIREN_ENABLE_PROFILING
	struct
	{
		int objects;
		int materials;
		int faces;
		int vertices;
	} profiling;
#endif
};

LIAPICALL (LIRenRender*, liren_render_new, (
	const char* dir));

LIAPICALL (void, liren_render_free, (
	LIRenRender* self));

LIAPICALL (LIRenShader*, liren_render_find_shader, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (LIRenImage*, liren_render_find_image, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (LIRenModel*, liren_render_find_model, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (int, liren_render_load_image, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (int, liren_render_load_model, (
	LIRenRender* self,
	const char*  name,
	LIMdlModel*  model));

LIAPICALL (void, liren_render_update, (
	LIRenRender* self,
	float        secs));

LIAPICALL (void, liren_render_set_global_shadows, (
	LIRenRender* self,
	int          value));

LIAPICALL (int, liren_render_get_light_count, (
	const LIRenRender* self));

LIAPICALL (void, liren_render_set_light_count, (
	LIRenRender* self,
	int          count));

LIAPICALL (void, liren_render_set_local_shadows, (
	LIRenRender* self,
	int          value));

LIAPICALL (int, liren_render_get_shaders_enabled, (
	const LIRenRender* self));

LIAPICALL (void, liren_render_set_shaders_enabled, (
	LIRenRender* self,
	int          value));

#ifndef NDEBUG
void
liren_check_errors ();
#else
#define liren_check_errors()
#endif

#endif

/** @} */
/** @} */
