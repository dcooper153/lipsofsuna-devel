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
 * \param module Terrain module.
 * \param chunk_size Number of grid points per chunk size.
 * \param grid_size Grid spacing in world units.
 * \return New terrain, or NULL.
 */
LIExtTerrain* liext_terrain_new (
	LIExtTerrainModule* module,
	int                 chunk_size,
	int                 grid_size)
{
	LIExtTerrain* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtTerrain));
	if (self == NULL)
		return NULL;
	self->module = module;
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

	/* Notify other extensions of the removal. */
	/* This is needed by physics-terrain at the time of writing. If not
	   informed, it would be left with an invalid pointer to this object. */
	lical_callbacks_call (self->module->program->callbacks, "terrain-free", lical_marshal_DATA_PTR, self);

	/* Free the terrain itself. */
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

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Add the stick. */
	return liext_terrain_chunk_add_stick (chunk, column_x, column_z, world_y, world_h, material);
}

/**
 * \brief Draws a stick at the given grid point with the given vertex offsets.
 * \param self Terrain chunk.
 * \param column_x X coordinate of the column, in grid units within the chunk.
 * \param column_z Z coordinate of the column, in grid units within the chunk.
 * \param bot00 Bottom vertex Y coordinate, in world units.
 * \param bot10 Bottom vertex Y coordinate, in world units.
 * \param bot01 Bottom vertex Y coordinate, in world units.
 * \param bot11 Bottom vertex Y coordinate, in world units.
 * \param top00 Top vertex Y coordinate, in world units.
 * \param top10 Top vertex Y coordinate, in world units.
 * \param top01 Top vertex Y coordinate, in world units.
 * \param top11 Top vertex Y coordinate, in world units.
 * \param material Terrain material ID.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_add_stick_corners (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	float         bot00,
	float         bot10,
	float         bot01,
	float         bot11,
	float         top00,
	float         top10,
	float         top01,
	float         top11,
	int           material)
{
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Add the stick. */
	return liext_terrain_chunk_add_stick_corners (chunk, column_x, column_z, bot00, bot10, bot01, bot11, top00, top10, top01, top11, material);
}

/**
 * \brief Calculates smooth normals for the (1,1) vertex of the column.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \return Nonzero on success, zero if not loaded.
 */
int liext_terrain_calculate_smooth_normals (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	int column_x[2][2];
	int column_z[2][2];
	LIExtTerrainChunkID id[2][2];
	LIExtTerrainChunk* chunk[2][2];
	LIExtTerrainColumn* column[2][2];

	/* Get the chunks. */
	id[0][0] = private_get_chunk_id_and_column (self, grid_x    , grid_z    , &column_x[0][0], &column_z[0][0]);
	id[1][0] = private_get_chunk_id_and_column (self, grid_x + 1, grid_z    , &column_x[1][0], &column_z[1][0]);
	id[0][1] = private_get_chunk_id_and_column (self, grid_x    , grid_z + 1, &column_x[0][1], &column_z[0][1]);
	id[1][1] = private_get_chunk_id_and_column (self, grid_x + 1, grid_z + 1, &column_x[1][1], &column_z[1][1]);
	chunk[0][0] = lialg_u32dic_find (self->chunks, id[0][0]);
	chunk[1][0] = lialg_u32dic_find (self->chunks, id[1][0]);
	chunk[0][1] = lialg_u32dic_find (self->chunks, id[0][1]);
	chunk[1][1] = lialg_u32dic_find (self->chunks, id[1][1]);
	if (chunk[0][0] == NULL || chunk[1][0] == NULL || chunk[0][1] == NULL || chunk[1][1] == NULL)
		return 0;

	/* Calculate the smooth normals. */
	column[0][0] = liext_terrain_chunk_get_column (chunk[0][0], column_x[0][0], column_z[0][0]);
	column[1][0] = liext_terrain_chunk_get_column (chunk[1][0], column_x[1][0], column_z[1][0]);
	column[0][1] = liext_terrain_chunk_get_column (chunk[0][1], column_x[0][1], column_z[0][1]);
	column[1][1] = liext_terrain_chunk_get_column (chunk[1][1], column_x[1][1], column_z[1][1]);
	liext_terrain_column_calculate_smooth_normals (column[0][0], column[1][0], column[0][1], column[1][1]);
	column[0][0]->stamp++;
	column[1][0]->stamp++;
	column[0][1]->stamp++;
	column[1][1]->stamp++;
	chunk[0][0]->stamp++;
	chunk[1][0]->stamp++;
	chunk[0][1]->stamp++;
	chunk[1][1]->stamp++;

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

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Clear the column. */
	liext_terrain_chunk_clear_column (chunk, column_x, column_z);

	return 1;
}

/**
 * \brief Casts a ray against the terrain.
 * \param self Terrain.
 * \param src Source point in world units.
 * \param dst Destination point in world units.
 * \param result_x Return location for the grid coordinates.
 * \param result_z Return location for the grid coordinates.
 * \param result_point Return location for the world unit hit point.
 * \param result_normal Return location for the normal vector.
 * \param result_fraction Return location for the ray fraction.
 * \return Nonzero if intersected.
 */
