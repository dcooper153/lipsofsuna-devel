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
 * \addtogroup liparPoint Point
 * @{
 */

#ifndef __PARTICLE_POINT_H__
#define __PARTICLE_POINT_H__

#include <math/lips-math.h>

typedef struct _liparPoint liparPoint;
struct _liparPoint
{
	float time;
	float time_fade;
	float time_life;
	float color[3];
	limatVector position;
	limatVector velocity;
	limatVector acceleration;
	liparPoint* prev;
	liparPoint* next;
};

void
lipar_point_init (liparPoint*        self,
                  const limatVector* position,
                  const limatVector* velocity);

void
lipar_point_get_color (const liparPoint* self,
                       float*            color);

#endif

/** @} */
/** @} */
