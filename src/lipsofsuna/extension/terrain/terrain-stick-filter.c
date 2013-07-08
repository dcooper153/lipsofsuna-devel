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

#include "terrain-stick-filter.h"

/**
 * \brief Filters sticks that match the material ID.
 * \param stick Terrain stick.
 * \param data Integer pointer to the material ID.
 * \return Nonzero if matched, zero if not.
 */
int liext_terrain_stick_filter_id (
	LIExtTerrainStick* stick,
	void*              data)
{
	int* id = data;

	return stick->material == *id;
}

/**
 * \brief Filters sticks that match the material mask.
 * \param stick Terrain stick.
 * \param data Integer pointer to the material mask.
 * \return Nonzero if matched, zero if not.
 */
int liext_terrain_stick_filter_mask (
	LIExtTerrainStick* stick,
	void*              data)
{
	int* mask = data;

	return *mask & (1 << stick->material);
}

/** @} */
/** @} */
