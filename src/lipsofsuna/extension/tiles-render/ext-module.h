/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __EXT_TILES_RENDER_MODULE_H__
#define __EXT_TILES_RENDER_MODULE_H__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/render.h"
#include "lipsofsuna/voxel.h"

typedef struct _LIExtBlock LIExtBlock;
typedef struct _LIExtBuildTask LIExtBuildTask;
typedef struct _LIExtModule LIExtModule;

#define LIEXT_SCRIPT_TILES_RENDER "TilesRender"

struct _LIExtModule
{
	LIAlgMemdic* blocks;
	LICalHandle calls[3];
	LIMaiProgram* program;
	LIRenRender* render;
	LIVoxManager* voxels;
	struct
	{
		LISysAsyncCall* worker;
		LISysMutex* mutex;
		LIExtBuildTask* pending;
		LIExtBuildTask* completed;
	} tasks;
};

LIExtModule* liext_tiles_render_new (
	LIMaiProgram* program);

void liext_tiles_render_free (
	LIExtModule* self);

int liext_tiles_render_build_all (
	LIExtModule* self);

int liext_tiles_render_build_block (
	LIExtModule*    self,
	LIVoxBlockAddr* addr);

void liext_tiles_render_clear_all (
	LIExtModule* self);

#endif
