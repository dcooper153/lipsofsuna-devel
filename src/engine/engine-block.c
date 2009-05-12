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

#include <stdlib.h>
#include <string.h>
#include "engine.h"
#include "engine-block.h"

static int
private_build_tiles (liengBlock*  self,
                     limatVector* vertices);

static void
private_optimize_tiles (liengBlock* self);

static int
private_update_physics (liengBlock*  self,
                        liengEngine* engine);

/*****************************************************************************/

/**
 * \brief Frees the contents of the block but not the block itself.
 *
 * \param self Block.
 */
void
lieng_block_free (liengBlock* self)
{
	int i;

	if (self->physics != NULL)
		liphy_object_free (self->physics);
	if (self->shape != NULL)
		liphy_shape_free (self->shape);

	switch (self->type)
	{
		case LIENG_BLOCK_TYPE_FULL:
			break;
		case LIENG_BLOCK_TYPE_HEIGHT:
			free (self->height);
			break;
		case LIENG_BLOCK_TYPE_MULTIPLE:
			for (i = 0 ; i < self->multiple->count ; i++)
				lieng_block_free (self->multiple->blocks + i);
			free (self->multiple->blocks);
			free (self->multiple);
			break;
		case LIENG_BLOCK_TYPE_TILES:
			free (self->tiles);
			break;
	}
}

/**
 * \brief Fills the block with the given terrain type.
 *
 * You need to call #lieng_block_rebuild manually if something was changed.
 *
 * \param self Block.
 * \param terrain Terrain type.
 */
void
lieng_block_fill (liengBlock* self,
                  liengTile   terrain)
{
	lieng_block_free (self);
	memset (self, 0, sizeof (liengBlock));
	self->type = LIENG_BLOCK_TYPE_FULL;
	self->full.terrain = terrain;
}

/**
 * \brief Rebuilds the physics and graphics of the block.
 *
 * \param self Block.
 * \param engine Engine.
 * \param offset Position vector of the block.
 * \return Nonzero on success.
 */
int
lieng_block_rebuild (liengBlock*        self,
                     liengEngine*       engine,
                     const limatVector* offset)
{
	limatTransform transform;

	if (!private_update_physics (self, engine))
		return 0;
	if (self->physics != NULL)
	{
		transform = limat_convert_vector_to_transform (*offset);
		liphy_object_set_transform (self->physics, &transform);
		liphy_object_set_realized (self->physics, 1);
	}

	return 1;
}

/**
 * \brief Sets the terrain type of a tile.
 *
 * This can alter the type of the block if, for example, a tile is removed from
 * a full block, in which case the block would be converted to a tiles block.
 *
 * You need to call #lieng_block_rebuild manually if something was changed.
 *
 * \param self Block.
 * \param x Offset of the tile within the block.
 * \param y Offset of the tile within the block.
 * \param z Offset of the tile within the block.
 * \param terrain Terrain type.
 * \return Nonzero if a tile was modified.
 */
int
lieng_block_set_tile (liengBlock* self,
                      uint8_t     x,
                      uint8_t     y,
                      uint8_t     z,
                      liengTile   terrain)
{
	int i;
	uint8_t tmp;
	liengBlockTiles* tiles;

	switch (self->type)
	{
		case LIENG_BLOCK_TYPE_FULL:
			tmp = self->full.terrain;
			if (tmp == terrain)
				return 0;
			tiles = calloc (1, sizeof (liengBlockTiles));
			if (tiles == NULL)
				return 0;
			i = x + 8 * y + 64 * z;
			memset (tiles->tiles, tmp, 512);
			tiles->tiles[i] = terrain;
			self->tiles = tiles;
			self->type = LIENG_BLOCK_TYPE_TILES;
			return 1;
		case LIENG_BLOCK_TYPE_HEIGHT:
			break;
		case LIENG_BLOCK_TYPE_MULTIPLE:
			for (i = 0 ; i < self->multiple->count ; i++)
			{
				if (lieng_block_set_tile (self->multiple->blocks + i, x, y, z, terrain))
					return 1;
			}
			break;
		case LIENG_BLOCK_TYPE_TILES:
			i = x + 8 * y + 64 * z;
			if (self->tiles->tiles[i] == terrain)
				return 0;
			self->tiles->tiles[i] = terrain;
			private_optimize_tiles (self);
			return 1;
	}

	return 0;
}

/*****************************************************************************/

