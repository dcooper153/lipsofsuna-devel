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

#ifndef __VOXEL_TYPES_H__
#define __VOXEL_TYPES_H__

#include <system/lips-system.h>

/* FIXME */
#define LIVOX_ENABLE_GRAPHICS

#define LIVOX_SECTOR_INDEX_FROM_POINT(p) LIVOX_SECTOR_INDEX((int)((p).x / LIVOX_SECTOR_WIDTH), (int)((p).y / LIVOX_SECTOR_WIDTH), (int)((p).z / LIVOX_SECTOR_WIDTH))
#define LIVOX_SECTOR_INDEX(x, y, z) ((x) + LIVOX_SECTORS_PER_LINE * (y) + LIVOX_SECTORS_PER_PLANE * (z))
#define LIVOX_SECTOR_WIDTH (LIVOX_BLOCKS_PER_LINE * LIVOX_BLOCK_WIDTH)
#define LIVOX_SECTORS_PER_LINE 256
#define LIVOX_SECTORS_PER_PLANE (LIVOX_SECTORS_PER_LINE * LIVOX_SECTORS_PER_LINE)
#define LIVOX_SECTORS_PER_WORLD (LIVOX_SECTORS_PER_LINE * LIVOX_SECTORS_PER_LINE * LIVOX_SECTORS_PER_LINE)
#define LIVOX_BLOCK_INDEX(x, y, z) ((x) + LIVOX_BLOCKS_PER_LINE * (y) + LIVOX_BLOCKS_PER_PLANE * (z))
#define LIVOX_BLOCK_WIDTH (LIVOX_TILE_WIDTH * LIVOX_TILES_PER_LINE)
#define LIVOX_BLOCKS_PER_LINE 4
#define LIVOX_BLOCKS_PER_PLANE (LIVOX_BLOCKS_PER_LINE * LIVOX_BLOCKS_PER_LINE)
#define LIVOX_BLOCKS_PER_SECTOR (LIVOX_BLOCKS_PER_LINE * LIVOX_BLOCKS_PER_LINE * LIVOX_BLOCKS_PER_LINE)
#define LIVOX_TILE_INDEX(x, y, z) ((x) + LIVOX_TILES_PER_LINE * (y) + LIVOX_TILES_PER_PLANE * (z))
#define LIVOX_TILE_WIDTH 2.0f
#define LIVOX_TILES_PER_LINE 8
#define LIVOX_TILES_PER_PLANE (LIVOX_TILES_PER_LINE * LIVOX_TILES_PER_LINE)
#define LIVOX_TILES_PER_BLOCK (LIVOX_TILES_PER_LINE * LIVOX_TILES_PER_LINE * LIVOX_TILES_PER_LINE)

typedef struct _livoxVoxel livoxVoxel;
struct _livoxVoxel
{
	uint16_t type;
	uint8_t damage;
};

typedef struct _livoxManager livoxManager;
typedef struct _livoxMaterial livoxMaterial;
typedef struct _livoxSector livoxSector;

#endif
