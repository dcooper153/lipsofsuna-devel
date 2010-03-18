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
 * \addtogroup lipar Particle
 * @{
 * \addtogroup LIParGroup Group
 * @{
 */

#ifndef __PARTICLE_GROUP_H__
#define __PARTICLE_GROUP_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "particle-point.h"

typedef struct _LIParGroup LIParGroup;
struct _LIParGroup
{
	char* texture;
	struct
	{
		int count;
		int count_used;
		int count_free;
		LIParPoint* all;
		LIParPoint* used;
		LIParPoint* free;
	} points;
};

LIAPICALL (LIParGroup*, lipar_group_new, (
	const char* texture));

LIAPICALL (void, lipar_group_free, (
	LIParGroup* self));

LIAPICALL (LIParPoint*, lipar_group_insert_point, (
	LIParGroup*        self,
	const LIMatVector* position,
	const LIMatVector* velocity));

LIAPICALL (void, lipar_group_update, (
	LIParGroup* self,
	float       secs));

#endif

/** @} */
/** @} */
