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

#ifndef __VOXEL_TYPES_H__
#define __VOXEL_TYPES_H__

#include <lipsofsuna/system.h>

enum
{
	LIVOX_OCCLUDE_ALL = 0x3F,
	LIVOX_OCCLUDE_XNEG = 0x01,
	LIVOX_OCCLUDE_XPOS = 0x02,
	LIVOX_OCCLUDE_YNEG = 0x04,
	LIVOX_OCCLUDE_YPOS = 0x08,
	LIVOX_OCCLUDE_ZNEG = 0x10,
	LIVOX_OCCLUDE_ZPOS = 0x20,
	LIVOX_OCCLUDE_OCCLUDER = 0x40,
	LIVOX_OCCLUDE_OCCLUDED = 0x80
};

typedef struct _LIVoxVoxel LIVoxVoxel;
struct _LIVoxVoxel
{
	uint16_t type;
	uint8_t damage;
	uint8_t rotation;
};

typedef struct _LIVoxManager LIVoxManager;
typedef struct _LIVoxMaterial LIVoxMaterial;
typedef struct _LIVoxSector LIVoxSector;

#endif
