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

#include "terrain-material.h"

/**
 * \brief Initializes the material.
 * \param self Material.
 * \param texture Texture number.
 */
void liext_terrain_material_init (
	LIExtTerrainMaterial* self,
	int                   texture)
{
	self->stick_type = LIEXT_TERRAIN_STICK_TYPE_DEFAULT;
	self->decoration_type = LIEXT_TERRAIN_DECORATION_TYPE_NONE;
	self->texture_top = texture;
	self->texture_bottom = texture;
	self->texture_side = texture;
	self->texture_decoration = texture;
	self->texture_scaling = 0.2f;
}

/** @} */
/** @} */
