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

#ifndef __EXT_TERRAIN_MODULE_H__
#define __EXT_TERRAIN_MODULE_H__

#include "lipsofsuna/extension.h"
#include "terrain-types.h"

struct _LIExtTerrainModule
{
	LIMaiProgram* program;
};

LIAPICALL (LIExtTerrainModule*, liext_terrain_module_new, (
	LIMaiProgram* program));

LIAPICALL (void, liext_terrain_module_free, (
	LIExtTerrainModule* self));

LIAPICALL (void, liext_terrain_module_get_memstat, (
	LIExtTerrainModule* self,
	LIMaiMemstat*       stat));

/*****************************************************************************/

LIAPICALL (void, liext_script_terrain, (
	LIScrScript* self));

/*****************************************************************************/

LIAPICALL (void, liext_terrain_unittest, ());

#endif
