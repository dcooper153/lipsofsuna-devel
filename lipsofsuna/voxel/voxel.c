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
 * \addtogroup LIVoxVoxel Voxel
 * @{
 */

#include "voxel.h"

/**
 * \brief Initializes a voxel of the given type.
 *
 * \param self Voxel.
 * \param type Terrain type.
 */
void
livox_voxel_init (LIVoxVoxel* self,
                  int         type)
{
	self->type = type;
	self->damage = 0;
	self->rotation = 0;
}

/**
 * \brief Writes the voxel to a stream.
 *
 * \param self Voxel.
 * \param writer Stream writer.
 * \return Nonzero on success.
 */
int
livox_voxel_read (LIVoxVoxel*  self,
                  LIArcReader* reader)
{
	uint16_t terrain;
	uint8_t damage;
	uint8_t rotation;

	if (!liarc_reader_get_uint16 (reader, &terrain) ||
		!liarc_reader_get_uint8 (reader, &damage) ||
		!liarc_reader_get_uint8 (reader, &rotation))
		return 0;
	self->type = terrain;
	self->damage = damage;
	self->rotation = rotation;

	return 1;
}

/**
 * \brief Rotates the voxel.
 *
 * \param self Voxel.
 * \param axis Axis of rotation. 0 = X, 1 = Y, 2 = Z.
 * \param step Number of steps to rotate.
 */
void
livox_voxel_rotate (LIVoxVoxel* self,
                    int         axis,
                    int         step)
{
	int xr;
	int yr;
	int zr;

	xr = self->rotation % 4;
	yr = self->rotation / 4 % 4;
	zr = self->rotation / 4 / 4 % 4;
	if (step < 0)
		step = (step % 4) + 4;
	switch (axis)
	{
		case 0: xr = (xr + step) % 4; break;
		case 1: yr = (yr + step) % 4; break;
		case 2: zr = (zr + step) % 4; break;
	}
	self->rotation = xr + 4 * yr + 4 * 4 * zr;
}

/**
 * \brief Writes the voxel to a stream.
 *
 * \param self Voxel.
 * \param writer Stream writer.
 * \return Nonzero on success.
 */
int
livox_voxel_write (LIVoxVoxel*  self,
                   LIArcWriter* writer)
{
	return liarc_writer_append_uint16 (writer, self->type) &&
	       liarc_writer_append_uint8 (writer, self->damage) &&
	       liarc_writer_append_uint8 (writer, self->rotation);
}

/**
 * \brief Gets the height of the voxel.
 *
 * This assumes that the voxel is a heightmap tile. If it is not, the returned
 * value is undefined. Otherwise, the value is within range [-1,1].
 *
 * \param self Voxel.
 * \return Height.
 */
float
livox_voxel_get_height (const LIVoxVoxel* self)
{
	/* FIXME: Should use `rotation' too. */
	return 1.0f - 2.0f * (self->damage / 255.0f);
}

/**
 * \brief Gets the terrain type of the voxel.
 *
 * \param self Voxel.
 * \return Terrain type.
 */
int
livox_voxel_get_type (const LIVoxVoxel* self)
{
	return self->type;
}

/**
 * \brief Gets the rotation quaternion of the voxel.
 *
 * \param self Voxel.
 * \param value Return location for the quaternion.
 */
void
livox_voxel_get_quaternion (const LIVoxVoxel* self,
                            LIMatQuaternion*  value)
{
	static const LIMatQuaternion rotations[64] =
	{
		{ 0.000000, 0.000000, 0.000000, 1.000000 },
		{ 0.707107, 0.000000, 0.000000, 0.707107 },
		{ 1.000000, 0.000000, 0.000000, -0.000000 },
		{ 0.707107, 0.000000, 0.000000, -0.707107 },
		{ 0.000000, 0.707107, 0.000000, 0.707107 },
		{ 0.500000, 0.500000, 0.500000, 0.500000 },
		{ 0.707107, -0.000000, 0.707107, -0.000000 },
		{ 0.500000, -0.500000, 0.500000, -0.500000 },
		{ 0.000000, 1.000000, 0.000000, -0.000000 },
		{ -0.000000, 0.707107, 0.707107, -0.000000 },
		{ -0.000000, -0.000000, 1.000000, 0.000000 },
		{ -0.000000, -0.707107, 0.707107, 0.000000 },
		{ 0.000000, 0.707107, 0.000000, -0.707107 },
		{ -0.500000, 0.500000, 0.500000, -0.500000 },
		{ -0.707107, -0.000000, 0.707107, 0.000000 },
		{ -0.500000, -0.500000, 0.500000, 0.500000 },
		{ 0.000000, 0.000000, 0.707107, 0.707107 },
		{ 0.500000, -0.500000, 0.500000, 0.500000 },
		{ 0.707107, -0.707107, -0.000000, -0.000000 },
		{ 0.500000, -0.500000, -0.500000, -0.500000 },
		{ 0.500000, 0.500000, 0.500000, 0.500000 },
		{ 0.707107, 0.000000, 0.707107, 0.000000 },
		{ 0.500000, -0.500000, 0.500000, -0.500000 },
		{ 0.000000, -0.707107, 0.000000, -0.707107 },
		{ 0.707107, 0.707107, -0.000000, -0.000000 },
		{ 0.500000, 0.500000, 0.500000, -0.500000 },
		{ -0.000000, 0.000000, 0.707107, -0.707107 },
		{ -0.500000, -0.500000, 0.500000, -0.500000 },
		{ 0.500000, 0.500000, -0.500000, -0.500000 },
		{ 0.000000, 0.707107, 0.000000, -0.707107 },
		{ -0.500000, 0.500000, 0.500000, -0.500000 },
		{ -0.707107, 0.000000, 0.707107, 0.000000 },
		{ 0.000000, 0.000000, 1.000000, -0.000000 },
		{ -0.000000, -0.707107, 0.707107, -0.000000 },
		{ -0.000000, -1.000000, -0.000000, 0.000000 },
		{ -0.000000, -0.707107, -0.707107, 0.000000 },
		{ 0.707107, -0.000000, 0.707107, -0.000000 },
		{ 0.500000, -0.500000, 0.500000, -0.500000 },
		{ -0.000000, -0.707107, -0.000000, -0.707107 },
		{ -0.500000, -0.500000, -0.500000, -0.500000 },
		{ 1.000000, -0.000000, -0.000000, 0.000000 },
		{ 0.707107, 0.000000, -0.000000, -0.707107 },
		{ -0.000000, 0.000000, -0.000000, -1.000000 },
		{ -0.707107, 0.000000, 0.000000, -0.707107 },
		{ 0.707107, -0.000000, -0.707107, 0.000000 },
		{ 0.500000, 0.500000, -0.500000, -0.500000 },
		{ 0.000000, 0.707107, 0.000000, -0.707107 },
		{ -0.500000, 0.500000, 0.500000, -0.500000 },
		{ 0.000000, 0.000000, 0.707107, -0.707107 },
		{ -0.500000, -0.500000, 0.500000, -0.500000 },
		{ -0.707107, -0.707107, -0.000000, 0.000000 },
		{ -0.500000, -0.500000, -0.500000, 0.500000 },
		{ 0.500000, -0.500000, 0.500000, -0.500000 },
		{ 0.000000, -0.707107, 0.000000, -0.707107 },
		{ -0.500000, -0.500000, -0.500000, -0.500000 },
		{ -0.707107, 0.000000, -0.707107, 0.000000 },
		{ 0.707107, -0.707107, -0.000000, 0.000000 },
		{ 0.500000, -0.500000, -0.500000, -0.500000 },
		{ 0.000000, 0.000000, -0.707107, -0.707107 },
		{ -0.500000, 0.500000, -0.500000, -0.500000 },
		{ 0.500000, -0.500000, -0.500000, 0.500000 },
		{ 0.707107, 0.000000, -0.707107, 0.000000 },
		{ 0.500000, 0.500000, -0.500000, -0.500000 },
		{ 0.000000, 0.707107, 0.000000, -0.707107 }
	};

	*value = rotations[self->rotation < 64? self->rotation : 0];
}

/** @} */
/** @} */
