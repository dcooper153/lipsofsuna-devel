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

#ifndef __RENDER_H__
#define __RENDER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/image.h>
#include <lipsofsuna/system.h>
#include "render-buffer.h"
#include "render-context.h"
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
	char* datadir;
	LIAlgPtrdic* scenes;
	LIAlgStrdic* shaders;
	LIAlgStrdic* images;
	LIAlgU32dic* models;
	LIRenContext* context;
	struct
	{
		float time;
		GLuint noise;
		GLuint depth_texture_max;
		LIRenBuffer* unit_quad;
		LIRenImage* empty_image;
	} helpers;
	struct
	{
		int offset;
		LIRenBuffer* buffer;
	} immediate;
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
	int          id));

LIAPICALL (int, liren_render_load_image, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (void, liren_render_update, (
	LIRenRender* self,
	float        secs));

LIAPICALL (LIRenContext*, liren_render_get_context, (
	LIRenRender* self));

#ifndef NDEBUG
void
liren_check_errors ();
#else
#define liren_check_errors()
#endif

#endif
