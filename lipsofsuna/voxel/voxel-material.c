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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup LIVoxMaterial Material
 * @{
 */

#include <lipsofsuna/string.h>
#include "voxel-material.h"

/**
 * \brief Creates a new material.
 *
 * \return New material or NULL.
 */
LIVoxMaterial*
livox_material_new ()
{
	int flags;
	LIVoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxMaterial));
	if (self == NULL)
		return NULL;
	self->type = LIVOX_MATERIAL_TYPE_TILE;

	/* Allocate name. */
	self->name = listr_dup ("");
	if (self->name == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Allocate model. */
	self->model = listr_dup ("");
	if (self->model == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Allocate materials. */
	flags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	if (!limdl_material_init (&self->mat_side) ||
	    !limdl_material_init (&self->mat_top) ||
	    !limdl_material_append_texture (&self->mat_side, LIMDL_TEXTURE_TYPE_IMAGE, flags, "stone-000") ||
	    !limdl_material_append_texture (&self->mat_top, LIMDL_TEXTURE_TYPE_IMAGE, flags, "grass-000"))
	{
		livox_material_free (self);
		return NULL;
	}

	/* FIXME: Abusing for texture scaling... */
	self->mat_side.emission = 0.9f;
	self->mat_top.emission = 0.9f;

	return self;
}

/**
 * \brief Creates a copy of a material.
 *
 * \param src Material to copy.
 * \return Soft copy of the material or NULL.
 */
LIVoxMaterial*
livox_material_new_copy (const LIVoxMaterial* src)
{
	LIVoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxMaterial));
	if (self == NULL)
		return NULL;

	/* Copy values. */
	self->id = src->id;
	self->flags = src->flags;
	self->type = src->type;
	self->friction = src->friction;

	/* Copy name. */
	self->name = listr_dup (src->name);
	if (self->name == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Copy model. */
	self->model = listr_dup (src->model);
	if (self->model == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Copy materials. */
	if (!limdl_material_init_copy (&self->mat_side, &src->mat_side) ||
	    !limdl_material_init_copy (&self->mat_top, &src->mat_top))
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
LIVoxMaterial*
livox_material_new_from_stream (LIArcReader* reader)
{
	LIVoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxMaterial));
	if (self == NULL)
		return NULL;

	/* Read from stream. */
	if (!livox_material_read (self, reader))
	{
		livox_material_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the material.
 *
 * \param self Material.
 */
void
livox_material_free (LIVoxMaterial* self)
{
	limdl_material_free (&self->mat_side);
	limdl_material_free (&self->mat_top);
	lisys_free (self->model);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Deserializes the material from a stream.
 *
 * The contents of the material are replaced with data read from the stream.
 * If the read fails, the function returns without modifying the material.
 *
 * \param self Material.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int
livox_material_read (LIVoxMaterial* self,
                     LIArcReader*   reader)
{
	float friction;
	char* model;
	char* name;
	uint32_t id;
	uint32_t flags;
	uint32_t type;
	LIMdlMaterial tmpmat[2];

	/* Initialize temporaries. */
	model = NULL;
	name = NULL;
	memset (tmpmat + 0, 0, sizeof (LIMdlMaterial));
	memset (tmpmat + 1, 0, sizeof (LIMdlMaterial));

	/* Read into temporaries. */
	if (!liarc_reader_get_uint32 (reader, &id) ||
	    !liarc_reader_get_uint32 (reader, &flags) ||
	    !liarc_reader_get_uint32 (reader, &type) ||
	    !liarc_reader_get_float (reader, &friction) ||
	    !liarc_reader_get_text (reader, "", &model) ||
	    !liarc_reader_get_text (reader, "", &name) ||
	    !limdl_material_read (tmpmat + 0, reader) ||
	    !limdl_material_read (tmpmat + 1, reader))
	{
		limdl_material_free (tmpmat + 0);
		limdl_material_free (tmpmat + 1);
		lisys_free (model);
		lisys_free (name);
		livox_material_free (self);
		return 0;
	}

	/* Succeeded so free old data and copy over. */
	lisys_free (self->model);
	lisys_free (self->name);
	limdl_material_free (&self->mat_side);
	limdl_material_free (&self->mat_top);
	self->id = id;
	self->flags = flags;
	self->type = type;
	self->friction = friction;
	self->model = model;
	self->name = name;
	self->mat_side = tmpmat[0];
	self->mat_top = tmpmat[1];

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
livox_material_write (LIVoxMaterial* self,
                      LIArcWriter*   writer)
{
	return liarc_writer_append_uint32 (writer, self->id) &&
	       liarc_writer_append_uint32 (writer, self->flags) &&
	       liarc_writer_append_uint32 (writer, self->type) &&
	       liarc_writer_append_float (writer, self->friction) &&
	       liarc_writer_append_string (writer, self->model) &&
	       liarc_writer_append_nul (writer) &&
	       liarc_writer_append_string (writer, self->name) &&
	       liarc_writer_append_nul (writer) &&
	       limdl_material_write (&self->mat_side, writer) &&
	       limdl_material_write (&self->mat_top, writer);
}

int
livox_material_set_name (LIVoxMaterial* self,
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
livox_material_set_model (LIVoxMaterial* self,
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
