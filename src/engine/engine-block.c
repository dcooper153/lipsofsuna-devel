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
#include "engine-voxel.h"

/**
 * \brief Frees the contents of the block but not the block itself.
 *
 * \param self Block.
 */
void
lieng_block_free (liengBlock* self)
{
	int i;
#ifndef LIENG_DISABLE_GRAPHICS
	limdlModel* model;
	lirndModel* rndmdl;
#endif

#ifndef LIENG_DISABLE_GRAPHICS
	if (self->render != NULL)
	{
		model = self->render->model->model;
		rndmdl = self->render->model;
		lirnd_object_free (self->render);
		lirnd_model_free (rndmdl);
		limdl_model_free (model);
	}
#endif
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
	if (terrain & 0xFF)
		self->full.terrain = terrain | 0xFF00;
	else
		self->full.terrain = 0;
}

/**
 * \brief Fills a box with the given terrain type.
 *
 * \param self Block.
 * \param aabb Bounding box relative to the origin of the block.
 * \param terrain Terrain type.
 * \return Nonzero if at least one voxel was modified.
 */
int
lieng_block_fill_aabb (liengBlock*      self,
                       const limatAabb* box,
                       liengTile        terrain)
{
	int x;
	int y;
	int z;
	int ret = 0;
	limatAabb child;

	terrain &= 0xFF;
	if (!terrain)
	{
		/* Erase terrain. */
		for (z = 0 ; z < LIENG_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIENG_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIENG_TILES_PER_LINE ; x++)
		{
			child.min = limat_vector_init (
				x * LIENG_TILE_WIDTH,
				y * LIENG_TILE_WIDTH,
				z * LIENG_TILE_WIDTH);
			child.max = limat_vector_init (
				(x + 1) * LIENG_TILE_WIDTH,
				(y + 1) * LIENG_TILE_WIDTH,
				(z + 1) * LIENG_TILE_WIDTH);
			if (limat_aabb_intersects_aabb (box, &child))
				ret |= lieng_block_set_voxel (self, x, y, z, 0);
		}
	}
	else
	{
		/* Paint terrain. */
		for (z = 0 ; z < LIENG_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIENG_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIENG_TILES_PER_LINE ; x++)
		{
			child.min = limat_vector_init (
				x * LIENG_TILE_WIDTH,
				y * LIENG_TILE_WIDTH,
				z * LIENG_TILE_WIDTH);
			child.max = limat_vector_init (
				(x + 1) * LIENG_TILE_WIDTH,
				(y + 1) * LIENG_TILE_WIDTH,
				(z + 1) * LIENG_TILE_WIDTH);
			if (limat_aabb_intersects_aabb (box, &child))
				ret |= lieng_block_set_voxel (self, x, y, z, 0xFF00 | terrain);
		}
	}

	return ret;
}

/**
 * \brief Fills a sphere with the given terrain type.
 *
 * \param self Block.
 * \param center Center of the sphere relative to the origin of the block.
 * \param radius Radius of the sphere.
 * \param terrain Terrain type.
 * \return Nonzero if at least one voxel was modified.
 */
int
lieng_block_fill_sphere (liengBlock*        self,
                         const limatVector* center,
                         float              radius,
                         liengTile          terrain)
{
	int i;
	int x;
	int y;
	int z;
	int ret = 0;
	liengTile tile;
	limatVector dist;
	static const limatVector corner_offsets[8] =
	{
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }
	};

	terrain &= 0xFF;
	if (!terrain)
	{
		/* Erase terrain. */
		for (z = 0 ; z < LIENG_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIENG_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIENG_TILES_PER_LINE ; x++)
		{
			tile = lieng_block_get_voxel (self, x, y, z);
			for (i = 0 ; i < 8 ; i++)
			{
				dist = limat_vector_subtract (*center, limat_vector_init (
					(x + corner_offsets[i].x) * LIENG_TILE_WIDTH,
					(y + corner_offsets[i].y) * LIENG_TILE_WIDTH,
					(z + corner_offsets[i].z) * LIENG_TILE_WIDTH));
				if (limat_vector_dot (dist, dist) <= radius * radius)
					tile &= ~(1 << (i + 8));
			}
			tile = lieng_voxel_validate (tile);
			ret |= lieng_block_set_voxel (self, x, y, z, tile);
		}
	}
	else
	{
		/* Paint terrain. */
		for (z = 0 ; z < LIENG_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIENG_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIENG_TILES_PER_LINE ; x++)
		{
			tile = lieng_block_get_voxel (self, x, y, z) & 0xFF00;
			tile |= terrain;
			for (i = 0 ; i < 8 ; i++)
			{
				dist = limat_vector_subtract (*center, limat_vector_init (
					(x + corner_offsets[i].x) * LIENG_TILE_WIDTH,
					(y + corner_offsets[i].y) * LIENG_TILE_WIDTH,
					(z + corner_offsets[i].z) * LIENG_TILE_WIDTH));
				if (limat_vector_dot (dist, dist) <= radius * radius)
					tile |= (1 << (i + 8));
			}
			tile = lieng_voxel_validate (tile);
			ret |= lieng_block_set_voxel (self, x, y, z, tile);
		}
	}
	self->rebuild |= ret;

	return ret;
}

