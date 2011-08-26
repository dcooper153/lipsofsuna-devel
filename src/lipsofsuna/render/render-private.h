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

#ifndef __RENDER_PRIVATE_H__
#define __RENDER_PRIVATE_H__

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/font.h"
#include "render-image.h"
#include "render-types.h"
#include "render21/render.h"
#include "render21/render-light.h"
#include "render21/render-scene.h"
#include "render32/render.h"
#include "render32/render-buffer.h"
#include "render32/render-framebuffer.h"
#include "render32/render-image.h"
#include "render32/render-light.h"
#include "render32/render-material.h"
#include "render32/render-model.h"
#include "render32/render-object.h"
#include "render32/render-scene.h"
#include "render32/render-shader.h"

/* #define LIREN_ENABLE_PROFILING */

typedef struct _LIRenLight LIRenLight;
typedef struct _LIRenModel LIRenModel;
typedef struct _LIRenOverlay LIRenOverlay;
typedef struct _LIRenOverlayElement LIRenOverlayElement;

struct _LIRenBuffer
{
	LIRenBuffer21* v21;
	LIRenBuffer32* v32;
};

struct _LIRenFramebuffer
{
	LIRenFramebuffer21* v21;
	LIRenFramebuffer32* v32;
};

struct _LIRenImage
{
	LIRenRender* render;
	LIRenImage21* v21;
	LIRenImage32* v32;
};

struct _LIRenLight
{
	int id;
	LIRenRender* render;
	LIRenLight21* v21;
	LIRenLight32* v32;
};

struct _LIRenModel
{
	int id;
	LIRenRender* render;
	LIRenModel21* v21;
	LIRenModel32* v32;
};

struct _LIRenObject
{
	int id;
	LIRenModel* model;
	LIRenRender* render;
	LIRenObject21* v21;
	LIRenObject32* v32;
};

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

struct _LIRenRender
{
	LIAlgRandom random;
	LIAlgStrdic* images;
	LIAlgStrdic* shaders;
	LIAlgU32dic* lights;
	LIAlgU32dic* models;
	LIAlgU32dic* objects;
	LIAlgU32dic* overlays;
	LIRenOverlay* root_overlay;
	LIRenRender21* v21;
	LIRenRender32* v32;
};

struct _LIRenShader
{
	LIRenRender* render;
	LIRenShader21* v21;
	LIRenShader32* v32;
};

LIAPICALL (LIRenModel*, liren_render_find_model, (
	LIRenRender* self,
	int          id));

#endif
