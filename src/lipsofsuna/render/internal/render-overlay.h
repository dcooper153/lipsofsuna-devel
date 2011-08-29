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

#ifndef __RENDER_INTERNAL_OVERLAY_H__
#define __RENDER_INTERNAL_OVERLAY_H__

#include "render-types.h"
#include "../font/font.h"
#include "../font/font-layout.h"

struct _LIRenOverlay
{
	int id;
	int behind;
	int visible;
	LIMatVector position;
	LIRenBuffer* buffer;
	LIRenOverlay* parent;
	LIRenRender* render;
	struct
	{
		int count;
		LIRenOverlayElement* array;
	} elements;
	struct
	{
		int count;
		LIRenOverlay** array;
	} overlays;
	struct
	{
		int count;
		LIRenVertex* array;
	} vertices;
	struct
	{
		int enabled;
		int samples;
		int hdr;
		GLint viewport[4];
		LIMatMatrix modelview;
		LIMatMatrix projection;
		LIMatFrustum frustum;
		LIRenFramebuffer* framebuffer;
		LIRenPassRender* render_passes;
		int render_passes_num;
		LIRenPassPostproc* postproc_passes;
		int postproc_passes_num;
	} scene;
};

struct _LIRenOverlayElement
{
	int buffer_start;
	int buffer_count;
	float color[4];
	int scissor_enabled;
	GLint scissor_rect[4];
	LIFntFont* font;
	LIRenImage* image;
	LIRenShader* shader;
};

#endif
