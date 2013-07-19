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

#ifndef __EXT_TERRAIN_TERRAIN_MATERIAL_H__
#define __EXT_TERRAIN_TERRAIN_MATERIAL_H__

#include "lipsofsuna/system.h"

typedef int LIExtTerrainStickType;
enum
{
	LIEXT_TERRAIN_STICK_TYPE_DEFAULT,
	LIEXT_TERRAIN_STICK_TYPE_MAX
};

typedef int LIExtTerrainDecorationType;
enum
{
	LIEXT_TERRAIN_DECORATION_TYPE_NONE,
	LIEXT_TERRAIN_DECORATION_TYPE_GRASS,
	LIEXT_TERRAIN_DECORATION_TYPE_MAX
};

typedef struct _LIExtTerrainMaterial LIExtTerrainMaterial;
struct _LIExtTerrainMaterial
{
	LIExtTerrainStickType stick_type;
	LIExtTerrainDecorationType decoration_type;
	int texture_top;
	int texture_bottom;
	int texture_side;
	int texture_decoration;
};

LIAPICALL (void, liext_terrain_material_init, (
	LIExtTerrainMaterial* self,
	int                   texture));

#endif
