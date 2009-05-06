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
 * \addtogroup lirndShader Shader
 * @{
 */

#ifndef __RENDER_SHADER_H__
#define __RENDER_SHADER_H__

#include <image/lips-image.h>
#include <model/lips-model.h>
#include "render-light.h"
#include "render-types.h"
#include "render-uniform.h"

struct _lirndShader
{
	char* name;
	GLint program;
	GLint vertex;
	GLint fragment;
	lirndRender* render;
	struct
	{
		int count;
	} lights;
	struct
	{
		int count;
		lirndUniform* array;
	} uniforms;
};

lirndShader*
lirnd_shader_new (lirndRender* render);

lirndShader*
lirnd_shader_new_from_data (lirndRender* render,
                            liReader*    reader);

lirndShader*
lirnd_shader_new_from_file (lirndRender* render,
                            const char*  path);

void
lirnd_shader_free (lirndShader* self);

#endif

/** @} */
/** @} */
