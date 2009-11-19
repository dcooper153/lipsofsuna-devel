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

#include <system/lips-system.h>
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
                  livoxVoxel*   terrain)
{
	int i;

	for (i = 0 ; i < LIVOX_TILES_PER_BLOCK ; i++)
		self->tiles[i] = *terrain;
	self->dirty = 0xFF;
	self->stamp++;
}

/**
 * \brief Optimizes the block.
 *
 * \param self Block.
 */
void
livox_block_optimize (livoxBlock* self)
{
	/* TODO? */
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
                  liarcReader*  reader)
{
	int x;
	int y;
	int z;
	uint8_t damage;
	uint8_t rotation;
	uint16_t terrain;
	livoxVoxel tmp;

	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		if (!liarc_reader_get_uint16 (reader, &terrain) ||
		    !liarc_reader_get_uint8 (reader, &damage) ||
		    !liarc_reader_get_uint8 (reader, &rotation))
			return 0;
		livox_voxel_init (&tmp, terrain);
		tmp.damage = damage;
		tmp.rotation = rotation;
		livox_block_set_voxel (self, x, y, z, &tmp);
	}

	return 1;
}

/**
 * \brief Writes block data to a stream.
 *
 * \param self Block.
 * \param writer Writer.
 * \return Nonzero on success.
 */
int
livox_block_write (livoxBlock*  self,
                   liarcWriter* writer)
{
	int i;

	for (i = 0 ; i < LIVOX_TILES_PER_BLOCK ; i++)
	{
		if (!liarc_writer_append_uint16 (writer, self->tiles[i].type) ||
		    !liarc_writer_append_uint8 (writer, self->tiles[i].damage) ||
		    !liarc_writer_append_uint8 (writer, self->tiles[i].rotation))
			return 0;
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
 * \brief Sets or clears the dirty flag of the block.
 *
 * \param self Block.
 * \param value Zero to clear, nonzero to set.
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
livox_block_get_empty (const livoxBlock* self)
{
#warning livox_block_get_empty not implemented.
	return 0;//self->objects.count == 0;
}

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
livoxVoxel*
livox_block_get_voxel (livoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z)
{
	return self->tiles + LIVOX_TILE_INDEX (x, y, z);
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * If the voxel data is changed, the dirty flag of the block is updated.
 *
 * \param self Block.
 * \param x Offset of the voxel within the block.
 * \param y Offset of the voxel within the block.
 * \param z Offset of the voxel within the block.
 * \param voxel Voxel data.
 * \return Nonzero if a voxel was modified.
 */
int
livox_block_set_voxel (livoxBlock* self,
                       uint8_t     x,
                       uint8_t     y,
                       uint8_t     z,
                       livoxVoxel* voxel)
{
	int i;

	/* Modify terrain. */
	i = LIVOX_TILE_INDEX (x, y, z);
	if (self->tiles[i].type == voxel->type &&
	    self->tiles[i].damage == voxel->damage &&
	    self->tiles[i].rotation == voxel->rotation)
		return 0;
	self->tiles[i] = *voxel;

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
