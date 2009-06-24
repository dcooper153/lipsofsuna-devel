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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvVoxel Voxel
 * @{
 */

#include "ext-listener.h"
#include "ext-sector.h"
#include "ext-module.h"

/**
 * \brief Reads terrain data from the database.
 *
 * \param self Sector.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
liext_sector_read (liengSector* self,
                   liarcSql*    sql)
{
	int i;
	int id;
	int size;
	const char* query;
	const void* bytes;
	liReader* reader;
	liengBlock* block;
	sqlite3_stmt* statement;

	/* Prepare statement. */
	id = LIENG_SECTOR_INDEX (self->x, self->y, self->z);
	query = "SELECT (data) FROM voxel_sectors WHERE id=?;";
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
	reader = li_reader_new (bytes, size);
	if (reader == NULL)
	{
		sqlite3_finalize (statement);
		return 0;
	}

	/* Deserialize terrain. */
	for (i = 0 ; i < LIENG_BLOCKS_PER_SECTOR ; i++)
	{
		block = self->blocks + i;
		if (!lieng_block_read (block, self->engine, reader))
		{
			sqlite3_finalize (statement);
			return 0;
		}
	}
	sqlite3_finalize (statement);

	/* Force rebuild. */
	self->dirty = 1;

	return 1;
}

void
liext_sector_update (liengSector* self,
                     float        secs)
{
	lieng_sector_update (self, secs);
}

/**
 * \brief Writes the terrain data of the sector to the database.
 *
 * \param self Sector.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
liext_sector_write (liengSector* self,
                    liarcSql*    sql)
{
	int i;
	int id;
	int col = 1;
	const char* query;
	sqlite3_stmt* statement;
	liarcWriter* writer;

	id = LIENG_SECTOR_INDEX (self->x, self->y, self->z);

	/* Don't save empty sectors. */
	if (liext_sector_get_empty (self))
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
	for (i = 0 ; i < LIENG_BLOCKS_PER_SECTOR ; i++)
		lieng_block_write (self->blocks + i, writer);

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
 * \brief Checks if the sector contains no terrain.
 *
 * \param self Sector.
 * \return Nonzero if the sector is empty.
 */
int
liext_sector_get_empty (const liengSector* self)
{
	int i;

	for (i = 0 ; i < LIENG_BLOCKS_PER_SECTOR ; i++)
	{
		if (self->blocks[i].type != LIENG_BLOCK_TYPE_FULL ||
		    self->blocks[i].full.terrain != 0)
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
