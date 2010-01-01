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
 * \addtogroup livoxSector Sector
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "voxel-manager.h"
#include "voxel-sector.h"
#include "voxel-private.h"

#define LIVOX_ERASE_SHIFT (0.25f * LIVOX_TILE_WIDTH)
#define LIVOX_TILES_PER_SECLINE (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE)

static int
private_build_block (livoxSector* self,
                     int          x,
                     int          y,
                     int          z);

/*****************************************************************************/

/**
 * \brief Creates a new sector.
 *
 * \param sector Sector manager sector.
 * \return New sector or NULL.
 */
livoxSector*
livox_sector_new (lialgSector* sector)
{
	livoxSector* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (livoxSector));
	if (self == NULL)
		return NULL;
	self->manager = lialg_sectors_get_userdata (sector->manager, "voxel");
	self->sector = sector;

	/* Load data. */
	if (self->manager->sql != NULL)
		livox_sector_read (self, self->manager->sql);

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
livox_sector_free (livoxSector* self)
{
	int i;

	/* Free blocks. */
	for (i = 0 ; i < LIVOX_BLOCKS_PER_SECTOR ; i++)
		livox_block_free (self->blocks + i, self->manager);

	lisys_free (self);
}

int
livox_sector_build_block (livoxSector* self,
                          int          x,
                          int          y,
                          int          z)
{
	return private_build_block (self, x, y, z);
}

/**
 * \brief Fills the sector with the given terrain type.
 *
 * \param self Sector.
 * \param terrain Terrain type.
 */
void
livox_sector_fill (livoxSector* self,
                   livoxVoxel*  terrain)
{
	int x;
	int y;
	int z;
	int i = 0;

	for (z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
	{
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		{
			for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++)
			{
				livox_block_fill (self->blocks + i, self->manager, terrain);
				i++;
			}
		}
	}
	self->dirty = 1;
}

/**
 * \brief Reads terrain data from the database.
 *
 * \param self Sector.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
livox_sector_read (livoxSector* self,
                   liarcSql*    sql)
{
	int x;
	int y;
	int z;
	int id;
	int size;
	uint16_t terrain;
	uint8_t damage;
	uint8_t rotation;
	const char* query;
	const void* bytes;
	liarcReader* reader;
	livoxVoxel tmp;
	sqlite3_stmt* statement;

	/* Prepare statement. */
	id = self->sector->index;
	query = "SELECT data FROM voxel_sectors WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Execute statement. */
	if (sqlite3_step (statement) != SQLITE_ROW)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Read columns. */
	bytes = sqlite3_column_blob (statement, 0);
	size = sqlite3_column_bytes (statement, 0);
	reader = liarc_reader_new (bytes, size);
	if (reader == NULL)
	{
		sqlite3_finalize (statement);
		return 0;
	}

	/* Deserialize terrain. */
	for (z = 0 ; z < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE ; x++)
	{
		if (!liarc_reader_get_uint16 (reader, &terrain) ||
		    !liarc_reader_get_uint8 (reader, &damage) ||
		    !liarc_reader_get_uint8 (reader, &rotation))
		{
			sqlite3_finalize (statement);
			liarc_reader_free (reader);
			return 0;
		}
		if (terrain || damage)
		{
			livox_voxel_init (&tmp, terrain);
			tmp.damage = damage;
			tmp.rotation = rotation;
			livox_sector_set_voxel (self, x, y, z, tmp);
		}
	}
	sqlite3_finalize (statement);

	/* Force rebuild. */
	liarc_reader_free (reader);
	self->dirty = 1;

	return 1;
}

/**
 * \brief Called once per tick to update the status of the sector.
 *
 * \param self Sector.
 * \param secs Number of seconds since the last update.
 */
void
livox_sector_update (livoxSector* self,
                     float        secs)
{
}

/**
 * \brief Writes the terrain data of the sector to the database.
 *
 * \param self Sector.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
livox_sector_write (livoxSector* self,
                    liarcSql*    sql)
{
	int x;
	int y;
	int z;
	int id;
	int col = 1;
	const char* query;
	sqlite3_stmt* statement;
	liarcWriter* writer;
	livoxVoxel* tmp;

	id = self->sector->index;

	/* Don't save empty sectors. */
	if (livox_sector_get_empty (self))
	{
		query = "DELETE FROM voxel_sectors WHERE id=?;";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			return 0;
		}
		if (sqlite3_bind_int (statement, col++, id) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		if (sqlite3_step (statement) != SQLITE_DONE)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		sqlite3_finalize (statement);
		return 1;
	}

	/* Serialize terrain. */
	writer = liarc_writer_new ();
	if (writer == NULL)
		return 0;
	for (z = 0 ; z < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE ; x++)
	{
		tmp = livox_sector_get_voxel (self, x, y, z);
		if (!liarc_writer_append_uint16 (writer, tmp->type) ||
		    !liarc_writer_append_uint8 (writer, tmp->damage) ||
		    !liarc_writer_append_uint8 (writer, tmp->rotation))
			return 0;
	}

	/* Prepare statement. */
	query = "INSERT OR REPLACE INTO voxel_sectors (id,data) VALUES (?,?);";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		liarc_writer_free (writer);
		return 0;
	}

	/* Bind values. */
	if (sqlite3_bind_int (statement, col++, id) != SQLITE_OK ||
	    sqlite3_bind_blob (statement, col++, writer->memory.buffer, writer->memory.length, SQLITE_TRANSIENT) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		liarc_writer_free (writer);
		return 0;
	}
	liarc_writer_free (writer);

	/* Write values. */
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