static int
private_build_tiles (liengBlock*  self,
                     limatVector* vertices)
{
	int x;
	int y;
	int z;
	int i = 0;
	int c = 0;
	const liengTile* tiles = self->tiles->tiles;

	for (z = 0 ; z < 8 ; z++)
	{
		for (y = 0 ; y < 8 ; y++)
		{
			for (x = 0 ; x < 8 ; x++)
			{
				if (!tiles[i])
				{
					i++;
					continue;
				}
				if (!x || !self->tiles->tiles[i - 1])
				{
					vertices[c++] = limat_vector_init (x, y, z);
					vertices[c++] = limat_vector_init (x, y, z + 1);
					vertices[c++] = limat_vector_init (x, y + 1, z + 1);
					vertices[c++] = limat_vector_init (x, y + 1, z);
				}
				if (x == 7 || !self->tiles->tiles[i + 1])
				{
					vertices[c++] = limat_vector_init (x + 1, y, z);
					vertices[c++] = limat_vector_init (x + 1, y, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y + 1, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y + 1, z);
				}
				if (!y || !self->tiles->tiles[i - 8])
				{
					vertices[c++] = limat_vector_init (x, y, z);
					vertices[c++] = limat_vector_init (x, y, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y, z);
				}
				if (y == 7 || !self->tiles->tiles[i + 8])
				{
					vertices[c++] = limat_vector_init (x, y + 1, z);
					vertices[c++] = limat_vector_init (x, y + 1, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y + 1, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y + 1, z);
				}
				if (!z || !self->tiles->tiles[i - 64])
				{
					vertices[c++] = limat_vector_init (x, y, z);
					vertices[c++] = limat_vector_init (x, y + 1, z);
					vertices[c++] = limat_vector_init (x + 1, y + 1, z);
					vertices[c++] = limat_vector_init (x + 1, y, z);
				}
				if (z == 7 || !self->tiles->tiles[i + 64])
				{
					vertices[c++] = limat_vector_init (x, y, z + 1);
					vertices[c++] = limat_vector_init (x, y + 1, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y + 1, z + 1);
					vertices[c++] = limat_vector_init (x + 1, y, z + 1);
				}
				i++;
			}
		}
	}

	return c;
}

static void
private_optimize_tiles (liengBlock* self)
{
	int i;
	uint8_t tile = self->tiles->tiles[0];

	/* Check if homogeneous. */
	for (i = 1 ; i < 512 ; i++)
	{
		if (self->tiles->tiles[i] != tile)
			return;
	}

	/* Reduce to full block. */
	free (self->tiles);
	self->full.terrain = tile;
	self->type = LIENG_BLOCK_TYPE_FULL;
}

static int
private_update_physics (liengBlock*  self,
                        liengEngine* engine)
{
	int count;
	limatAabb aabb;
	limatVector vertices[4048];

	/* Free old physics data. */
	if (self->physics != NULL)
	{
		liphy_object_free (self->physics);
		self->physics = NULL;
	}
	if (self->shape != NULL)
	{
		liphy_shape_free (self->shape);
		self->shape = NULL;
	}

	/* Create collision shape. */
	if (self->type == LIENG_BLOCK_TYPE_FULL)
	{
		if (!self->full.terrain)
			return 1;
		aabb.min = limat_vector_init (0.0f, 0.0f, 0.0f);
		aabb.max = limat_vector_init (8.0f, 8.0f, 8.0f);
		self->shape = liphy_shape_new_aabb (engine->physics, &aabb);
		if (self->shape == NULL)
			return 0;
		self->physics = liphy_object_new (engine->physics, self->shape,
			LIPHY_SHAPE_MODE_BOX, LIPHY_CONTROL_MODE_STATIC);
	}
	else if (self->type == LIENG_BLOCK_TYPE_TILES)
	{
		count = private_build_tiles (self, vertices);
		self->shape = liphy_shape_new_convex (engine->physics, vertices, count);
		if (self->shape == NULL)
			return 0;
		self->physics = liphy_object_new (engine->physics, self->shape,
			LIPHY_SHAPE_MODE_CONCAVE, LIPHY_CONTROL_MODE_STATIC);
	}
	else
	{
		/* FIXME! */
		return 0;
	}
	if (self->physics == NULL)
		return 0;
	liphy_object_set_collision_group (self->physics, LIENG_PHYSICS_GROUP_TILES);
	liphy_object_set_collision_mask (self->physics, ~LIENG_PHYSICS_GROUP_TILES);

	return 1;
}

/** @} */
/** @} */
