/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtRegion Region
 * @{
 */

#include "ext-module.h"
#include "ext-sector.h"

/**
 * \brief Creates a new sector.
 *
 * \param sector Sector manager sector.
 * \return New sector or NULL.
 */
LIExtSector* liext_sector_new (
	LIAlgSector* sector)
{
	LIExtModule* module;
	LIExtSector* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtSector));
	if (self == NULL)
		return NULL;
	module = lialg_sectors_get_userdata (sector->manager, "region");
	self->server = limai_program_find_component (module->program, "server");
	self->sector = sector;

	/* Load data. */
	if (self->server != NULL && self->server->sql != NULL)
	{
		if (!liext_sector_read (self, self->server->sql))
		{
			lisys_error_report ();
			liext_sector_free (self);
			return NULL;
		}
	}

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void liext_sector_free (
	LIExtSector* self)
{
	LIExtRegion* region;
	LIExtRegion* region_next;

	/* Free regions. */
	for (region = self->regions ; region != NULL ; region = region_next)
	{
		region_next = region->next;
		lisys_free (region);
	}

	lisys_free (self);
}

/**
 * \brief Reads the sector from the database.
 *
 * \param self Sector.
 * \param sql Database.
 * \return Nonzero on success.
 */
int liext_sector_read (
	LIExtSector* self,
	LIArcSql*    sql)
{
	int id;
	int ret;
	int size;
	uint32_t idx;
	uint32_t flags;
	uint32_t type;
	uint32_t value;
	const char* query;
	const void* bytes;
	LIArcReader* reader;
	LIExtRegion* region;
	LIMatVector position;
	LIMatVector dimensions;
	sqlite3_stmt* statement;

	/* Prepare statement. */
	id = self->sector->index;
	query = "SELECT data FROM regions WHERE sector=?;";
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
	while (1)
	{
		/* Read row. */
		ret = sqlite3_step (statement);
		if (ret == SQLITE_DONE)
		{
			sqlite3_finalize (statement);
			break;
		}
		if (ret != SQLITE_ROW)
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

		/* Read data. */
		if (!liarc_reader_get_uint32 (reader, &idx) ||
		    !liarc_reader_get_uint32 (reader, &type) ||
		    !liarc_reader_get_uint32 (reader, &flags) ||
		    !liarc_reader_get_uint32 (reader, &value) ||
		    !liarc_reader_get_float (reader, &position.x) ||
		    !liarc_reader_get_float (reader, &position.y) ||
		    !liarc_reader_get_float (reader, &position.z) ||
		    !liarc_reader_get_float (reader, &dimensions.x) ||
		    !liarc_reader_get_float (reader, &dimensions.y) ||
		    !liarc_reader_get_float (reader, &dimensions.z))
		{
			sqlite3_finalize (statement);
			liarc_reader_free (reader);
			return 0;
		}
		liarc_reader_free (reader);

		/* Create region. */
		region = lisys_calloc (1, sizeof (LIExtRegion));
		if (region == NULL)
		{
			sqlite3_finalize (statement);
			return 0;
		}
		region->id = idx;
		region->type = type;
		region->flags = flags;
		region->value = value;
		region->position = position;
		region->size = dimensions;

		/* Store region. */
		if (self->regions != NULL)
			self->regions->prev = region;
		region->next = self->regions;
		self->regions = region;
	}

	return 1;
}

/** @} */
/** @} */
