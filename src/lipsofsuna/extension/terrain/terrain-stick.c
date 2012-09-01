/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "module.h"

/**
 * \brief Creates a new terrain stick.
 * \return Terrain stick, or NULL.
 */
LIExtTerrainStick* liext_terrain_stick_new (
	int   material,
	float height)
{
	LIExtTerrainStick* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtTerrainStick));
	if (self == NULL)
		return NULL;
	self->material = material;
	self->height = height;

	return self;
}

/**
 * \brief Frees the stick.
 * \param self Terrain stick.
 */
void liext_terrain_stick_free (
	LIExtTerrainStick* self)
{
	lisys_free (self);
}

/**
 * \brief Gets the serialized contents of the stick.
 * \param self Terrain stick.
 * \param writer Archive write.
 * \return Nonzero on success.
 */
int liext_terrain_stick_get_data (
	LIExtTerrainStick* self,
	LIArcWriter*       writer)
{
	return liarc_writer_append_uint32 (writer, self->material) &&
	       liarc_writer_append_float (writer, self->height) &&
	       liarc_writer_append_float (writer, self->corners[0][0]) &&
	       liarc_writer_append_float (writer, self->corners[1][0]) &&
	       liarc_writer_append_float (writer, self->corners[0][1]) &&
	       liarc_writer_append_float (writer, self->corners[1][1]);
}

/**
 * \brief Sets the contents of the stick from serialized data.
 * \param self Terrain stick.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_stick_set_data (
	LIExtTerrainStick* self,
	LIArcReader*       reader)
{
	uint32_t tmp1;
	float tmp2[5];

	if (!liarc_reader_get_uint32 (reader, &tmp1) &&
	    !liarc_reader_get_float (reader, tmp2 + 0) &&
	    !liarc_reader_get_float (reader, tmp2 + 1) &&
	    !liarc_reader_get_float (reader, tmp2 + 2) &&
	    !liarc_reader_get_float (reader, tmp2 + 3) &&
	    !liarc_reader_get_float (reader, tmp2 + 4))
		return 0;
	self->material = tmp1;
	self->height = tmp2[0];
	self->corners[0][0] = tmp2[1];
	self->corners[1][0] = tmp2[2];
	self->corners[0][1] = tmp2[3];
	self->corners[1][1] = tmp2[4];

	return 1;
}

/** @} */
/** @} */