/**
 * \brief Optimizes the block.
 *
 * \param self Block.
 */
void
lieng_block_optimize (liengBlock* self)
{
	int i;
	liengTile tile;

	if (self->type == LIENG_BLOCK_TYPE_TILES)
	{
		/* Convert homegeneous tile blocks to full blocks. */
		tile = self->tiles->tiles[0];
		for (i = 1 ; i < LIENG_TILES_PER_BLOCK ; i++)
		{
			if (self->tiles->tiles[i] != tile)
				return;
		}
		free (self->tiles);
		self->full.terrain = tile;
		self->type = LIENG_BLOCK_TYPE_FULL;
	}
}

/**
 * \brief Gets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the block.
 * \param y Offset of the voxel within the block.
 * \param z Offset of the voxel within the block.
 * \return Terrain type or zero.
 */
liengTile
lieng_block_get_voxel (liengBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z)
{
	switch (self->type)
	{
		case LIENG_BLOCK_TYPE_FULL:
			assert (self->full.terrain != 0xFF00);
			return self->full.terrain;
		case LIENG_BLOCK_TYPE_HEIGHT:
			break;
		case LIENG_BLOCK_TYPE_MULTIPLE:
			/* FIXME: Not implemented. */
			break;
		case LIENG_BLOCK_TYPE_TILES:
			assert (self->tiles->tiles[LIENG_TILE_INDEX (x, y, z)] != 0xFF00);
			return self->tiles->tiles[LIENG_TILE_INDEX (x, y, z)];
	}

	return 0;
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * This can alter the type of the block if, for example, a tile is removed from
 * a full block, in which case the block would be converted to a tiles block.
 *
 * You need to call #lieng_block_rebuild manually if something was changed.
 *
 * \param self Block.
 * \param x Offset of the voxel within the block.
 * \param y Offset of the voxel within the block.
 * \param z Offset of the voxel within the block.
 * \param terrain Terrain type.
 * \return Nonzero if a voxel was modified.
 */
int
lieng_block_set_voxel (liengBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z,
                       liengTile   terrain)
{
	int i;
	liengTile tmp;
	liengBlockTiles* tiles;

	terrain = lieng_voxel_validate (terrain);
	switch (self->type)
	{
		case LIENG_BLOCK_TYPE_FULL:
			tmp = lieng_voxel_init (0xFF, self->full.terrain);
			if (tmp == terrain)
				return 0;
			tiles = calloc (1, sizeof (liengBlockTiles));
			if (tiles == NULL)
				return 0;
			for (i = 0 ; i < LIENG_TILES_PER_BLOCK ; i++)
				tiles->tiles[i] = tmp;
			i = LIENG_TILE_INDEX (x, y, z);
			tiles->tiles[i] = terrain;
			self->tiles = tiles;
			self->type = LIENG_BLOCK_TYPE_TILES;
			self->rebuild = 1;
			return 1;
		case LIENG_BLOCK_TYPE_HEIGHT:
			break;
		case LIENG_BLOCK_TYPE_MULTIPLE:
			for (i = 0 ; i < self->multiple->count ; i++)
			{
				if (lieng_block_set_voxel (self->multiple->blocks + i, x, y, z, terrain))
				{
					self->rebuild = 1;
					return 1;
				}
			}
			break;
		case LIENG_BLOCK_TYPE_TILES:
			i = LIENG_TILE_INDEX (x, y, z);
			if (self->tiles->tiles[i] == terrain)
				return 0;
			self->tiles->tiles[i] = terrain;
			self->rebuild = 1;
			return 1;
	}

	return 0;
}

/** @} */
/** @} */
