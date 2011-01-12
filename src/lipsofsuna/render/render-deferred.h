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

#ifndef __RENDER_DEFERRED_H__
#define __RENDER_DEFERRED_H__

#include "render.h"
#include "render-types.h"

struct _LIRenDeferred
{
	int hdr;
	int width;
	int height;
	int samples;
	GLuint render_framebuffer;
	GLuint render_textures[2];
	GLuint postproc_framebuffers[2];
	GLuint postproc_textures[3];
	LIRenRender* render;
};

LIAPICALL (LIRenDeferred*, liren_deferred_new, (
	LIRenRender* render,
	int          width,
	int          height,
	int          samples,
	int          hdr));

LIAPICALL (void, liren_deferred_free, (
	LIRenDeferred* self));

LIAPICALL (int, liren_deferred_resize, (
	LIRenDeferred* self,
	int            width,
	int            height,
	int            samples,
	int            hdr));

#endif
