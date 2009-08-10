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

#include "voxel-material.h"

static int
private_read_textures (livoxMaterial* self,
                       liarcSql*      sql);

/*****************************************************************************/

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
	self = calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Allocate name. */
	self->name = strdup ("");
	if (self->name == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self);
		return NULL;
	}

	/* Initialize material. */
	if (!limdl_material_init (&self->model))
	{
		free (self->name);
		free (self);
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
	self = calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->id = src->id;
	self->friction = src->friction;
	self->scale = src->scale;
	self->model = src->model;
	self->name = strdup (src->name);
	if (self->name == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self);
		return NULL;
	}
	if (!limdl_material_init_copy (&self->model, &src->model))
	{
		free (self->name);
		free (self);
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
	self = calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Read numeric values. */
	col = 0;
	self->id = sqlite3_column_int (stmt, col++);
	self->model.flags = sqlite3_column_int (stmt, col++);
	self->friction = sqlite3_column_double (stmt, col++);
	self->scale = sqlite3_column_double (stmt, col++);
	self->model.shininess = sqlite3_column_double (stmt, col++);
	self->model.diffuse[0] = sqlite3_column_double (stmt, col++);
	self->model.diffuse[1] = sqlite3_column_double (stmt, col++);
	self->model.diffuse[2] = sqlite3_column_double (stmt, col++);
	self->model.diffuse[3] = sqlite3_column_double (stmt, col++);
	self->model.specular[0] = sqlite3_column_double (stmt, col++);
	self->model.specular[1] = sqlite3_column_double (stmt, col++);
	self->model.specular[2] = sqlite3_column_double (stmt, col++);
	self->model.specular[3] = sqlite3_column_double (stmt, col++);

	/* Read name column. */
	self->name = (char*) sqlite3_column_text (stmt, col);
	size = sqlite3_column_bytes (stmt, col++);
	if (size > 0 && self->name != NULL)
		self->name = strdup (self->name);
	else
		self->name = strdup ("");
	if (self->name == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self);
		return NULL;
	}

	/* Read shader column. */
	self->model.shader = (char*) sqlite3_column_text (stmt, col);
	size = sqlite3_column_bytes (stmt, col++);
	if (size > 0 && self->model.shader != NULL)
		self->model.shader = strdup (self->model.shader);
	else
		self->model.shader = strdup ("default");
	if (self->model.shader == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self->name);
		free (self);
		return NULL;
	}

	/* Read textures. */
	if (!private_read_textures (self, sql))
	{
		livox_material_free (self);
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
livox_material_new_from_stream (liReader* reader)
{
	uint32_t id;
	livoxMaterial* self;

	/* Allocate self. */
	self = calloc (1, sizeof (livoxMaterial));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Read values. */
	if (!li_reader_get_uint32 (reader, &id) ||
	    !li_reader_get_text (reader, "", &self->name) ||
	    !li_reader_get_float (reader, &self->friction) ||
	    !li_reader_get_float (reader, &self->scale) ||
	    !limdl_material_read (&self->model, reader))
	{
		free (self->name);
		free (self);
		return NULL;
	}
	self->id = id;

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
	limdl_material_clear_textures (&self->model);
	free (self->model.shader);
	free (self->name);
	free (self);
}

/**
 * \brief Appends a new texture to the material.
 *
 * \param self Material.
 * \param string Texture string.
 * \return Nonzero on success.
 */
int
livox_material_append_texture (livoxMaterial* self,
                               const char*    string)
{
	int i;
	int flags;

	i = self->model.textures.count;
	if (!limdl_material_realloc_textures (&self->model, i + 1))
		return 0;
	flags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	if (!limdl_material_set_texture (&self->model, i, LIMDL_TEXTURE_TYPE_IMAGE, flags, string))
	{
		limdl_material_realloc_textures (&self->model, i);
		return 0;
	}

	return 1;
}

/**
 * \brief Removes a texture from the material.
 *
 * \param self Material.
 * \param index Texture index.
 */
void
livox_material_remove_texture (livoxMaterial* self,
                               int            index)
{
	assert (index >= 0);
	assert (index < self->model.textures.count);

	free (self->model.textures.array[index].string);
	lialg_array_remove (&self->model.textures, index);
}

/**
 * \brief Serializes the material to a database.
 *
 * \param self Material.
 * \param writer Stream writer.
 * \return Nonzero on success.
 */
int
livox_material_write_to_sql (livoxMaterial* self,
                             liarcSql*      sql)
{
	int i;
	limdlTexture* texture;

	if (!liarc_sql_insert (sql, "voxel_materials",
		"id", LIARC_SQL_INT, self->id,
		"flags", LIARC_SQL_INT, self->model.flags,
		"fric", LIARC_SQL_FLOAT, self->friction,
		"scal", LIARC_SQL_FLOAT, self->scale,
		"shi", LIARC_SQL_FLOAT, self->model.shininess,
		"dif0", LIARC_SQL_FLOAT, self->model.diffuse[0],
		"dif1", LIARC_SQL_FLOAT, self->model.diffuse[1],
		"dif2", LIARC_SQL_FLOAT, self->model.diffuse[2],
		"dif3", LIARC_SQL_FLOAT, self->model.diffuse[3],
		"spe0", LIARC_SQL_FLOAT, self->model.specular[0],
		"spe1", LIARC_SQL_FLOAT, self->model.specular[1],
		"spe2", LIARC_SQL_FLOAT, self->model.specular[2],
		"spe3", LIARC_SQL_FLOAT, self->model.specular[3],
		"name", LIARC_SQL_TEXT, self->name,
		"shdr", LIARC_SQL_TEXT, self->model.shader, NULL))
		return 0;

	for (i = 0 ; i < self->model.textures.count ; i++)
	{
		texture = self->model.textures.array + i;
		if (!liarc_sql_insert (sql, "voxel_textures",
			"mat", LIARC_SQL_INT, self->id,
			"unit", LIARC_SQL_INT, i,
			"flags", LIARC_SQL_INT, texture->flags,
			"name", LIARC_SQL_TEXT, texture->string, NULL))
			return 0;
	}

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
	       liarc_writer_append_string (writer, self->name) &&
	       liarc_writer_append_nul (writer) &&
	       liarc_writer_append_float (writer, self->friction) &&
	       liarc_writer_append_float (writer, self->scale) &&
	       limdl_material_write (&self->model, writer);
}

int
livox_material_set_name (livoxMaterial* self,
                         const char*    value)
{
	char* tmp;

	tmp = strdup (value);
	if (tmp == NULL)
		return 0;
	free (self->name);
	self->name = tmp;

	return 1;
}

/*****************************************************************************/

static int
private_read_textures (livoxMaterial* self,
                       liarcSql*      sql)
{
	int col;
	int ret;
	int unit;
	int flags;
	int size;
	const char* name;
	const char* query;
	sqlite3_stmt* statement;

	/* Prepare statement. */
	query = "SELECT unit,flags,name FROM voxel_textures WHERE mat=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, self->id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Read textures. */
	for (ret = sqlite3_step (statement) ; ret != SQLITE_DONE ; ret = sqlite3_step (statement))
	{
		/* Check for errors. */
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}

		/* Read values. */
		col = 0;
		unit = sqlite3_column_int (statement, col++);
		if (unit < 0)
			continue;
		flags = sqlite3_column_int (statement, col++);
		name = (char*) sqlite3_column_text (statement, col);
		size = sqlite3_column_bytes (statement, col++);
		if (!size || name == NULL)
			name = "";

		/* Allocate materials. */
		if (self->model.textures.count <= unit)
		{
			if (!limdl_material_realloc_textures (&self->model, unit + 1))
			{
				sqlite3_finalize (statement);
				return 0;
			}
		}

		/* Set values. */
		if (!limdl_material_set_texture (&self->model, unit, LIMDL_TEXTURE_TYPE_IMAGE, flags, name))
		{
			sqlite3_finalize (statement);
			return 0;
		}
	}
	sqlite3_finalize (statement);

	return 1;
}

/** @} */
/** @} */
