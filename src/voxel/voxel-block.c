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

#include <stdlib.h>
#include <string.h>
#include "voxel.h"
#include "voxel-block.h"
#include "voxel-manager.h"
#include "voxel-private.h"

/**
 * \brief Frees the contents of the block but not the block itself.
 *
 * \param self Block.
 * \param manager Voxel manager.
 */
void
livox_block_free (livoxBlock*   self,
                  livoxManager* manager)
{
#ifndef LIVOX_DISABLE_GRAPHICS
	if (self->render != NULL)
		manager->renderapi->lirnd_object_free (self->render);
#endif
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	if (self->shape != NULL)
		liphy_shape_free (self->shape);

	switch (self->type)
	{
		case LIVOX_BLOCK_TYPE_FULL:
			break;
		case LIVOX_BLOCK_TYPE_TILES:
			free (self->tiles);
			break;
	}
}

/**
 * \brief Fills the block with the given terrain type.
 *
 * \param self Block.
 * \param manager Voxel manager.
 * \param terrain Terrain type.
 */
void
livox_block_fill (livoxBlock*   self,
                  livoxManager* manager,
                  livoxVoxel    terrain)
{
	terrain = livox_voxel_init (0xFF, terrain);
	if (self->type == LIVOX_BLOCK_TYPE_FULL)
	{
		/* Set new terrain. */
		if (self->full.terrain != terrain)
		{
			self->full.terrain = terrain;
			self->dirty = 0xFF;
			self->stamp++;
		}
	}
	else
	{
		/* Free old data. */
		livox_block_free (self, manager);
		memset (self, 0, sizeof (livoxBlock));

		/* Set new terrain. */
		self->type = LIVOX_BLOCK_TYPE_FULL;
		self->dirty = 0xFF;
		self->full.terrain = terrain;
		self->stamp++;
	}
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
livox_block_fill_aabb (livoxBlock*      self,
                       const limatAabb* box,
                       livoxVoxel       terrain)
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
		for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
		{
			child.min = limat_vector_init (
				x * LIVOX_TILE_WIDTH,
				y * LIVOX_TILE_WIDTH,
				z * LIVOX_TILE_WIDTH);
			child.max = limat_vector_init (
				(x + 1) * LIVOX_TILE_WIDTH,
				(y + 1) * LIVOX_TILE_WIDTH,
				(z + 1) * LIVOX_TILE_WIDTH);
			if (limat_aabb_intersects_aabb (box, &child))
				ret |= livox_block_set_voxel (self, x, y, z, 0);
		}
	}
	else
	{
		/* Paint terrain. */
		for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
		{
			child.min = limat_vector_init (
				x * LIVOX_TILE_WIDTH,
				y * LIVOX_TILE_WIDTH,
				z * LIVOX_TILE_WIDTH);
			child.max = limat_vector_init (
				(x + 1) * LIVOX_TILE_WIDTH,
				(y + 1) * LIVOX_TILE_WIDTH,
				(z + 1) * LIVOX_TILE_WIDTH);
			if (limat_aabb_intersects_aabb (box, &child))
				ret |= livox_block_set_voxel (self, x, y, z, 0xFF00 | terrain);
		}
	}
	if (ret)
		self->stamp++;

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
livox_block_fill_sphere (livoxBlock*        self,
                         const limatVector* center,
                         float              radius,
                         livoxVoxel         terrain)
{
	int i;
	int x;
	int y;
	int z;
	int found;
	int ret = 0;
	livoxVoxel tile;
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
		for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
		{
			tile = livox_block_get_voxel (self, x, y, z);
			for (i = 0 ; i < 8 ; i++)
			{
				dist = limat_vector_subtract (*center, limat_vector_init (
					(x + corner_offsets[i].x) * LIVOX_TILE_WIDTH,
					(y + corner_offsets[i].y) * LIVOX_TILE_WIDTH,
					(z + corner_offsets[i].z) * LIVOX_TILE_WIDTH));
				if (limat_vector_dot (dist, dist) <= radius * radius)
					tile &= ~(1 << (i + 8));
			}
			tile = livox_voxel_validate (tile);
			ret |= livox_block_set_voxel (self, x, y, z, tile);
		}
	}
	else
	{
		/* Paint terrain. */
		for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
		for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
		for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
		{
			found = 0;
			tile = livox_block_get_voxel (self, x, y, z) & 0xFF00;
			tile |= terrain;
			for (i = 0 ; i < 8 ; i++)
			{
				dist = limat_vector_subtract (*center, limat_vector_init (
					(x + corner_offsets[i].x) * LIVOX_TILE_WIDTH,
					(y + corner_offsets[i].y) * LIVOX_TILE_WIDTH,
					(z + corner_offsets[i].z) * LIVOX_TILE_WIDTH));
				if (limat_vector_dot (dist, dist) <= radius * radius)
				{
					tile |= (1 << (i + 8));
					found = 1;
				}
			}
			if (found)
			{
				tile = livox_voxel_validate (tile);
				ret |= livox_block_set_voxel (self, x, y, z, tile);
			}
		}
	}
	if (ret)
		self->stamp++;

	return ret;
}

/**
 * \brief Optimizes the block.
 *
 * \param self Block.
 */
void
livox_block_optimize (livoxBlock* self)
{
	int i;
	livoxVoxel tile;

	if (self->type == LIVOX_BLOCK_TYPE_TILES)
	{
		/* Convert homegeneous tile blocks to full blocks. */
		tile = self->tiles->tiles[0];
		for (i = 1 ; i < LIVOX_TILES_PER_BLOCK ; i++)
		{
			if (self->tiles->tiles[i] != tile)
				return;
		}
		free (self->tiles);
		self->full.terrain = tile;
		self->type = LIVOX_BLOCK_TYPE_FULL;
	}
}

