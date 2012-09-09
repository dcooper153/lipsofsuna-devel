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
	if (self->model != NULL)
		limdl_model_free (self->model);
	lisys_free (self);
}

/**
 * \brief Draws a stick at the given grid point and Y offset.
 * \param self Terrain chunk.
 * \param column_x X coordinate of the column, in grid units within the chunk.
 * \param column_z Z coordinate of the column, in grid units within the chunk.
 * \param world_y Y offset of the stick in world units.
 * \param world_h Y height of the stick in world units.
 * \param material Terrain material ID.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_chunk_add_stick (
	LIExtTerrainChunk* self,
	int                column_x,
	int                column_z,
	float              world_y,
	float              world_h,
	int                material)
{
	LIExtTerrainColumn* column;

	/* Add the stick to the column. */
	column = liext_terrain_chunk_get_column (self, column_x, column_z);
	if (!liext_terrain_column_add_stick (column, world_y, world_h, material))
		return 0;

	/* Update stamps. */
	/* The stamps of the neighbor columns are updated because face culling
	   may have changed due to the addition of the stick. */
	self->stamp++;
	if (column_x > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x - 1, column_z);
		column->stamp++;
	}
	if (column_x < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x + 1, column_z);
		column->stamp++;
	}
	if (column_z > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z - 1);
		column->stamp++;
	}
	if (column_z < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z + 1);
		column->stamp++;
	}

	return 1;
}

/**
 * \brief Builds the model of the chunk.
 * \param self Terrain chunk.
 * \param grid_size Grid size.
 * \return Nonzero on success, zero on failure.
 */
int liext_terrain_chunk_build_model (
	LIExtTerrainChunk* self,
	float              grid_size)
{
	int i;
	int j;
	LIExtTerrainColumn* column;
	LIExtTerrainStick* sticks_back;
	LIExtTerrainStick* sticks_front;
	LIExtTerrainStick* sticks_left;
	LIExtTerrainStick* sticks_right;
	LIMatTransform transform;
	LIMdlBuilder* builder;

	/* Check if changes are needed. */
	if (self->stamp == self->stamp_model)
		return 1;

	/* Build the column models. */
	for (j = 0 ; j < self->size ; j++)
	{
		for (i = 0 ; i < self->size ; i++)
		{
			/* Get the neighbor sticks. */
			sticks_back = NULL;
			sticks_front = NULL;
			sticks_left = NULL;
			sticks_right = NULL;
			if (i > 0)
			{
				column = self->columns + (i - 1) + j * self->size;
				sticks_left = column->sticks;
			}
			if (i < self->size - 1)
			{
				column = self->columns + (i + 1) + j * self->size;
				sticks_right = column->sticks;
			}
			if (j > 0)
			{
				column = self->columns + i + (j - 1) * self->size;
				sticks_front = column->sticks;
			}
			if (j < self->size - 1)
			{
				column = self->columns + (j + 1) * self->size;
				sticks_back = column->sticks;
			}

			/* Build the model. */
			column = self->columns + i + j * self->size;
			if (!liext_terrain_column_build_model (column, sticks_back, sticks_front, sticks_left, sticks_right, grid_size))
				return 0;
		}
	}

	/* Allocate the model. */
	if (self->model == NULL)
	{
		self->model = limdl_model_new ();
		if (self->model == NULL)
			return 0;
	}
	else
		limdl_model_clear (self->model);

	/* Allocate the model builder. */
	builder = limdl_builder_new (self->model);
	if (builder == NULL)
		return 0;

	/* Merge the models. */
	for (j = 0 ; j < self->size ; j++)
	{
		for (i = 0 ; i < self->size ; i++)
		{
			column = self->columns + i + j * self->size;
			if (column->model != NULL)
			{
				transform = limat_transform_init (limat_vector_init (i * grid_size, 0.0f, j * grid_size), limat_quaternion_identity ());
				limdl_builder_merge_model (builder, column->model, &transform);
			}
		}
	}

	/* Finish the build. */
	limdl_builder_finish (builder);
	limdl_builder_free (builder);
	self->stamp_model = self->stamp;

	/* Sanity checks. */
	lisys_assert (self->model->materials.count <= 1);
	lisys_assert (self->model->lod.count == 1);
	lisys_assert (self->model->lod.array[0].face_groups.count <= 1);

	return 1;
}

/**
 * \brief Clears the stick at the given grid point.
 * \param self Terrain chunk.
 * \param grid_x X coordinate in grid units.
 * \param grid_z Z coordinate in grid units.
 */
void liext_terrain_chunk_clear_column (
	LIExtTerrainChunk* self,
	int                column_x,
	int                column_z)
{
	LIExtTerrainColumn* column;

	/* Clear the column. */
	column = liext_terrain_chunk_get_column (self, column_x, column_z);
	if (column->sticks == NULL)
		return;
	liext_terrain_column_clear (column);

	/* Update stamps. */
	/* The stamps of the neighbor columns are updated because face culling
	   may have changed due to the removal of the sticks. */
	self->stamp++;
	column->stamp++;
	if (column_x > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x - 1, column_z);
		column->stamp++;
	}
	if (column_x < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x + 1, column_z);
		column->stamp++;
	}
	if (column_z > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z - 1);
		column->stamp++;
	}
	if (column_z < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z + 1);
		column->stamp++;
	}
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
