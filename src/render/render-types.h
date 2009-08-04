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

#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__

#include <video/lips-video.h>

enum _lirndFlags
{
	LIRND_FLAG_FIXED     = 0x01, /* Force fixed function. */
	LIRND_FLAG_SHADOW0   = 0x02, /* Sun casts shadows. */
	LIRND_FLAG_SHADOW1   = 0x04, /* Lamps cast shadows. */
	LIRND_FLAG_LIGHTING  = 0x08, /* Render with lighting. */
	LIRND_FLAG_TEXTURING = 0x10, /* Render with texturing. */
};

enum _lirndSelect
{
	LIRND_SELECT_OBJECT,
	LIRND_SELECT_SECTOR,
};

typedef struct _lirndBuffer lirndBuffer;
typedef struct _lirndContext lirndContext;
typedef enum _lirndFlags lirndFlags;
typedef struct _lirndFormat lirndFormat;
typedef struct _lirndLight lirndLight;
typedef struct _lirndLighting lirndLighting;
typedef struct _lirndMaterial lirndMaterial;
typedef struct _lirndModel lirndModel;
typedef struct _lirndObject lirndObject;
typedef struct _lirndRender lirndRender;
typedef struct _lirndResources lirndResources;
typedef struct _lirndScene lirndScene;
typedef enum _lirndSelect lirndSelect;
typedef struct _lirndSelection lirndSelection;
typedef struct _lirndShader lirndShader;
typedef struct _lirndTexture lirndTexture;
typedef void (*lirndCallback) (lirndContext*, lirndObject*, void*);

struct _lirndFormat
{
	int size;
	int tex_count;
	int tex_formats[3];
	int tex_offsets[3];
	int nml_format;
	int nml_offset;
	int vtx_format;
	int vtx_offset;
};

struct _lirndSelection
{
	lirndSelect type;
	float depth;
	uint32_t object;
	limatVector point;
};

#endif
