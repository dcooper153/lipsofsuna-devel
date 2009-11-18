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
 */
void
livox_voxel_rotate (livoxVoxel* self)
{
	/* TODO: Support all 24 orientations. */
	/* TODO: Support rotating around different axis. */
	self->rotation++;
	if (self->rotation >= 4)
		self->rotation = 0;
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
	switch (self->rotation)
	{
		case 1: *value = limat_quaternion_rotation (0.5f * M_PI, limat_vector_init (0.0f, 1.0f, 0.0f)); break;
		case 2: *value = limat_quaternion_rotation (1.0f * M_PI, limat_vector_init (0.0f, 1.0f, 0.0f)); break;
		case 3: *value = limat_quaternion_rotation (1.5f * M_PI, limat_vector_init (0.0f, 1.0f, 0.0f)); break;
		/* TODO: Add the rest of the 24 orientations here. */
		default:
			*value = limat_quaternion_identity ();
			break;
	}
}

/** @} */
/** @} */
