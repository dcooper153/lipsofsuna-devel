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
 * \param type Terrain type.
 * \return Voxel.
 */
void
livox_voxel_init (livoxVoxel* self,
                  int         type)
{
	self->terrain = type;
	self->displacex = 0;
	self->displacey = 0;
	self->displacez = 0;
}

/**
 * \brief Gets the terrain type of the voxel.
 *
 * \param self Voxel.
 * \return Terrain type.
 */
int
livox_voxel_get_type (livoxVoxel self)
{
	return self.terrain;
}

/** @} */
/** @} */