int liext_terrain_intersect_ray (
	LIExtTerrain*      self,
	const LIMatVector* src,
	const LIMatVector* dst,
	int*               result_x,
	int*               result_z,
	LIMatVector*       result_point,
	LIMatVector*       result_normal,
	float*             result_fraction)
{
	int x;
	int z;
	int prev_x;
	int prev_z;
	float f;
	float len;
	LIExtTerrainColumn* column;
	LIMatVector diff;
	LIMatVector dir;
	LIMatVector point;
	LIMatVector rel_src;
	LIMatVector rel_dst;

	/* Calculate the ray direction. */
	diff = limat_vector_subtract (*dst, *src);
	dir = limat_vector_normalize (diff);
	len = limat_vector_get_length (diff);
	prev_x = -1;
	prev_z = -1;

	/* Move along the ray and sample columns. */
	for (f = 0.0f ; f < len ; f++)
	{
		/* Determine the current grid point. */
		point = limat_vector_add (*src, limat_vector_multiply (dir, f));
		x = (int)(point.x / self->grid_size);
		z = (int)(point.z / self->grid_size);

		/* Skip out of bounds or alrady handled grid points. */
		if (x < 0 || z < 0)
			continue;
		if (x == prev_x && z == prev_z)
			continue;

		/* Collide against the column. */
		column = liext_terrain_get_column (self, x, z);
		if (column != NULL)
		{
			rel_src.x = src->x / self->grid_size - x;
			rel_src.y = src->y;
			rel_src.z = src->z / self->grid_size - z;
			rel_dst.x = dst->x / self->grid_size - x;
			rel_dst.y = dst->y;
			rel_dst.z = dst->z / self->grid_size - z;
			if (liext_terrain_column_intersect_ray (column, &rel_src, &rel_dst, result_point, result_normal, result_fraction))
			{
				*result_x = x;
				*result_z = z;
				*result_point = limat_vector_add (*result_point, limat_vector_init (x, 0.0f, z));
				result_point->x *= self->grid_size;
				result_point->z *= self->grid_size;
				return 1;
			}
		}

		/* Advance the sampling point. */
		prev_x = x;
		prev_z = z;
	}

	return 0;
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
 * \brief Smoothens the vertices of the given column.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param world_y Y offset of the smoothing range start.
 * \param world_h Height of the smoothing range.
 * \return Nonzero on success, zero if already unloaded.
 */
int liext_terrain_smoothen_column (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	float         world_y,
	float         world_h)
{
	int column_x[2][2];
	int column_z[2][2];
	LIExtTerrainChunkID id[2][2];
	LIExtTerrainChunk* chunk[2][2];
	LIExtTerrainColumn* column[2][2];

	/* Get the chunks. */
	id[0][0] = private_get_chunk_id_and_column (self, grid_x    , grid_z    , &column_x[0][0], &column_z[0][0]);
	id[1][0] = private_get_chunk_id_and_column (self, grid_x + 1, grid_z    , &column_x[1][0], &column_z[1][0]);
	id[0][1] = private_get_chunk_id_and_column (self, grid_x    , grid_z + 1, &column_x[0][1], &column_z[0][1]);
	id[1][1] = private_get_chunk_id_and_column (self, grid_x + 1, grid_z + 1, &column_x[1][1], &column_z[1][1]);
	chunk[0][0] = lialg_u32dic_find (self->chunks, id[0][0]);
	chunk[1][0] = lialg_u32dic_find (self->chunks, id[1][0]);
	chunk[0][1] = lialg_u32dic_find (self->chunks, id[0][1]);
	chunk[1][1] = lialg_u32dic_find (self->chunks, id[1][1]);
	if (chunk[0][0] == NULL || chunk[1][0] == NULL || chunk[0][1] == NULL || chunk[1][1] == NULL)
		return 0;

	/* Smoothen the column. */
	column[0][0] = liext_terrain_chunk_get_column (chunk[0][0], column_x[0][0], column_z[0][0]);
	column[1][0] = liext_terrain_chunk_get_column (chunk[1][0], column_x[1][0], column_z[1][0]);
	column[0][1] = liext_terrain_chunk_get_column (chunk[0][1], column_x[0][1], column_z[0][1]);
	column[1][1] = liext_terrain_chunk_get_column (chunk[1][1], column_x[1][1], column_z[1][1]);
	liext_terrain_column_smoothen (column[0][0], column[1][0], column[0][1], column[1][1], world_y, world_h);
	column[0][0]->stamp++;
	column[1][0]->stamp++;
	column[0][1]->stamp++;
	column[1][1]->stamp++;
	chunk[0][0]->stamp++;
	chunk[1][0]->stamp++;
	chunk[0][1]->stamp++;
	chunk[1][1]->stamp++;

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

/**
 * \brief Get the nearest chunk to the given grid point whose model is out of date.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param result_x Return location for the grid coordinate of the chunk.
 * \param result_z Return location for the grid coordinate of the chunk.
 * \return Nonzero if found, zero if all models are up to date.
 */
int liext_terrain_get_nearest_chunk_with_outdated_model (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	int*          result_x,
	int*          result_z)
{
	int best_x = -1;
	int best_z = -1;
	int chunk_x;
	int chunk_z;
	float best_dist = 0.0f;
	float chunk_dist;
	LIAlgU32dicIter iter;
	LIExtTerrainChunk* chunk;

	LIALG_U32DIC_FOREACH (iter, self->chunks)
	{
		chunk = iter.value;
		if (chunk->stamp == chunk->stamp_model)
			continue;
		chunk_x = (iter.key % 0xFFFF) * self->chunk_size;
		chunk_z = (iter.key / 0xFFFF) * self->chunk_size;
		chunk_dist = hypotf (chunk_x - grid_x, chunk_z - grid_z);
		if (best_x == -1 || chunk_dist < best_dist)
		{
			best_x = chunk_x;
			best_z = chunk_z;
			best_dist = chunk_dist;
		}
	}

	if (best_x == -1)
		return 0;
	*result_x = best_x;
	*result_z = best_z;

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
