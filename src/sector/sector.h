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
 * \addtogroup lisec Sector 
 * @{
 * \addtogroup lisecSector Sector 
 * @{
 */

#ifndef __SECTOR_H__
#define __SECTOR_H__

#include <math/lips-math.h>

#define LISEC_SECTOR_INVALID 0xFFFFFFFF
#define LISEC_SECTOR_SIZE 100

/**
 * \brief The size of a map sector as a vector presentation.
 */
static const limatVector lisec_sector_size =
{
	LISEC_SECTOR_SIZE,
	LISEC_SECTOR_SIZE,
	LISEC_SECTOR_SIZE
};

#endif

/** @} */
/** @} */

