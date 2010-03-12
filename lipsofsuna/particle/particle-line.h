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
 * \addtogroup LIParLine Line
 * @{
 */

#ifndef __PARTICLE_LINE_H__
#define __PARTICLE_LINE_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>

typedef struct _LIParLine LIParLine;
struct _LIParLine
{
	float time[2];
	float time_fade[2];
	float time_life[2];
	float color[2][3];
	LIMatVector position[2];
	LIMatVector velocity[2];
	LIMatVector acceleration[2];
	LIParLine* prev;
	LIParLine* next;
};

LIAPICALL (void, lipar_line_init, (
	LIParLine*         self,
	const LIMatVector* position0,
	const LIMatVector* position1,
	const LIMatVector* velocity0,
	const LIMatVector* velocity1));

LIAPICALL (void, lipar_line_get_colors, (
	const LIParLine* self,
	float*           color0,
	float*           color1));

#endif

/** @} */
/** @} */
