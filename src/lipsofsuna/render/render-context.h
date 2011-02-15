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

#ifndef __RENDER_CONTEXT_H__
#define __RENDER_CONTEXT_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/model.h>
#include "render-buffer.h"
#include "render-light.h"
#include "render-material.h"
#include "render-scene.h"
#include "render-texture.h"
#include "render-types.h"
#include "render-uniforms.h"

#define LIREN_CONTEXT_MAX_LIGHTS 3

typedef struct _LIRenContextTexture LIRenContextTexture;
struct _LIRenContextTexture
{
	GLuint texture;
};

struct _LIRenContext
{
	int deferred;
	int incomplete;
	int shadows;
	GLuint array;
	LIRenLight* light;
	LIRenRender* render;
	LIRenScene* scene;
	LIRenShader* shader;
	LIMatFrustum frustum;
	LIRenUniforms uniforms;
	GLuint shadow_texture;
	int alpha_to_coverage;
	int color_write;
	int shader_pass;
	struct
	{
		int enable;
		GLenum blend_src;
		GLenum blend_dst;
	} blend;
	struct
	{
		unsigned int blend : 1;
		unsigned int buffer : 1;
		unsigned int cull : 1;
		unsigned int depth : 1;
		unsigned int shader : 1;
	} changed;
	struct
	{
		int enable;
		GLenum front_face;
	} cull;
	struct
	{
		int enable_test;
		int enable_write;
		int depth_func;
	} depth;
	struct
	{
		LIMatMatrix model;
		LIMatMatrix modelview;
		LIMatMatrix modelviewinverse;
		LIMatMatrix projection;
		LIMatMatrix projectioninverse;
		LIMatMatrix view;
	} matrix;
	struct
	{
		int enabled;
		int rect[4];
	} scissor;
	struct
	{
		int count;
		LIRenContextTexture array[9];
	} textures;
};

LIAPICALL (void, liren_context_init, (
	LIRenContext* self));

LIAPICALL (void, liren_context_bind, (
	LIRenContext* self));

LIAPICALL (void, liren_context_render_array, (
	LIRenContext* self,
	GLenum        type,
	int           start,
	int           count));

LIAPICALL (int, liren_context_render_immediate, (
	LIRenContext*      self,
	GLenum             type,
	const LIRenVertex* vertices,
	int                count));

LIAPICALL (void, liren_context_render_indexed, (
	LIRenContext* self,
	int           start,
	int           count));

LIAPICALL (void, liren_context_set_alpha_to_coverage, (
	LIRenContext* self,
	int           value));

LIAPICALL (void, liren_context_set_blend, (
	LIRenContext* self,
	int           enable,
	GLenum        blend_src,
	GLenum        blend_dst));

LIAPICALL (void, liren_context_set_buffer, (
	LIRenContext* self,
	LIRenBuffer*  vertex));

LIAPICALL (void, liren_context_set_color_write, (
	LIRenContext* self,
	int           value));

LIAPICALL (void, liren_context_set_cull, (
	LIRenContext* self,
	int           enable,
	int           front_face));

LIAPICALL (int, liren_context_get_deferred, (
	LIRenContext* self));

LIAPICALL (void, liren_context_set_deferred, (
	LIRenContext* self,
	int           value));

LIAPICALL (void, liren_context_set_depth, (
	LIRenContext* self,
	int           enable_test,
	int           enable_write,
	GLenum        depth_func));

LIAPICALL (void, liren_context_set_diffuse, (
	LIRenContext* self,
	const float*  value));

LIAPICALL (void, liren_context_set_flags, (
	LIRenContext* self,
	int           value));

LIAPICALL (void, liren_context_set_frustum, (
	LIRenContext*       self,
	const LIMatFrustum* frustum));

LIAPICALL (void, liren_context_set_light, (
	LIRenContext* self,
	int           index,
	LIRenLight*   value));

LIAPICALL (void, liren_context_set_material, (
	LIRenContext*        self,
	const LIRenMaterial* value));

LIAPICALL (void, liren_context_set_mesh, (
	LIRenContext* self,
	LIRenMesh*    mesh));

LIAPICALL (void, liren_context_set_modelmatrix, (
	LIRenContext*      self,
	const LIMatMatrix* value));

LIAPICALL (void, liren_context_set_param, (
	LIRenContext* self,
	const float*  value));

LIAPICALL (void, liren_context_set_viewmatrix, (
	LIRenContext*      self,
	const LIMatMatrix* value));

LIAPICALL (void, liren_context_set_projection, (
	LIRenContext*      self,
	const LIMatMatrix* value));

LIAPICALL (void, liren_context_set_scene, (
	LIRenContext* self,
	LIRenScene*   scene));

LIAPICALL (int, liren_context_get_scissor, (
	LIRenContext* self,
	int*          x,
	int*          y,
	int*          w,
	int*          h));

LIAPICALL (void, liren_context_set_scissor, (
	LIRenContext* self,
	int           enabled,
	int           x,
	int           y,
	int           w,
	int           h));

LIAPICALL (void, liren_context_set_shader, (
	LIRenContext* self,
	int           pass,
	LIRenShader*  value));

LIAPICALL (void, liren_context_set_textures, (
	LIRenContext* self,
	LIRenTexture* value,
	int           count));

LIAPICALL (void, liren_context_set_textures_raw, (
	LIRenContext* self,
	GLuint*       value,
	int           count));

#endif
