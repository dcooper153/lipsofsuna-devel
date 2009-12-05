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
 * \addtogroup lirndDeferred Deferred
 * @{
 */

#ifndef __RENDER_DEFERRED_H__
#define __RENDER_DEFERRED_H__

#include "render.h"
#include "render-types.h"

struct _lirndDeferred
{
	int width;
	int height;
	GLuint deferred_fbo;
	GLuint target_fbo;
	GLuint depth_texture;
	GLuint diffuse_texture;
	GLuint specular_texture;
	GLuint normal_texture;
	GLuint target_texture;
	lirndRender* render;
};

lirndDeferred*
lirnd_deferred_new (lirndRender* render,
                    int          width,
                    int          height);

void
lirnd_deferred_free (lirndDeferred* self);

int
lirnd_deferred_resize (lirndDeferred* self,
                       int            width,
                       int            height);

void
lirnd_deferred_read_pixel (lirndDeferred* self,
                           int            x,
                           int            y,
                           int            texture,
                           float*         result);

#endif

/** @} */
/** @} */
