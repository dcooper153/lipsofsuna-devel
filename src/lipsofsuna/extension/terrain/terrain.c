/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
#include "terrain.h"

static LIExtTerrainChunkID private_get_chunk_id (
	const LIExtTerrain* self,
	int                 grid_x,
	int                 grid_z);

static LIExtTerrainChunkID private_get_chunk_id_and_column (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	int*          column_x,
	int*          column_y);

static void private_mark_column_dirty (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z);

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
	float               grid_size)
{
	int i;
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

	/* Initialize the materials. */
	liext_terrain_material_init (self->materials, 0);
	for (i = 1 ; i < LIEXT_TERRAIN_MATERIAL_MAX ; ++i)
		liext_terrain_material_init (self->materials + i, i - 1);

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
 * \brief Draws a box of sticks at the given grid points.
 * \param self Terrain.
 * \param grid_x1 Minimum X coordinate in grid units.
 * \param grid_z1 Minimum Z coordinate in grid units.
 * \param grid_x2 Maximum X coordinate in grid units.
 * \param grid_z2 Maximum Z coordinate in grid units.
 * \param world_y Y offset of the box in world units.
 * \param world_h Y height of the box in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Number of successful stick insertions.
 */
int liext_terrain_add_box (
	LIExtTerrain*           self,
	int                     grid_x1,
	int                     grid_z1,
	int                     grid_x2,
	int                     grid_z2,
	float                   world_y,
	float                   world_h,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	int x;
	int z;
	int ret = 0;

	for (z = grid_z1 ; z <= grid_z2 ; z++)
	{
		for (x = grid_x1 ; x <= grid_x2 ; x++)
		{
			ret += liext_terrain_add_stick (self, x, z, world_y, world_h,
				material, filter_func, filter_data);
		}
	}

	return ret;
}

/**
 * \brief Draws a sloped box of sticks at the given grid points.
 * \param self Terrain.
 * \param grid_x1 Minimum X coordinate in grid units.
 * \param grid_z1 Minimum Z coordinate in grid units.
 * \param grid_x2 Maximum X coordinate in grid units.
 * \param grid_z2 Maximum Z coordinate in grid units.
 * \param bot00 Y offset of the box in world units.
 * \param top00 Y height of the box in world units.
 * \param bot10 Y offset of the box in world units.
 * \param top10 Y height of the box in world units.
 * \param bot01 Y offset of the box in world units.
 * \param top01 Y height of the box in world units.
 * \param bot11 Y offset of the box in world units.
 * \param top11 Y height of the box in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Number of successful stick insertions.
 */
int liext_terrain_add_box_corners (
	LIExtTerrain*           self,
	int                     grid_x1,
	int                     grid_z1,
	int                     grid_x2,
	int                     grid_z2,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	int x;
	int z;
	int sx;
	int sz;
	float b00, b10, b01, b11;
	float t00, t10, t01, t11;
	int ret = 0;

	sx = grid_x2 - grid_x1 + 1;
	sz = grid_z2 - grid_z1 + 1;
	for (z = 0 ; z < sz ; z++)
	{
		for (x = 0 ; x < sx ; x++)
		{
			b00 = limat_number_bilinear ((float) x    / sx, (float) z    / sz, bot00, bot10, bot01, bot11);
			b10 = limat_number_bilinear ((float)(x+1) / sx, (float) z    / sz, bot00, bot10, bot01, bot11);
			b01 = limat_number_bilinear ((float) x    / sx, (float)(z+1) / sz, bot00, bot10, bot01, bot11);
			b11 = limat_number_bilinear ((float)(x+1) / sx, (float)(z+1) / sz, bot00, bot10, bot01, bot11);
			t00 = limat_number_bilinear ((float) x    / sx, (float) z    / sz, top00, top10, top01, top11);
			t10 = limat_number_bilinear ((float)(x+1) / sx, (float) z    / sz, top00, top10, top01, top11);
			t01 = limat_number_bilinear ((float) x    / sx, (float)(z+1) / sz, top00, top10, top01, top11);
			t11 = limat_number_bilinear ((float)(x+1) / sx, (float)(z+1) / sz, top00, top10, top01, top11);
			ret += liext_terrain_add_stick_corners (self,
				grid_x1 + x, grid_z1 + z,
				b00, b10, b01, b11, t00, t10, t01, t11,
				material, filter_func, filter_data);
		}
	}

	return ret;
}

/**
 * \brief Draws a stick at the given grid point and Y offset.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param world_y Y offset of the stick in world units.
 * \param world_h Y height of the stick in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Nonzero on success, zero if not loaded or allocating memory failed.
 */
int liext_terrain_add_stick (
	LIExtTerrain*           self,
	int                     grid_x,
	int                     grid_z,
	float                   world_y,
	float                   world_h,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	int ret;
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
	ret = liext_terrain_chunk_add_stick (chunk, column_x, column_z, world_y, world_h,
		material, filter_func, filter_data);

	/* Mark neighbor chunks dirty on border updates. */
	if (column_x == 0)
		private_mark_column_dirty (self, grid_x - 1, grid_z);
	if (column_x == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x + 1, grid_z);
	if (column_z == 0)
		private_mark_column_dirty (self, grid_x, grid_z - 1);
	if (column_z == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x, grid_z + 1);

	return ret;
}

/**
 * \brief Draws a stick at the given grid point with the given vertex offsets.
 * \param self Terrain chunk.
 * \param grid_x X coordinate of the column, in grid units within the chunk.
 * \param grid_z Z coordinate of the column, in grid units within the chunk.
 * \param bot00 Bottom vertex Y coordinate, in world units.
 * \param bot10 Bottom vertex Y coordinate, in world units.
 * \param bot01 Bottom vertex Y coordinate, in world units.
 * \param bot11 Bottom vertex Y coordinate, in world units.
 * \param top00 Top vertex Y coordinate, in world units.
 * \param top10 Top vertex Y coordinate, in world units.
 * \param top01 Top vertex Y coordinate, in world units.
 * \param top11 Top vertex Y coordinate, in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_add_stick_corners (
	LIExtTerrain*           self,
	int                     grid_x,
	int                     grid_z,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	int ret;
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
	ret = liext_terrain_chunk_add_stick_corners (chunk, column_x, column_z, bot00, bot10, bot01, bot11, top00, top10, top01, top11,
		material, filter_func, filter_data);

	/* Mark neighbor chunks dirty on border updates. */
	if (column_x == 0)
		private_mark_column_dirty (self, grid_x - 1, grid_z);
	if (column_x == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x + 1, grid_z);
	if (column_z == 0)
		private_mark_column_dirty (self, grid_x, grid_z - 1);
	if (column_z == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x, grid_z + 1);

	return ret;
}

