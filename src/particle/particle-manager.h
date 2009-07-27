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
 * \addtogroup lipar Particle
 * @{
 * \addtogroup liparManager Manager
 * @{
 */

#ifndef __PARTICLE_MANAGER_H__
#define __PARTICLE_MANAGER_H__

#include "particle-line.h"
#include "particle-point.h"

typedef struct _liparManager liparManager;
struct _liparManager
{
	struct
	{
		int count;
		int count_used;
		int count_free;
		liparLine* all;
		liparLine* used;
		liparLine* free;
	} lines;
	struct
	{
		int count;
		int count_used;
		int count_free;
		liparPoint* all;
		liparPoint* used;
		liparPoint* free;
	} points;
};

liparManager*
lipar_manager_new (int points,
                   int lines);

void
lipar_manager_free (liparManager* self);

liparLine*
lipar_manager_insert_line (liparManager*      self,
                           const limatVector* position0,
                           const limatVector* position1,
                           const limatVector* velocity0,
                           const limatVector* velocity1);

liparPoint*
lipar_manager_insert_point (liparManager*      self,
                            const limatVector* position,
                            const limatVector* velocity);

void
lipar_manager_remove_line (liparManager* self,
                           liparLine*    line);

void
lipar_manager_remove_point (liparManager* self,
                            liparPoint*   point);

void
lipar_manager_update (liparManager* self,
                      float         secs);

#endif

/** @} */
/** @} */
