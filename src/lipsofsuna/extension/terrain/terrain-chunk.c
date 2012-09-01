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
 * \brief Creates a new terrain chunk.
 * \param size Chunk size along one axis in columns.
 * \return Terrain chunk, or NULL.
 */
LIExtTerrainChunk* liext_terrain_chunk_new (
	int size)
{
	LIExtTerrainChunk* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtTerrainChunk));
	if (self == NULL)
		return NULL;
	self->size = size;

	/* Allocate the chunk array. */
	self->columns = lisys_calloc (size * size, sizeof (LIExtTerrainColumn));
	if (self->columns == NULL)
	{
		liext_terrain_chunk_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the chunk.
 * \param self Terrain chunk.
 */
void liext_terrain_chunk_free (
	LIExtTerrainChunk* self)
{
	int i;

	if (self->columns != NULL)
	{
		for (i = 0 ; i < self->size * self->size ; i++)
			liext_terrain_column_clear (self->columns + i);
	}
	lisys_free (self);
}

/**
 * \brief Gets a column by the grid point within the chunk.
 * \param self Terrain chunk.
 * \param x X grid coordinate withing the chunk.
 * \param z Z grid coordinate withing the chunk.
 * \return Column.
 */
LIExtTerrainColumn* liext_terrain_chunk_get_column (
	LIExtTerrainChunk* self,
	int                x,
	int                z)
{
	return self->columns + x + z * self->size;
}

/**
 * \brief Gets the serialized contents of the chunk.
 * \param self Terrain chunk.
 * \param writer Archive write.
 * \return Nonzero on success.
 */
int liext_terrain_chunk_get_data (
	LIExtTerrainChunk* self,
	LIArcWriter*       writer)
{
	int i;

	for (i = 0 ; i < self->size * self->size ; i++)
	{
		if (!liext_terrain_column_get_data (self->columns + i, writer))
			return 0;
	}

	return 1;
}

/**
 * \brief Sets the contents of the chunk from serialized data.
 * \param self Terrain chunk.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_chunk_set_data (
	LIExtTerrainChunk* self,
	LIArcReader*       reader)
{
	int i;

	self->stamp++;
	for (i = 0 ; i < self->size * self->size ; i++)
	{
		if (!liext_terrain_column_set_data (self->columns + i, reader))
			return 0;
		self->columns[i].stamp++;
	}

	return 1;
}

/** @} */
/** @} */
