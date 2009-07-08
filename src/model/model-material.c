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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlMaterial Material
 * @{
 */

#include "model-material.h"

/**
 * \brief Deserializes the material from a stream.
 *
 * \param self Material.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int
limdl_material_read (limdlMaterial* self,
                     liReader*      reader)
{
	int i;
	uint32_t tmp[4];

	/* Read header. */
	if (!li_reader_get_uint32 (reader, tmp + 0) ||
		!li_reader_get_uint32 (reader, tmp + 1) ||
		!li_reader_get_uint32 (reader, tmp + 2) ||
		!li_reader_get_float (reader, &self->emission) ||
		!li_reader_get_float (reader, &self->shininess) ||
		!li_reader_get_float (reader, self->diffuse + 0) ||
		!li_reader_get_float (reader, self->diffuse + 1) ||
		!li_reader_get_float (reader, self->diffuse + 2) ||
		!li_reader_get_float (reader, self->diffuse + 3) ||
		!li_reader_get_float (reader, self->specular + 0) ||
		!li_reader_get_float (reader, self->specular + 1) ||
		!li_reader_get_float (reader, self->specular + 2) ||
		!li_reader_get_float (reader, self->specular + 3) ||
		!li_reader_get_uint32 (reader, tmp + 3))
		return 0;
	self->start = tmp[0];
	self->end = tmp[1];
	self->flags = tmp[2];
	self->textures.count = tmp[3];

	/* Read shader. */
	if (!li_reader_get_text (reader, "", &self->shader))
		return 0;

	/* Read textures. */
	if (self->textures.count > 0)
	{
		self->textures.textures = calloc (self->textures.count, sizeof (limdlTexture));
		if (self->textures.textures == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			return 0;
		}
		for (i = 0 ; i < self->textures.count ; i++)
		{
			if (!li_reader_get_uint32 (reader, tmp + 0) ||
			    !li_reader_get_uint32 (reader, tmp + 1) ||
			    !li_reader_get_uint32 (reader, tmp + 2) ||
			    !li_reader_get_uint32 (reader, tmp + 3) ||
			    !li_reader_get_text (reader, "", &self->textures.textures[i].string))
				return 0;
			self->textures.textures[i].type = tmp[0];
			self->textures.textures[i].flags = tmp[1];
			self->textures.textures[i].width = tmp[2];
			self->textures.textures[i].height = tmp[3];
		}
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
limdl_material_write (limdlMaterial* self,
                      liarcWriter*   writer)
{
	int i;
	limdlTexture* texture;

	if (!liarc_writer_append_uint32 (writer, self->start) ||
	    !liarc_writer_append_uint32 (writer, self->end) ||
	    !liarc_writer_append_uint32 (writer, self->flags) ||
	    !liarc_writer_append_float (writer, self->emission) ||
	    !liarc_writer_append_float (writer, self->shininess) ||
	    !liarc_writer_append_float (writer, self->diffuse[0]) ||
	    !liarc_writer_append_float (writer, self->diffuse[1]) ||
	    !liarc_writer_append_float (writer, self->diffuse[2]) ||
	    !liarc_writer_append_float (writer, self->diffuse[3]) ||
	    !liarc_writer_append_float (writer, self->specular[0]) ||
	    !liarc_writer_append_float (writer, self->specular[1]) ||
	    !liarc_writer_append_float (writer, self->specular[2]) ||
	    !liarc_writer_append_float (writer, self->specular[3]) ||
	    !liarc_writer_append_uint32 (writer, self->textures.count) ||
	    !liarc_writer_append_string (writer, self->shader) ||
	    !liarc_writer_append_nul (writer))
		return 0;
	for (i = 0 ; i < self->textures.count ; i++)
	{
		texture = self->textures.textures + i;
		if (!liarc_writer_append_uint32 (writer, texture->type) ||
		    !liarc_writer_append_uint32 (writer, texture->flags) ||
		    !liarc_writer_append_uint32 (writer, texture->width) ||
		    !liarc_writer_append_uint32 (writer, texture->height) ||
		    !liarc_writer_append_string (writer, texture->string) ||
		    !liarc_writer_append_nul (writer))
			return 0;
	}

	return 1;
}

/** @} */
/** @} */
