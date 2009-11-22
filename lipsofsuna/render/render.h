/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndRender Render
 * @{
 */

#ifndef __RENDER_H__
#define __RENDER_H__

#include <algorithm/lips-algorithm.h>
#include <config/lips-config.h>
#include <image/lips-image.h>
#include <system/lips-system.h>
#include "render-light.h"
#include "render-lighting.h"
#include "render-material.h"
#include "render-object.h"
#include "render-resources.h"
#include "render-shader.h"
#include "render-types.h"

/* #define LIRND_ENABLE_PROFILING */

struct _lirndRender
{
	lialgPtrdic* scenes;
	lirndResources* resources;
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
	} helpers;
	struct
	{
		int enabled;
		lirndShader* shader;
		lirndShader* shadowmap;
		lirndShader* fixed;
	} shader;
#ifdef LIRND_ENABLE_PROFILING
	struct
	{
		int objects;
		int materials;
		int faces;
		int vertices;
	} profiling;
#endif
};

lirndRender*
lirnd_render_new (const char* dir);

void
lirnd_render_free (lirndRender* self);

lirndShader*
lirnd_render_find_shader (lirndRender* self,
                          const char*  name);

lirndImage*
lirnd_render_find_image (lirndRender* self,
                         const char*  name);

lirndModel*
lirnd_render_find_model (lirndRender* self,
                         const char*  name);

int
lirnd_render_load_image (lirndRender* self,
                         const char*  name);

int
lirnd_render_load_model (lirndRender* self,
                         const char*  name,
                         limdlModel*  model);

void
lirnd_render_update (lirndRender* self,
                     float        secs);

void
lirnd_render_set_global_shadows (lirndRender* self,
                                 int          value);

int
lirnd_render_get_light_count (const lirndRender* self);

void
lirnd_render_set_light_count (lirndRender* self,
                              int          count);

void
lirnd_render_set_local_shadows (lirndRender* self,
                                int          value);

int
lirnd_render_get_shaders_enabled (const lirndRender* self);

void
lirnd_render_set_shaders_enabled (lirndRender* self,
                                  int          value);

#endif

/** @} */
/** @} */
