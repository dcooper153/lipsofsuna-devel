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
 * \addtogroup livoxMaterial Material
 * @{
 */

#include <string/lips-string.h>
#include "voxel-material.h"

/**
 * \brief Creates a new material.
 *
 * \return New material or NULL.
 */
livoxMaterial*
livox_material_new ()
{
	livoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
		return NULL;

	/* Allocate name. */
	self->name = listr_dup ("");
	if (self->name == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate model. */
	self->model = listr_dup ("");
	if (self->model == NULL)
	{
		lisys_free (self->name);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a copy of a material.
 *
 * \param src Material to copy.
 * \return Soft copy of the material or NULL.
 */
livoxMaterial*
livox_material_new_copy (const livoxMaterial* src)
{
	livoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
		return NULL;

	/* Copy values. */
	self->id = src->id;
	self->flags = src->flags;
	self->friction = src->friction;

	/* Copy name. */
	self->name = listr_dup (src->name);
	if (self->name == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Copy model. */
	self->model = listr_dup (src->model);
	if (self->model == NULL)
	{
		lisys_free (self->name);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Deserializes a material from an SQL statement.
 *
 * \param sql SQL database.
 * \param stmt SQL statement;
 * \return New material or NULL.
 */
livoxMaterial*
livox_material_new_from_sql (liarcSql*     sql,
                             sqlite3_stmt* stmt)
{
	int col;
	int size;
	livoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
		return NULL;

	/* Read numeric values. */
	col = 0;
	self->id = sqlite3_column_int (stmt, col++);
	self->flags = sqlite3_column_int (stmt, col++);
	self->friction = sqlite3_column_double (stmt, col++);

	/* Read name column. */
	self->name = (char*) sqlite3_column_text (stmt, col);
	size = sqlite3_column_bytes (stmt, col++);
	if (size > 0 && self->name != NULL)
		self->name = listr_dup (self->name);
	else
		self->name = listr_dup ("");
	if (self->name == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Read model column. */
	self->model = (char*) sqlite3_column_text (stmt, col);
	size = sqlite3_column_bytes (stmt, col++);
	if (size > 0 && self->model != NULL)
		self->model = listr_dup (self->model);
	else
		self->model = listr_dup ("");
	if (self->model == NULL)
	{
		lisys_free (self->name);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Deserializes a material from a stream.
 *
 * \param reader Stream reader.
 * \return New material or NULL.
 */
livoxMaterial*
livox_material_new_from_stream (liarcReader* reader)
{
	uint32_t id;
	uint32_t flags;
	livoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
		return NULL;

	/* Read values. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint32 (reader, &flags) ||
	    !liarc_reader_get_float (reader, &self->friction) ||
	    !liarc_reader_get_text (reader, "", &self->name) ||
	    !liarc_reader_get_text (reader, "", &self->model))
	{
		lisys_free (self->model);
		lisys_free (self->name);
		lisys_free (self);
		return NULL;
	}
	self->id = id;
	self->flags = flags;

	return self;
}

/**
 * \brief Frees the material.
 *
 * \param self Material.
 */
void
livox_material_free (livoxMaterial* self)
{
	lisys_free (self->model);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Serializes the material to a database.
 *
 * \param self Material.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
livox_material_write_to_sql (livoxMaterial* self,
                             liarcSql*      sql)
{
	if (!liarc_sql_insert (sql, "voxel_materials",
		"id", LIARC_SQL_INT, self->id,
		"flags", LIARC_SQL_INT, self->flags,
		"fric", LIARC_SQL_FLOAT, self->friction,
		"name", LIARC_SQL_TEXT, self->name,
		"model", LIARC_SQL_TEXT, self->model, NULL))
		return 0;

	return 1;
}

/**
 * \brief Serializes the material to a stream.
 *
 * \param self Material.
 * \param writer Stream writer.
 * \return Nonzero on success.
 */
int
livox_material_write_to_stream (livoxMaterial* self,
                                liarcWriter*   writer)
{
	return liarc_writer_append_uint32 (writer, self->id) &&
	       liarc_writer_append_uint32 (writer, self->flags) &&
	       liarc_writer_append_float (writer, self->friction) &&
	       liarc_writer_append_string (writer, self->name) &&
	       liarc_writer_append_nul (writer) &&
	       liarc_writer_append_string (writer, self->model) &&
	       liarc_writer_append_nul (writer);
}

int
livox_material_set_name (livoxMaterial* self,
                         const char*    value)
{
	char* tmp;

	tmp = listr_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->name);
	self->name = tmp;

	return 1;
}

int
livox_material_set_model (livoxMaterial* self,
                          const char*    value)
{
	char* tmp;

	tmp = listr_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->model);
	self->model = tmp;

	return 1;
}

/** @} */
/** @} */
