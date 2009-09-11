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
#ifndef LIVOX_DISABLE_GRAPHICS
	if (self->render != NULL)
		manager->renderapi->lirnd_object_free (self->render);
#endif
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	if (self->shape != NULL)
		liphy_shape_free (self->shape);
}

/**
 * \brief Erases terrain inside the box.
 *
 * \param self Block.
 * \param aabb Bounding box relative to the origin of the block.
 * \return Nonzero if at least one voxel was modified.
 */
int
livox_block_erase_aabb (livoxBlock*      self,
                        const limatAabb* box)
{
	return livox_block_fill_aabb (self, box, 0);
}

/**
 * \brief Erases terrain inside the sphere.
 *
 * \param self Block.
 * \param center Center of the sphere relative to the origin of the block.
 * \param radius Radius of the sphere.
 * \return Nonzero if at least one voxel was modified.
 */
int
livox_block_erase_sphere (livoxBlock*        self,
                          const limatVector* center,
                          float              radius)
{
	return livox_block_fill_sphere (self, center, radius, 0);
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
                  int           terrain)
{
	int i;

	for (i = 0 ; i < LIVOX_TILES_PER_BLOCK ; i++)
		livox_voxel_init (self->tiles + i, terrain);
	self->dirty = 0xFF;
	self->stamp++;
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
                       int              terrain)
{
	int x;
	int y;
	int z;
	int ret = 0;
	limatAabb child;
	livoxVoxel tmp;

	livox_voxel_init (&tmp, terrain);

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
			ret |= livox_block_set_voxel (self, x, y, z, &tmp);
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
                         int                terrain)
{
	int x;
	int y;
	int z;
	int ret = 0;
	livoxVoxel tmp;
	limatVector dist;

	livox_voxel_init (&tmp, terrain);

	/* Paint terrain. */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		dist = limat_vector_subtract (*center, limat_vector_init (
			x * LIVOX_TILE_WIDTH,
			y * LIVOX_TILE_WIDTH,
			z * LIVOX_TILE_WIDTH));
		if (limat_vector_dot (dist, dist) <= radius * radius)
			ret |= livox_block_set_voxel (self, x, y, z, &tmp);
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
	uint8_t type;
	uint8_t terrain;
	int8_t displacex;
	int8_t displacey;
	int8_t displacez;
	livoxVoxel tmp;

	if (!liarc_reader_get_uint8 (reader, &type))
		return 0;
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		if (!liarc_reader_get_uint8 (reader, &terrain) ||
		    !liarc_reader_get_int8 (reader, &displacex) ||
		    !liarc_reader_get_int8 (reader, &displacey) ||
		    !liarc_reader_get_int8 (reader, &displacez))
			return 0;
		livox_voxel_init (&tmp, terrain);
		tmp.displacex = displacex;
		tmp.displacey = displacey;
		tmp.displacez = displacez;
		livox_block_set_voxel (self, x, y, z, &tmp);
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

	if (!liarc_writer_append_uint8 (writer, 0))
		return 0;
	for (i = 0 ; i < LIVOX_TILES_PER_BLOCK ; i++)
	{
		if (!liarc_writer_append_uint8 (writer, self->tiles[i].terrain) ||
		    !liarc_writer_append_int8 (writer, self->tiles[i].displacex) ||
		    !liarc_writer_append_int8 (writer, self->tiles[i].displacey) ||
		    !liarc_writer_append_int8 (writer, self->tiles[i].displacez))
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
livox_block_get_empty (const livoxBlock* self)
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
 * This can alter the type of the block if, for example, a tile is removed from
 * a full block, in which case the block would be converted to a tiles block.
 *
 * You need to call #livox_block_rebuild manually if something was changed.
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
	if (self->tiles[i].terrain == voxel->terrain &&
	    self->tiles[i].displacex == voxel->displacex &&
	    self->tiles[i].displacey == voxel->displacey &&
	    self->tiles[i].displacez == voxel->displacez)
		return 0;
	self->tiles[i].terrain = voxel->terrain;
    self->tiles[i].displacex = voxel->displacex;
    self->tiles[i].displacey = voxel->displacey;
    self->tiles[i].displacez = voxel->displacez;

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