/**
 * \brief Reads block data from a stream.
 *
 * \param self Block.
 * \param manager Voxel manager.
 * \param reader Reader.
 * \return Nonzero on success.
 */
int
livox_block_read (livoxBlock*   self,
                  livoxManager* manager,
                  liReader*     reader)
{
	int x;
	int y;
	int z;
	uint8_t type;
	uint16_t terrain;

	if (!li_reader_get_uint8 (reader, &type))
		return 0;
	switch (type)
	{
		case LIVOX_BLOCK_TYPE_FULL:
			if (!li_reader_get_uint16 (reader, &terrain))
				return 0;
			livox_block_fill (self, manager, terrain);
			break;
		case LIVOX_BLOCK_TYPE_TILES:
			for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
			for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
			for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
			{
				if (!li_reader_get_uint16 (reader, &terrain))
					return 0;
				livox_block_set_voxel (self, x, y, z, terrain);
			}
			break;
	}

	return 1;
}

/**
 * \brief Writes block data to a stream.
 *
 * \param self Block.
 * \param reader Reader.
 * \return Nonzero on success.
 */
int
livox_block_write (livoxBlock*  self,
                   liarcWriter* writer)
{
	int i;

	if (!liarc_writer_append_uint8 (writer, self->type))
		return 0;
	switch (self->type)
	{
		case LIVOX_BLOCK_TYPE_FULL:
			if (!liarc_writer_append_uint16 (writer, self->full.terrain))
				return 0;
			break;
		case LIVOX_BLOCK_TYPE_TILES:
			for (i = 0 ; i < LIVOX_TILES_PER_BLOCK ; i++)
			{
				if (!liarc_writer_append_uint16 (writer, self->tiles->tiles[i]))
					return 0;
			}
			break;
	}

	return 1;
}

/**
 * \brief Returns nonzero if the block is dirty.
 *
 * \param self Block.
 * \return Nonzero if dirty.
 */
int
livox_block_get_dirty (const livoxBlock* self)
{
	return self->dirty;
}

/**
 * \brief Returns nonzero if the block is dirty.
 *
 * \param self Block.
 * \return Nonzero if dirty.
 */
void
livox_block_set_dirty (livoxBlock* self,
                       int         value)
{
	self->dirty = value;
}

/**
 * \brief Checks if the block is empty.
 *
 * \param self Block.
 * \return Boolean.
 */
int
livox_block_get_empty (livoxBlock* self)
{
	return self->physics != NULL;
}

#ifndef LIVOX_DISABLE_GRAPHICS
/**
 * \brief Gets the render object of the block.
 *
 * \param self Block.
 * \return Render object or NULL.
 */
lirndObject*
livox_block_get_render (livoxBlock* self)
{
	return self->render;
}
#endif

/**
 * \brief Gets the modification stamp of the block.
 *
 * \param self Block.
 * \return Modification stamp.
 */
int
livox_block_get_stamp (const livoxBlock* self)
{
	return self->stamp;
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
livoxVoxel
livox_block_get_voxel (livoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z)
{
	switch (self->type)
	{
		case LIVOX_BLOCK_TYPE_FULL:
			assert (self->full.terrain != 0xFF00);
			return self->full.terrain;
		case LIVOX_BLOCK_TYPE_TILES:
			assert (self->tiles->tiles[LIVOX_TILE_INDEX (x, y, z)] != 0xFF00);
			return self->tiles->tiles[LIVOX_TILE_INDEX (x, y, z)];
	}

	return 0;
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * This can alter the type of the block if, for example, a tile is removed from
 * a full block, in which case the block would be converted to a tiles block.
 *
 * You need to call #livox_block_rebuild manually if something was changed.
 *
 * \param self Block.
 * \param x Offset of the voxel within the block.
 * \param y Offset of the voxel within the block.
 * \param z Offset of the voxel within the block.
 * \param terrain Terrain type.
 * \return Nonzero if a voxel was modified.
 */
int
livox_block_set_voxel (livoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z,
                       livoxVoxel  terrain)
{
	int i;
	livoxVoxel tmp;
	livoxBlockTiles* tiles;

	/* Modify terrain. */
	terrain = livox_voxel_validate (terrain);
	switch (self->type)
	{
		case LIVOX_BLOCK_TYPE_FULL:
			tmp = livox_voxel_init (0xFF, self->full.terrain);
			if (tmp == terrain)
				return 0;
			tiles = calloc (1, sizeof (livoxBlockTiles));
			if (tiles == NULL)
				return 0;
			for (i = 0 ; i < LIVOX_TILES_PER_BLOCK ; i++)
				tiles->tiles[i] = tmp;
			i = LIVOX_TILE_INDEX (x, y, z);
			tiles->tiles[i] = terrain;
			self->tiles = tiles;
			self->type = LIVOX_BLOCK_TYPE_TILES;
			break;
		case LIVOX_BLOCK_TYPE_TILES:
			i = LIVOX_TILE_INDEX (x, y, z);
			if (self->tiles->tiles[i] == terrain)
				return 0;
			self->tiles->tiles[i] = terrain;
			break;
		default:
			return 0;
	}

	/* Mark faces dirty. */
	if (x == 0)
		self->dirty |= 0x01;
	if (x == LIVOX_TILES_PER_LINE - 1)
		self->dirty |= 0x02;
	if (y == 0)
		self->dirty |= 0x04;
	if (y == LIVOX_TILES_PER_LINE - 1)
		self->dirty |= 0x08;
	if (z == 0)
		self->dirty |= 0x10;
	if (z == LIVOX_TILES_PER_LINE - 1)
		self->dirty |= 0x20;
	self->dirty |= 0x80;

	return 1;
}

/** @} */
/** @} */
