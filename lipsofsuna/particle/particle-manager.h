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
 * \addtogroup LIParManager Manager
 * @{
 */

#ifndef __PARTICLE_MANAGER_H__
#define __PARTICLE_MANAGER_H__

#include "particle-line.h"
#include "particle-point.h"

typedef struct _LIParManager LIParManager;
struct _LIParManager
{
	struct
	{
		int count;
		int count_used;
		int count_free;
		LIParLine* all;
		LIParLine* used;
		LIParLine* free;
	} lines;
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

LIParManager*
lipar_manager_new (int points,
                   int lines);

void
lipar_manager_free (LIParManager* self);

LIParLine*
lipar_manager_insert_line (LIParManager*      self,
                           const LIMatVector* position0,
                           const LIMatVector* position1,
                           const LIMatVector* velocity0,
                           const LIMatVector* velocity1);

LIParPoint*
lipar_manager_insert_point (LIParManager*      self,
                            const LIMatVector* position,
                            const LIMatVector* velocity);

void
lipar_manager_remove_line (LIParManager* self,
                           LIParLine*    line);

void
lipar_manager_remove_point (LIParManager* self,
                            LIParPoint*   point);

void
lipar_manager_update (LIParManager* self,
                      float         secs);

#endif

/** @} */
/** @} */
