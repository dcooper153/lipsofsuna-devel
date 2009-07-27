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
 * \addtogroup liparLine Line
 * @{
 */

#ifndef __PARTICLE_LINE_H__
#define __PARTICLE_LINE_H__

#include <math/lips-math.h>

typedef struct _liparLine liparLine;
struct _liparLine
{
	float time[2];
	float time_fade[2];
	float time_life[2];
	float color[2][3];
	limatVector position[2];
	limatVector velocity[2];
	limatVector acceleration[2];
	liparLine* prev;
	liparLine* next;
};

void
lipar_line_init (liparLine*         self,
                 const limatVector* position0,
                 const limatVector* position1,
                 const limatVector* velocity0,
                 const limatVector* velocity1);

void
lipar_line_get_colors (const liparLine* self,
                       float*           color0,
                       float*           color1);

#endif

/** @} */
/** @} */