/**
 * \brief Gets a voxel block.
 *
 * \param self Sector.
 * \param index Block index.
 * \return Block.
 */
livoxBlock*
livox_sector_get_block (livoxSector* self,
                        int          index)
{
	assert (index >= 0);
	assert (index < LIVOX_BLOCKS_PER_SECTOR);

	return self->blocks + index;
}

/**
 * \brief Gets the bounding box of the sector.
 *
 * \param self Sector.
 * \param result Return location for the bounding box.
 */
void
livox_sector_get_bounds (const livoxSector* self,
                         limatAabb*         result)
{
	limatVector min;
	limatVector max;

	min = self->sector->position;
	max = limat_vector_init (LIVOX_SECTOR_WIDTH, LIVOX_SECTOR_WIDTH, LIVOX_SECTOR_WIDTH);
	max = limat_vector_add (min, max);
	limat_aabb_init_from_points (result, &min, &max);
}

/**
 * \brief Returns nonzero if the sector is dirty.
 *
 * \param self Sector.
 * \return Nonzero if dirty.
 */
int
livox_sector_get_dirty (const livoxSector* self)
{
	return self->dirty;
}

/**
 * \brief Sets or clears the dirty flag of the sector.
 *
 * \param self Sector.
 * \param value Zero to clear, nonzero to set.
 */
void
livox_sector_set_dirty (livoxSector* self,
                        int          value)
{
	self->dirty = value;
}

/**
 * \brief Checks if the sector contains no terrain.
 *
 * \param self Sector.
 * \return Nonzero if the sector is empty.
 */
int
livox_sector_get_empty (const livoxSector* self)
{
	int i;

	for (i = 0 ; i < LIVOX_BLOCKS_PER_SECTOR ; i++)
	{
		if (!livox_block_get_empty (self->blocks + i))
			return 0;
	}

	return 1;
}

/**
 * \brief Gets the offset of the sector in the world in voxels.
 *
 * \param self Sector.
 * \param x Return location for the offset.
 * \param y Return location for the offset.
 * \param z Return location for the offset.
 */
void
livox_sector_get_offset (const livoxSector* self,
                         int*               x,
                         int*               y,
                         int*               z)
{
	*x = self->sector->x;
	*y = self->sector->y;
	*z = self->sector->z;
}

/**
 * \brief Gets the origin of the sector.
 *
 * \param self Sector.
 * \param result Return location for the position vector.
 */
void
livox_sector_get_origin (const livoxSector* self,
                         limatVector*       result)
{
	*result = self->sector->position;
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \return Terrain type or zero.
 */
livoxVoxel*
livox_sector_get_voxel (livoxSector* self,
                        int          x,
                        int          y,
                        int          z)
{
	livoxBlock* block;
	int bx = x / LIVOX_TILES_PER_LINE;
	int by = y / LIVOX_TILES_PER_LINE;
	int bz = z / LIVOX_TILES_PER_LINE;
	int tx = x % LIVOX_TILES_PER_LINE;
	int ty = y % LIVOX_TILES_PER_LINE;
	int tz = z % LIVOX_TILES_PER_LINE;

	assert (x >= 0 && y >= 0 && z >= 0);
	assert (x < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE);
	assert (y < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE);
	assert (z < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE);
	block = self->blocks + LIVOX_BLOCK_INDEX (bx, by, bz);

	return livox_block_get_voxel (block, tx, ty, tz);
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \param terrain Terrain type.
 * \return Nonzero if a voxel was modified.
 */
int
livox_sector_set_voxel (livoxSector* self,
                        int          x,
                        int          y,
                        int          z,
                        livoxVoxel   terrain)
{
	int ret;
	int bx = x / LIVOX_TILES_PER_LINE;
	int by = y / LIVOX_TILES_PER_LINE;
	int bz = z / LIVOX_TILES_PER_LINE;
	int tx = x % LIVOX_TILES_PER_LINE;
	int ty = y % LIVOX_TILES_PER_LINE;
	int tz = z % LIVOX_TILES_PER_LINE;
	livoxBlock* block;

	assert (x >= 0 && y >= 0 && z >= 0);
	assert (x < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE);
	assert (y < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE);
	assert (z < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE);
	block = self->blocks + LIVOX_BLOCK_INDEX (bx, by, bz);
	ret = livox_block_set_voxel (block, tx, ty, tz, &terrain);
	if (ret)
	{
		block->stamp++;
		self->dirty = 1;
	}

	return ret;
}

/*****************************************************************************/

static int
private_build_block (livoxSector* self,
                     int          x,
                     int          y,
                     int          z)
{
	livoxUpdateEvent event;

	/* Invoke callbacks. */
	event.sector[0] = self->sector->x;
	event.sector[1] = self->sector->y;
	event.sector[2] = self->sector->z;
	event.block[0] = x;
	event.block[1] = y;
	event.block[2] = z;
	lical_callbacks_call (self->manager->callbacks, self->manager, "block-load", lical_marshal_DATA_PTR, &event);

	return 1;
}

/** @} */
/** @} */