/**
 * \brief Builds the model of the chunk.
 * \param self Terrain chunk.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param offset Offset of the chunk, in world units.
 * \return Model owned by the terrain on success. NULL otherwise.
 */
LIMdlModel* liext_terrain_build_chunk_model (
	LIExtTerrain*      self,
	int                grid_x,
	int                grid_z,
	const LIMatVector* offset)
{
	int chunk_w;
	LIExtTerrainChunk* chunk;
	LIExtTerrainChunk* chunk_left;
	LIExtTerrainChunk* chunk_right;
	LIExtTerrainChunk* chunk_front;
	LIExtTerrainChunk* chunk_back;

	/* Get the chunk. */
	chunk = liext_terrain_get_chunk (self, grid_x, grid_z);
	if (chunk == NULL)
		return NULL;

	/* Get the neighbor chunks for culling. */
	chunk_w = self->chunk_size;
	chunk_left = (grid_x >= chunk_w)? liext_terrain_get_chunk (self, grid_x - chunk_w, grid_z) : NULL;
	chunk_right = liext_terrain_get_chunk (self, grid_x + chunk_w, grid_z);
	chunk_front = (grid_z >= chunk_w)? liext_terrain_get_chunk (self, grid_x, grid_z - chunk_w) : NULL;
	chunk_back = liext_terrain_get_chunk (self, grid_x, grid_z + chunk_w);

	/* Build the model. */
	if (!liext_terrain_chunk_build_model (chunk, self->materials,
		chunk_back, chunk_front, chunk_left, chunk_right, self->grid_size, offset))
	{
		return NULL;
	}

	return chunk->model;
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
 * \brief Casts a sphere against the terrain and returns the hit fraction.
 * \param self Terrain.
 * \param sphere_cast_start Cast start position of the sphere, in grid units.
 * \param sphere_cast_end Cast end position of the sphere, in grid units.
 * \param sphere_radius Sphere radius, in grid units.
 * \param result Return location for the hit fraction.
 * \return Nonzero if hit. Zero otherwise.
 */
int liext_terrain_cast_sphere (
	const LIExtTerrain*    self,
	const LIMatVector*     sphere_cast_start,
	const LIMatVector*     sphere_cast_end,
	float                  sphere_radius,
	LIExtTerrainCollision* result)
{
	int i;
	int j;
	int min_x;
	int max_x;
	int min_z;
	int max_z;
	LIExtTerrainChunk* chunk;
	LIExtTerrainChunkID id;
	LIExtTerrainCollision best;
	LIExtTerrainCollision frac;
	LIMatVector pos;
	LIMatVector cast1;
	LIMatVector cast2;

	best.fraction = LIMAT_INFINITE;
	min_x = (int)((LIMAT_MIN (sphere_cast_start->x, sphere_cast_end->x) - sphere_radius) / self->chunk_size);
	max_x = (int)((LIMAT_MAX (sphere_cast_start->x, sphere_cast_end->x) + sphere_radius) / self->chunk_size);
	min_z = (int)((LIMAT_MIN (sphere_cast_start->z, sphere_cast_end->z) - sphere_radius) / self->chunk_size);
	max_z = (int)((LIMAT_MAX (sphere_cast_start->z, sphere_cast_end->z) + sphere_radius) / self->chunk_size);
	min_x = LIMAT_MAX (min_x, 0);
	max_x = LIMAT_MIN (max_x, 0xFFFF);
	min_z = LIMAT_MAX (min_z, 0);
	max_z = LIMAT_MIN (max_z, 0xFFFF);

	for (j = min_z ; j <= max_z ; j++)
	{
		for (i = min_x ; i <= max_x ; i++)
		{
			/* Find the chunk. */
			id = private_get_chunk_id (self, i * self->chunk_size, j * self->chunk_size);
			chunk = lialg_u32dic_find (self->chunks, id);
			if (chunk == NULL)
				continue;

			/* Transform the coordinates to the chunk space. */
			pos = limat_vector_init (i * self->chunk_size, 0.0f, j * self->chunk_size);
			cast1 = limat_vector_subtract (*sphere_cast_start, pos);
			cast2 = limat_vector_subtract (*sphere_cast_end, pos);

			/* Cast against the chunk. */
			if (liext_terrain_chunk_cast_sphere (chunk, &cast1, &cast2, sphere_radius, &frac))
			{
				if (best.fraction > frac.fraction)
				{
					best = frac;
					best.x += i;
					best.z += j;
					best.point = limat_vector_add (best.point, pos);
				}
			}
		}
	}

	if (best.fraction > 1.0f)
		return 0;
	*result = best;
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

	/* Mark neighbor chunks dirty on border updates. */
	if (column_x == 0)
		private_mark_column_dirty (self, grid_x - 1, grid_z);
	if (column_x == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x + 1, grid_z);
	if (column_z == 0)
		private_mark_column_dirty (self, grid_x, grid_z - 1);
	if (column_z == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x, grid_z + 1);

	return 1;
}

/**
 * \brief Clears the stick at the given grid point.
 * \param self Terrain.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 * \param y Y offset.
 * \param min_height Minimum stick height.
 * \return Y offset.
 * \return Nonzero on success, zero if not loaded.
 */
float liext_terrain_find_nearest_empty_stick (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z,
	float         y,
	float         min_height)
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
		return 1000000000.0f;

	/* Find the stick. */
	column = liext_terrain_chunk_get_column (chunk, column_x, column_z);
	return liext_terrain_column_find_nearest_empty_stick (column, y, min_height);
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
	int i;
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
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

	/* Mark neighbor chunks dirty. */
	grid_x -= column_x;
	grid_z -= column_z;
	for (i = 0 ; i < self->chunk_size ; ++i)
	{
		private_mark_column_dirty (self, grid_x - 1               , grid_z + i);
		private_mark_column_dirty (self, grid_x + self->chunk_size, grid_z + i);
		private_mark_column_dirty (self, grid_x + i               , grid_z - 1);
		private_mark_column_dirty (self, grid_x + i               , grid_z + self->chunk_size);
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
 * \brief Gets the total number of loaded chunks.
 * \param self Terrain.
 * \return Chunk count.
 */
int liext_terrain_get_chunk_count (
	const LIExtTerrain* self)
{
	return self->chunks->size;
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
	int i;
	int ret;
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;

	/* Get the chunk. */
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk == NULL)
		return 0;

	/* Set the data. */
	ret = liext_terrain_chunk_set_data (chunk, reader);

	/* Mark neighbor chunks dirty. */
	grid_x -= column_x;
	grid_z -= column_z;
	for (i = 0 ; i < self->chunk_size ; ++i)
	{
		private_mark_column_dirty (self, grid_x - 1               , grid_z + i);
		private_mark_column_dirty (self, grid_x + self->chunk_size, grid_z + i);
		private_mark_column_dirty (self, grid_x + i               , grid_z - 1);
		private_mark_column_dirty (self, grid_x + i               , grid_z + self->chunk_size);
	}

	return ret;
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

	/* Mark neighbor chunks dirty on border updates. */
	if (column_x == 0)
		private_mark_column_dirty (self, grid_x - 1, grid_z);
	if (column_x == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x + 1, grid_z);
	if (column_z == 0)
		private_mark_column_dirty (self, grid_x, grid_z - 1);
	if (column_z == self->chunk_size - 1)
		private_mark_column_dirty (self, grid_x, grid_z + 1);

	return 1;
}

/**
 * \brief Gets the estimate of how much memory the terrain uses.
 * \param self Terrain.
 * \return Memory consumption in bytes.
 */
int liext_terrain_get_memory_used (
	const LIExtTerrain* self)
{
	int total;
	LIAlgU32dicIter iter;

	total = sizeof (*self);
	LIALG_U32DIC_FOREACH (iter, self->chunks)
		total += liext_terrain_chunk_get_memory_used (iter.value);

	return total;
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
	const LIExtTerrain* self,
	int                 grid_x,
	int                 grid_z)
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

static void private_mark_column_dirty (
	LIExtTerrain* self,
	int           grid_x,
	int           grid_z)
{
	int column_x;
	int column_z;
	LIExtTerrainChunkID id;
	LIExtTerrainChunk* chunk;
	LIExtTerrainColumn* column;

	if (grid_x < 0 || grid_z < 0)
		return;
	id = private_get_chunk_id_and_column (self, grid_x, grid_z, &column_x, &column_z);
	chunk = lialg_u32dic_find (self->chunks, id);
	if (chunk != NULL)
	{
		column = liext_terrain_chunk_get_column (chunk, column_x, column_z);
		column->stamp++;
		chunk->stamp++;
	}
}

/** @} */
/** @} */
