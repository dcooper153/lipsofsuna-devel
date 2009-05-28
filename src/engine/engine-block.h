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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengBlock Block
 * @{
 */

#ifndef __ENGINE_BLOCK_H__
#define __ENGINE_BLOCK_H__

#include <math/lips-math.h>
#include <physics/lips-physics.h>
#include <system/lips-system.h>
#ifndef LIENG_DISABLE_GRAPHICS
#include <render/lips-render.h>
#endif
#include "engine-types.h"

#define LIENG_TILE_INDEX(x, y, z) ((x) + LIENG_TILES_PER_LINE * (y) + LIENG_TILES_PER_PLANE * (z))
#define LIENG_TILE_WIDTH 1.0f
#define LIENG_TILES_PER_LINE 16
#define LIENG_TILES_PER_PLANE (LIENG_TILES_PER_LINE * LIENG_TILES_PER_LINE)
#define LIENG_TILES_PER_BLOCK (LIENG_TILES_PER_LINE * LIENG_TILES_PER_LINE * LIENG_TILES_PER_LINE)
#define LIENG_HEIGHTS_PER_LINE (LIENG_TILES_PER_LINE + 1)
#define LIENG_HEIGHTS_PER_BLOCK (LIENG_TILES_PER_LINE + 1)
#define LIENG_BLOCK_WIDTH (LIENG_TILE_WIDTH * LIENG_TILES_PER_LINE)

enum
{
	LIENG_BLOCK_TYPE_FULL,
	LIENG_BLOCK_TYPE_TILES
};

typedef struct _liengBlock liengBlock;
typedef struct _liengBlockFull liengBlockFull;
typedef struct _liengBlockTiles liengBlockTiles;

struct _liengBlockFull
{
	liengTile terrain;
};

struct _liengBlockTiles
{
	uint8_t flags;
	liengTile tiles[LIENG_TILES_PER_BLOCK];
};

struct _liengBlock
{
	uint8_t type;
	uint8_t dirty;
	liphyShape* shape;
	liphyObject* physics;
#ifndef LIENG_DISABLE_GRAPHICS
	lirndObject* render;
#endif
	union
	{
		liengBlockFull full;
		liengBlockTiles* tiles;
	};
};

void
lieng_block_free (liengBlock* self);

void
lieng_block_fill (liengBlock* self,
                  liengTile   terrain);

int
lieng_block_fill_aabb (liengBlock*      self,
                       const limatAabb* box,
                       liengTile        terrain);

int
lieng_block_fill_sphere (liengBlock*        self,
                         const limatVector* center,
                         float              radius,
                         liengTile          terrain);

void
lieng_block_optimize (liengBlock* self);

int
lieng_block_read (liengBlock* self,
                  liReader*   reader);

int
lieng_block_write (liengBlock*  self,
                   liarcWriter* writer);

liengTile
lieng_block_get_voxel (liengBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z);

int
lieng_block_set_voxel (liengBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z,
                       liengTile   terrain);

#endif

/** @} */
/** @} */
