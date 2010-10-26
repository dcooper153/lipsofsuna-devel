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
	LIRenRender* render;
	LIRenScene* scene;
	LIRenShader* shader;
	LIRenBuffer* buffer;
	LIMatFrustum frustum;
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
		unsigned int lights : 1;
		unsigned int material : 1;
		unsigned int matrix_model : 1;
		unsigned int matrix_projection : 1;
		unsigned int matrix_view : 1;
		unsigned int shader : 1;
		unsigned int textures : 1;
		unsigned int uniforms : 1;
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
		int count;
		LIRenLight* array[9];
	} lights;
	struct
	{
		float parameters[4];
		float shininess;
		float diffuse[4];
		float specular[4];
	} material;
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

LIAPICALL (void, liren_context_set_blend, (
	LIRenContext* self,
	int           enable,
	GLenum        blend_src,
	GLenum        blend_dst));

LIAPICALL (void, liren_context_set_buffer, (
	LIRenContext* self,
	LIRenBuffer*  vertex));

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

LIAPICALL (void, liren_context_set_lights, (
	LIRenContext* self,
	LIRenLight**  value,
	int           count));

LIAPICALL (void, liren_context_set_material, (
	LIRenContext*        self,
	const LIRenMaterial* value));

LIAPICALL (void, liren_context_set_material_shader, (
	LIRenContext*        self,
	const LIRenMaterial* value));

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

LIAPICALL (void, liren_context_set_shader, (
	LIRenContext* self,
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
