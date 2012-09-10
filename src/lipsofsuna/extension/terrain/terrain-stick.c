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
	self->vertices[0][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[0][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);

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
 * \brief Copies the vertices from another stick.
 * \param self Terrain stick.
 * \param src Terrain stick.
 */
void liext_terrain_stick_copy_vertices (
	LIExtTerrainStick* self,
	LIExtTerrainStick* src)
{
	self->vertices[0][0] = src->vertices[0][0];
	self->vertices[1][0] = src->vertices[1][0];
	self->vertices[0][1] = src->vertices[0][1];
	self->vertices[1][1] = src->vertices[1][1];
}

/**
 * \brief Resets the vertices to the flat orientation.
 * \param self Terrain stick.
 */
void liext_terrain_stick_reset_vertices (
	LIExtTerrainStick* self)
{
	memset (self->vertices, 0, sizeof (self->vertices));
	self->vertices[0][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[0][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
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
	       liarc_writer_append_float (writer, self->vertices[0][0].offset) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].normal.z) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].offset) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].normal.z) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].offset) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].normal.z) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].offset) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].normal.z);
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
	int i;
	int x;
	int z;
	uint32_t tmp1;
	float tmp2[21];

	/* Read the values. */
	if (!liarc_reader_get_uint32 (reader, &tmp1))
		return 0;
	for (i = 0 ; i < 21 ; i++)
	{
		if (!liarc_reader_get_float (reader, tmp2 + i))
			return 0;
	}

	/* Validate the values to the best of our ability. */
	/* We check that the height is positive and that the vertex offsets are
	   not below the bottom of the stick. We cannot check that the offsets do
	   not extend above the bottom of the next stick. */
	if (tmp2[0] <= 0.0f)
		return 0;
	for (i = 0 ; i < 4 ; i++)
	{
		if (tmp2[5 * i + 1] > tmp2[0])
			tmp2[5 * i + 1] = tmp2[0];
	}

	/* Apply the values. */
	self->material = tmp1;
	self->height = tmp2[(i = 0)];
	for (z = 0 ; z < 2 ; z++)
	{
		for (x = 0 ; x < 2 ; x++)
		{
			self->vertices[x][z].offset = tmp2[++i];
			self->vertices[x][z].splatting = tmp2[++i];
			self->vertices[x][z].normal.x = tmp2[++i];
			self->vertices[x][z].normal.y = tmp2[++i];
			self->vertices[x][z].normal.z = tmp2[++i];
		}
	}

	return 1;
}

/**
 * \brief Calculates the face normal of the stick.
 * \param self Terrain stick.
 * \param result Return location for the vector.
 */
void liext_terrain_stick_get_normal (
	const LIExtTerrainStick* self,
	LIMatVector*             result)
{
	LIMatVector v1;
	LIMatVector v2;
	LIMatVector v3;
	LIMatVector v4;
	LIMatVector n1;
	LIMatVector n2;

	v1 = limat_vector_init (1.0f, self->vertices[1][0].offset - self->vertices[0][0].offset, 0.0f);
	v2 = limat_vector_init (0.0f, self->vertices[0][1].offset - self->vertices[0][0].offset, 1.0f);
	v3 = limat_vector_init (-1.0f, self->vertices[1][0].offset - self->vertices[1][1].offset, 0.0f);
	v4 = limat_vector_init (0.0f, self->vertices[0][1].offset - self->vertices[1][1].offset, -1.0f);
	n1 = limat_vector_normalize (limat_vector_cross (v2, v1));
	n2 = limat_vector_normalize (limat_vector_cross (v4, v3));
	*result = limat_vector_multiply (limat_vector_add (n1, n2), 0.5f);
}

/**
 * \brief Calculates the face normal of the stick when one vertex is overridden.
 * \param self Terrain stick.
 * \param result Return location for the vector.
 */
void liext_terrain_stick_get_normal_override (
	const LIExtTerrainStick* self,
	int                      vertex_x,
	int                      vertex_y,
	float                    vertex_offset,
	LIMatVector*             result)
{
	float offsets[2][2];
	LIMatVector v1;
	LIMatVector v2;
	LIMatVector v3;
	LIMatVector v4;
	LIMatVector n1;
	LIMatVector n2;

	offsets[0][0] = self->vertices[0][0].offset;
	offsets[1][0] = self->vertices[1][0].offset;
	offsets[0][1] = self->vertices[0][1].offset;
	offsets[1][1] = self->vertices[1][1].offset;
	offsets[vertex_x][vertex_y] = vertex_offset;
	v1 = limat_vector_init (1.0f, offsets[1][0] - offsets[0][0], 0.0f);
	v2 = limat_vector_init (0.0f, offsets[0][1] - offsets[0][0], 1.0f);
	v3 = limat_vector_init (-1.0f, offsets[1][0] - offsets[1][1], 0.0f);
	v4 = limat_vector_init (0.0f, offsets[0][1] - offsets[1][1], -1.0f);
	n1 = limat_vector_normalize (limat_vector_cross (v2, v1));
	n2 = limat_vector_normalize (limat_vector_cross (v4, v3));
	*result = limat_vector_multiply (limat_vector_add (n1, n2), 0.5f);
}

/** @} */
/** @} */
