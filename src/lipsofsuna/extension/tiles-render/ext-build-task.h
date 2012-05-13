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

#ifndef __EXT_TILES_RENDER_BUILD_TASK_H__
#define __EXT_TILES_RENDER_BUILD_TASK_H__

#include "ext-module.h"

struct _LIExtBuildTask
{
	LIVoxBlockAddr addr;
	LIVoxBuilder* builder;
	LIMatVector offset;
	LIMdlModel* model;
	LIExtBuildTask* next;
};

LIAPICALL (LIExtBuildTask*, liext_tiles_build_task_new, (
	LIExtModule*      module,
	LIVoxUpdateEvent* event));

LIAPICALL (void, liext_tiles_build_task_free, (
	LIExtBuildTask* self));

LIAPICALL (int, liext_tiles_build_task_compare, (
	LIExtBuildTask* self,
	LIExtBuildTask* task));

LIAPICALL (int, liext_tiles_build_task_process, (
	LIExtBuildTask* self));

#endif
