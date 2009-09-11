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

/**
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxBlock Block
 * @{
 */

#ifndef __VOXEL_PRIVATE_H__
#define __VOXEL_PRIVATE_H__

typedef struct _livoxBlockFull livoxBlockFull;
typedef struct _livoxBlockTiles livoxBlockTiles;

struct _livoxBlock
{
	uint8_t type;
	uint8_t dirty;
	uint16_t stamp;
	liphyShape* shape;
	liphyObject* physics;
#ifndef LIVOX_DISABLE_GRAPHICS
	lirndObject* render;
#endif
	livoxVoxel tiles[LIVOX_TILES_PER_BLOCK];
};

struct _livoxSector
{
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t dirty;
	livoxBlock blocks[LIVOX_BLOCKS_PER_SECTOR];
	livoxManager* manager;
	limatVector origin;
};

#endif
