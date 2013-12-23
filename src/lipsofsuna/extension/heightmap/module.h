/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_HEIGHTMAP_MODULE_H__
#define __EXT_HEIGHTMAP_MODULE_H__

#include "lipsofsuna/extension.h"
#include "../image/module.h"

#define LIEXT_SCRIPT_HEIGHTMAP "Heightmap"

typedef struct _LIExtHeightmap LIExtHeightmap;
typedef struct _LIExtHeightmapHooks LIExtHeightmapHooks;
typedef struct _LIExtHeightmapModule LIExtHeightmapModule;

/*****************************************************************************/

struct _LIExtHeightmap
{
	int id;
	int size;
	int visible;
	float min;
	float max;
	float spacing;
	float* heights;
	void* physics_data;
	void* render_data;
	LIExtHeightmapModule* module;
	LIMatVector position;
};

LIAPICALL (LIExtHeightmap*, liext_heightmap_new, (
	LIExtHeightmapModule* module,
	LIImgImage*           image,
	const LIMatVector*    position,
	int                   size,
	float                 spacing,
	float                 scaling));

LIAPICALL (void, liext_heightmap_free, (
	LIExtHeightmap* self));

LIAPICALL (int, liext_heightmap_get_height, (
	LIExtHeightmap* self,
	float           x,
	float           z,
	int             clamp,
	float*          result));

/*****************************************************************************/

typedef void (*LIExtHeightmapInitFunc)(void*, LIExtHeightmap*);
typedef void (*LIExtHeightmapFreeFunc)(void*, LIExtHeightmap*);

struct _LIExtHeightmapHooks
{
	void* data;
	LIExtHeightmapInitFunc init;
	LIExtHeightmapFreeFunc free;
};

/*****************************************************************************/

struct _LIExtHeightmapModule
{
	LIAlgU32dic* heightmaps;
	LIMaiProgram* program;
	LIExtHeightmapHooks* physics_hooks;
	LIExtHeightmapHooks* render_hooks;
};

LIAPICALL (LIExtHeightmapModule*, liext_heightmap_module_new, (
	LIMaiProgram* program));

LIAPICALL (void, liext_heightmap_module_free, (
	LIExtHeightmapModule* self));

LIAPICALL (void, liext_heightmap_module_get_memstat, (
	LIExtHeightmapModule* self,
	LIMaiMemstat*         stat));

/*****************************************************************************/

LIAPICALL (void, liext_script_heightmap, (
	LIScrScript* self));

#endif
