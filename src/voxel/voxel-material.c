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

/**
 * \brief Deserializes a material from an SQL statement.
 *
 * \param sql SQL database.
 * \param stmt SQL statement;
 * \return New material or NULL.
 */
livoxMaterial*
livox_material_new (liarcSql*     sql,
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

	/* TODO: Read textures. */
	self->model.textures.count = 1;
	self->model.textures.textures = calloc (1, sizeof (limdlTexture));
	self->model.textures.textures[0].type = LIMDL_TEXTURE_TYPE_IMAGE;
	self->model.textures.textures[0].flags = LIMDL_TEXTURE_FLAG_REPEAT | LIMDL_TEXTURE_FLAG_MIPMAP;
	self->model.textures.textures[0].width = 256;
	self->model.textures.textures[0].height = 256;
	self->model.textures.textures[0].string = "grass-000";

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
	printf ("FIXME: livox_material_free: free textures\n");
	free (self->model.shader);
	free (self->name);
	free (self);
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

/** @} */
/** @} */
