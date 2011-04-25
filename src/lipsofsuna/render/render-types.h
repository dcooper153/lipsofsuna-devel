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

#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/video.h>

enum _LIRenFlags
{
	LIREN_FLAG_FIXED     = 0x01, /* Force fixed function. */
	LIREN_FLAG_SHADOW0   = 0x02, /* Sun casts shadows. */
	LIREN_FLAG_SHADOW1   = 0x04, /* Lamps cast shadows. */
	LIREN_FLAG_LIGHTING  = 0x08, /* Render with lighting. */
	LIREN_FLAG_TEXTURING = 0x10, /* Render with texturing. */
};

enum _LIRenSelect
{
	LIREN_SELECT_OBJECT,
	LIREN_SELECT_SECTOR,
};

typedef struct _LIRenBuffer LIRenBuffer;
typedef struct _LIRenContext LIRenContext;
typedef enum _LIRenFlags LIRenFlags;
typedef struct _LIRenFormat LIRenFormat;
typedef struct _LIRenFramebuffer LIRenFramebuffer;
typedef struct _LIRenLight LIRenLight;
typedef struct _LIRenLighting LIRenLighting;
typedef struct _LIRenMaterial LIRenMaterial;
typedef struct _LIRenModel LIRenModel;
typedef struct _LIRenObject LIRenObject;
typedef struct _LIRenPassRender LIRenPassRender;
typedef struct _LIRenPassPostproc LIRenPassPostproc;
typedef struct _LIRenRender LIRenRender;
typedef struct _LIRenResources LIRenResources;
typedef struct _LIRenScene LIRenScene;
typedef enum _LIRenSelect LIRenSelect;
typedef struct _LIRenSelection LIRenSelection;
typedef struct _LIRenShader LIRenShader;
typedef struct _LIRenSortgroup LIRenSortgroup;
typedef struct _LIRenSortface LIRenSortface;
typedef struct _LIRenSort LIRenSort;
typedef struct _LIRenTexture LIRenTexture;
typedef struct _LIRenVertex LIRenVertex;
typedef void (*lirenCallback) (LIRenContext*, LIRenObject*, void*);

struct _LIRenFormat
{
	int size;
	int tex_format;
	int tex_offset;
	int nml_format;
	int nml_offset;
	int vtx_format;
	int vtx_offset;
};

struct _LIRenPassRender
{
	int pass;
	int sort;
};

struct _LIRenPassPostproc
{
	int mipmap;
	char* shader;
};

struct _LIRenSelection
{
	LIRenSelect type;
	float depth;
	uint32_t object;
	LIMatVector point;
};

struct _LIRenVertex
{
	float coord[3];
	float normal[3];
	float texcoord[2];
};

#endif
