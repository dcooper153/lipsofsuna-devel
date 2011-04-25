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
#include "lipsofsuna/image.h"
#include "render-types.h"
#include "render32/render-attribute.h"
#include "render32/render-buffer-texture.h"
#include "render32/render-context.h"
#include "render32/render-lighting.h"
#include "render32/render-mesh.h"
#include "render32/render-particles.h"
#include "render32/render-program.h"
#include "render32/render-sort.h"
#include "render32/render-uniform.h"

/* #define LIREN_ENABLE_PROFILING */

struct _LIRenBuffer
{
	int type;
	GLuint index_buffer;
	GLuint vertex_array;
	GLuint vertex_buffer;
	LIRenFormat vertex_format;
	struct
	{
		int count;
	} indices;
	struct
	{
		int count;
	} vertices;
};

struct _LIRenFramebuffer
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

struct _LIRenImage
{
	int added;
	int empty;
	char* name;
	char* path;
	LIImgTexture* texture;
	LIRenRender* render;
};

struct _LIRenLight
{
	int directional;
	int enabled;
	float priority;
	float cutoff;
	float exponent;
	float shadow_far;
	float shadow_near;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float equation[3];
	LIMatAabb bounds;
	LIMatMatrix projection;
	LIMatMatrix modelview;
	LIMatMatrix modelview_inverse;
	LIMatTransform transform;
	const LIMdlNode* node;
	LIRenScene* scene;
	struct
	{
		float pos_world[3];
		float pos_view[3];
		float dir_world[3];
		float dir_view[3];
		float spot[3];
		LIMatMatrix matrix;
	} cache;
	struct
	{
		GLuint fbo;
		GLuint map;
	} shadow;
};

struct _LIRenMaterial
{
	int flags;
	float parameters[4];
	float shininess;
	float diffuse[4];
	float specular[4];
	float strand_start;
	float strand_end;
	float strand_shape;
	LIRenShader* shader;
	struct
	{
		int count;
		LIRenTexture* array;
	} textures;
};

typedef struct _LIRenModelGroup LIRenModelGroup;
struct _LIRenModelGroup
{
	int start;
	int count;
	LIMatVector center;
};

struct _LIRenModel
{
	int id;
	LIMatAabb bounds;
	LIRenMesh mesh;
	LIRenParticles particles;
	LIRenRender* render;
	struct
	{
		int count;
		LIRenModelGroup* array;
	} groups;
	struct
	{
		int count;
		LIRenMaterial** array;
	} materials;
};

struct _LIRenObject
{
	int id;
	int realized;
	float sort;
	void* userdata;
	LIMatAabb bounds;
	LIMatTransform transform;
	LIMdlPose* pose;
	LIRenScene* scene;
	LIRenModel* model;
	struct
	{
		int width;
		int height;
		GLuint depth;
		GLuint map;
		GLuint fbo[6];
	} cubemap;
	struct
	{
		int count;
		LIRenLight** array;
	} lights;
	struct
	{
		LIMatVector center;
		LIMatMatrix matrix;
	} orientation;
	struct
	{
		int loop;
		float time;
	} particle;
};

struct _LIRenRender
{
	int anisotrophy;
	LIAlgPtrdic* scenes;
	LIAlgRandom random;
	LIAlgStrdic* shaders;
	LIAlgStrdic* images;
	LIAlgU32dic* models;
	LIAlgPtrdic* models_ptr;
	LIPthPaths* paths;
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

struct _LIRenScene
{
	float time;
	LIAlgPtrdic* groups;
	LIAlgU32dic* objects;
	LIRenRender* render;
	LIRenSort* sort;
	LIRenLighting* lighting;
	struct
	{
		int postproc_passes;
		int rendering;
		GLint original_viewport[4];
		LIRenContext* context;
		LIRenFramebuffer* framebuffer;
	} state;
	struct
	{
		LIRenObject* model;
	} sky;
};

struct _LIRenShader
{
	int sort;
	char* name;
	LIRenRender* render;
	LIRenProgram passes[LIREN_SHADER_PASS_COUNT];
};

#endif
