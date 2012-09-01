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

static LIExtTerrainChunkID private_get_chunk_id (
	LIExtTerrain* self,
	int           chunk_x,
	int           chunk_z);

static LIExtTerrainChunkID private_get_chunk_id_and_column (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	int*          column_x,
	int*          column_y);

/*****************************************************************************/

/**
 * \brief Creates a new terrain.
 * \param chunk_size Number of grid points per chunk size.
 * \param grid_size Grid spacing in world units.
 * \return New terrain, or NULL.
 */
LIExtTerrain* liext_terrain_new (
	int chunk_size,
	int grid_size)
{
	LIExtTerrain* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtTerrain));
	if (self == NULL)
		return NULL;
	self->chunk_size = chunk_size;
	self->grid_size = grid_size;

	/* Allocate the chunk dictionary. */
	self->chunks = lialg_u32dic_new ();
	if (self->chunks == NULL)
	{
		liext_terrain_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the terrain.
 * \param self Terrain.
 */
void liext_terrain_free (
	LIExtTerrain* self)
{
	LIAlgU32dicIter iter;

	if (self->chunks != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->chunks)
			liext_terrain_chunk_free (iter.value);
		lialg_u32dic_free (self->chunks);
	}
	lisys_free (self);
}

/**
 * \brief Draws a stick at the given grid point and Y offset.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param world_y Y offset of the stick in world units.
 * \param world_h Y height of the stick in world units.
 * \param material Terrain material ID.
 * \return Nonzero on success, zero if not loaded or allocating memory failed.
 */
int liext_terrain_add_stick (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	float         world_y,
	float         world_h,
	int           material)
{
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;
	LIExtTerrainColumn* column;

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Add the stick to the column. */
	column = liext_terrain_chunk_get_column (chunk, column_x, column_z);
	if (!liext_terrain_column_add_stick (column, world_y, world_h, material))
		return 0;
	chunk->stamp++;

	return 1;
}

/**
 * \brief Clears the stick at the given grid point.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \return Nonzero on success, zero if not loaded.
 */
int liext_terrain_clear_column (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;
	LIExtTerrainColumn* column;

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Clear the column. */
	column = liext_terrain_chunk_get_column (chunk, column_x, column_z);
	if (column->sticks == NULL)
		return 1;
	liext_terrain_column_clear (column);
	column->stamp++;
	chunk->stamp++;

	return 1;
}

/**
 * \brief Loads the chunk at the given grid point.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \return Nonzero if loaded or already loaded, zero if loading failed.
 */
int liext_terrain_load_chunk (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;

	/* Check for an existing chunk. */
	id = private_get_chunk_id (self, grid_x, grid_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk != NULL)
		return 1;

	/* Create the new chunk. */
	chunk = liext_terrain_chunk_new (self->chunk_size);
	if (chunk == NULL)
		return 0;

	/* Add the chunk to the dictionary. */
	if (!lialg_u32dic_insert (self->chunks, id, chunk))
	{
		liext_terrain_chunk_free (chunk);
		return 0;
	}

	return 1;
}

/**
 * \brief Unloads the chunk at the given grid point.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \return Nonzero if unloaded, zero if already unloaded.
 */
int liext_terrain_unload_chunk (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;

	/* Get the chunk. */
	id = private_get_chunk_id (self, grid_x, grid_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Remove the chunk from the dictionary. */
	lialg_u32dic_remove (self->chunks, id);

	/* Free the chunk. */
	liext_terrain_chunk_free (chunk);
	lialg_u32dic_remove (self->chunks, id);

	return 1;
}

/**
 * \brief Get the chunk at the given grid point.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \return Chunk, or NULL if not loaded.
 */
LIExtTerrainChunk* liext_terrain_get_chunk (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	LIExtTerrainChunkID id;

	id = private_get_chunk_id (self, grid_x, grid_z);

	return lialg_u32dic_find (self->chunks, id);
}

/**
 * \brief Gets the serialized contents of the chunk.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param writer Archive write.
 * \return Nonzero on success.
 */
int liext_terrain_get_chunk_data (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcWriter*  writer)
{
	LIExtTerrainChunk* chunk;

	/* Get the column. */
	chunk = liext_terrain_get_chunk (self, grid_x, grid_z);
	if (chunk == NULL)
		return 0;

	return liext_terrain_chunk_get_data (chunk, writer);
}

/**
 * \brief Sets the contents of the chunk from serialized data.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_set_chunk_data (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcReader*  reader)
{
	LIExtTerrainChunk* chunk;

	/* Get the column. */
	chunk = liext_terrain_get_chunk (self, grid_x, grid_z);
	if (chunk == NULL)
		return 0;

	return liext_terrain_chunk_set_data (chunk, reader);
}

/**
 * \brief Sets the column at the given grid point.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \return Column, or NULL if not loaded.
 */
LIExtTerrainColumn* liext_terrain_get_column (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return NULL;

	return liext_terrain_chunk_get_column (chunk, column_x, column_z);
}

/**
 * \brief Gets the serialized data of the column.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param writer Archive writer.
 * \return Nonzero on success.
 */
int liext_terrain_get_column_data (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcWriter*  writer)
{
	LIExtTerrainColumn* column;

	/* Get the column. */
	column = liext_terrain_get_column (self, grid_x, grid_z);
	if (column == NULL)
		return 0;

	return liext_terrain_column_get_data (column, writer);
}

/**
 * \brief Sets the contents of the column from serialized data.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_set_column_data (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	LIArcReader*  reader)
{
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;
	LIExtTerrainColumn* column;

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Set the column data. */
	column = liext_terrain_chunk_get_column (chunk, column_x, column_z);
	if (!liext_terrain_column_set_data (column, reader))
		return 0;
	chunk->stamp++;

	return 1;
}

/*****************************************************************************/

static LIExtTerrainChunkID private_get_chunk_id (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	int chunk_x = grid_x / self->chunk_size;
	int chunk_z = grid_z / self->chunk_size;

	return chunk_x + chunk_z * 0xFFFF;
}

static LIExtTerrainChunkID private_get_chunk_id_and_column (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	int*          column_x,
	int*          column_z)
{
	int chunk_x = grid_x / self->chunk_size;
	int chunk_z = grid_z / self->chunk_size;

	*column_x = grid_x % self->chunk_size;
	*column_z = grid_z % self->chunk_size;

	return chunk_x + chunk_z * 0xFFFF;
}

/** @} */
/** @} */
