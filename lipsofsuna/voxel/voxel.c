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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxVoxel Voxel
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
livox_voxel_init (livoxVoxel* self,
                  int         type)
{
	self->type = type;
	self->damage = 0;
	self->rotation = 0;
}

/**
 * \brief Rotates the voxel.
 *
 * \param self Voxel.
 * \param axis Axis of rotation. 0 = X, 1 = Y, 2 = Z.
 * \param step Number of steps to rotate.
 */
void
livox_voxel_rotate (livoxVoxel* self,
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
 * \brief Gets the terrain type of the voxel.
 *
 * \param self Voxel.
 * \return Terrain type.
 */
int
livox_voxel_get_type (const livoxVoxel* self)
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
livox_voxel_get_quaternion (const livoxVoxel* self,
                            limatQuaternion*  value)
{
	static const limatQuaternion rotations[64] =
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
