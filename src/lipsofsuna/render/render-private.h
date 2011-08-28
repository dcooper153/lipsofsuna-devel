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
#include "render-types.h"
#include "internal/render-buffer.h"
#include "internal/render-image.h"
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
typedef struct _LIRenOverlay LIRenOverlay;
typedef struct _LIRenOverlayElement LIRenOverlayElement;

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
};

LIAPICALL (void, liren_render_draw_clipped_buffer, (
	LIRenRender*       self,
	LIRenShader*       shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	int                start,
	int                count,
	LIRenBuffer*       buffer));

LIAPICALL (void, liren_render_draw_indexed_triangles_T2V3, (
	LIRenRender*      self,
	LIRenShader*      shader,
	LIMatMatrix*      matrix,
	GLuint            texture,
	const float*      diffuse,
	const float*      vertex_data,
	const LIRenIndex* index_data,
	int               index_count));

LIAPICALL (LIRenImage*, liren_render_find_image, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (LIRenModel*, liren_render_find_model, (
	LIRenRender* self,
	int          id));

LIAPICALL (LIRenShader*, liren_render_find_shader, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (void, liren_render_render_scene, (
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

#endif
