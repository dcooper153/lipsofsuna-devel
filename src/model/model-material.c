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
 * \brief Removes all textures from the material.
 *
 * \param self Material.
 */
void
limdl_material_clear_textures (limdlMaterial* self)
{
	int i;

	for (i = 0 ; i < self->textures.count ; i++)
		free (self->textures.array[i].string);
	free (self->textures.array);
	self->textures.array = NULL;
	self->textures.count = 0;
}

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
	uint32_t tmp[2];

	/* Read header. */
	if (!li_reader_get_uint32 (reader, tmp + 0) ||
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
		!li_reader_get_float (reader, &self->strand_start) ||
		!li_reader_get_float (reader, &self->strand_end) ||
		!li_reader_get_float (reader, &self->strand_shape) ||
		!li_reader_get_uint32 (reader, tmp + 1))
		return 0;
	self->flags = tmp[0];
	self->textures.count = tmp[1];

	/* Read shader. */
	if (!li_reader_get_text (reader, "", &self->shader))
		return 0;

	/* Read textures. */
	if (self->textures.count > 0)
	{
		self->textures.array = calloc (self->textures.count, sizeof (limdlTexture));
		if (self->textures.array == NULL)
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
			    !li_reader_get_text (reader, "", &self->textures.array[i].string))
				return 0;
			self->textures.array[i].type = tmp[0];
			self->textures.array[i].flags = tmp[1];
			self->textures.array[i].width = tmp[2];
			self->textures.array[i].height = tmp[3];
		}
	}

	return 1;
}

/**
 * \brief Allocates or reallocates the textures of the material.
 *
 * \param self Material.
 * \param count Texture count.
 * \return Nonzero on success.
 */
int
limdl_material_realloc_textures (limdlMaterial* self,
                                 int            count)
{
	int i;
	limdlTexture* tmp;

	if (count == self->textures.count)
		return 1;
	if (!count)
	{
		limdl_material_clear_textures (self);
		return 1;
	}
	else if (count < self->textures.count)
	{
		for (i = count ; i < self->textures.count ; i++)
			free (self->textures.array[i].string);
		tmp = realloc (self->textures.array, count * sizeof (limdlTexture));
		if (tmp != NULL)
			self->textures.array = tmp;
		self->textures.count = count;
	}
	else
	{
		tmp = realloc (self->textures.array, count * sizeof (limdlTexture));
		if (tmp == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			return 0;
		}
		self->textures.array = tmp;
		for (i = self->textures.count ; i < count ; i++)
		{
			self->textures.array[i].type = LIMDL_TEXTURE_TYPE_EMPTY;
			self->textures.array[i].flags = LIMDL_TEXTURE_FLAG_REPEAT;
			self->textures.array[i].width = 0;
			self->textures.array[i].height = 0;
			self->textures.array[i].string = NULL;
		}
		self->textures.count = count;
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

	if (!liarc_writer_append_uint32 (writer, self->flags) ||
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
	    !liarc_writer_append_float (writer, self->strand_start) ||
	    !liarc_writer_append_float (writer, self->strand_end) ||
	    !liarc_writer_append_float (writer, self->strand_shape) ||
	    !liarc_writer_append_uint32 (writer, self->textures.count) ||
	    !liarc_writer_append_string (writer, self->shader) ||
	    !liarc_writer_append_nul (writer))
		return 0;
	for (i = 0 ; i < self->textures.count ; i++)
	{
		texture = self->textures.array + i;
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

/**
 * \brief Sets texture unit information.
 *
 * \param self Material.
 * \param unit Texture unit.
 * \param type Texture type.
 * \param flags Texture parameters.
 * \param name Image name.
 * \return Nonzero on success.
 */
int
limdl_material_set_texture (limdlMaterial* self,
                            int            unit,
                            int            type,
                            int            flags,
                            const char*    name)
{
	char* dup;

	assert (unit >= 0);
	assert (unit < self->textures.count);

	dup = strdup (name);
	if (dup == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	free (self->textures.array[unit].string);
	self->textures.array[unit].type = type;
	self->textures.array[unit].flags = flags;
	self->textures.array[unit].string = dup;

	return 1;
}

/** @} */
/** @} */
