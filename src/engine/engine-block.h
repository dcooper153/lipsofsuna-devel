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

#include <physics/lips-physics.h>
#include <system/lips-system.h>
#ifndef LIENG_DISABLE_GRAPHICS
#include <render/lips-render.h>
#endif
#include "engine-types.h"

enum
{
	LIENG_BLOCK_TYPE_FULL,
	LIENG_BLOCK_TYPE_HEIGHT,
	LIENG_BLOCK_TYPE_MULTIPLE,
	LIENG_BLOCK_TYPE_TILES
};

typedef struct _liengBlock liengBlock;
typedef struct _liengBlockFull liengBlockFull;
typedef struct _liengBlockHeight liengBlockHeight;
typedef struct _liengBlockMultiple liengBlockMultiple;
typedef struct _liengBlockTiles liengBlockTiles;

struct _liengBlockFull
{
	liengTile terrain;
};

struct _liengBlockHeight
{
	uint8_t flags;
	uint8_t terrain;
	uint8_t heights[81];
};

struct _liengBlockMultiple
{
	uint8_t count;
	uint8_t* types;
	liengBlock* blocks;
};

struct _liengBlockTiles
{
	uint8_t flags;
	liengTile tiles[512];
};

struct _liengBlock
{
	uint8_t type;
	liphyShape* shape;
	liphyObject* physics;
#ifndef LIENG_DISABLE_GRAPHICS
	//lirndBlock* render;
#endif
	union
	{
		liengBlockFull full;
		liengBlockHeight* height;
		liengBlockMultiple* multiple;
		liengBlockTiles* tiles;
	};
};

void
lieng_block_free (liengBlock* self);

void
lieng_block_fill (liengBlock* self,
                  liengTile   terrain);

int
lieng_block_rebuild (liengBlock*        self,
                     liengEngine*       engine,
                     const limatVector* offset);

int
lieng_block_set_tile (liengBlock* self,
                      uint8_t     x,
                      uint8_t     y,
                      uint8_t     z,
                      liengTile   terrain);

#endif

/** @} */
/** @} */
