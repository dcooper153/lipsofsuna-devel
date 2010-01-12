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

#include "particle-line.h"

void
lipar_line_init (LIParLine*         self,
                 const LIMatVector* position0,
                 const LIMatVector* position1,
                 const LIMatVector* velocity0,
                 const LIMatVector* velocity1)
{
	self->position[0] = *position0;
	self->position[1] = *position1;
	self->velocity[0] = *velocity0;
	self->velocity[1] = *velocity1;
	self->acceleration[0] = self->acceleration[1] = limat_vector_init (0.0f, 0.0f, 0.0f);
	self->time[0] = self->time[1] = 0.0f;
	self->time_fade[0] = self->time_fade[1] = 0.0f;
	self->time_life[0] = self->time_life[1] = 5.0f;
	self->color[0][0] = self->color[1][0] = 0.2f;
	self->color[0][1] = self->color[1][1] = 0.15f;
	self->color[0][2] = self->color[1][2] = 0.1f;
}

void
lipar_line_get_colors (const LIParLine* self,
                       float*           color0,
                       float*           color1)
{
	float fade;

	fade = LIMAT_MAX (self->time_life[0] - self->time_fade[0], 0.0f);
	fade = LIMAT_MAX (self->time[0] - fade, 0.0f);
	color0[0] = self->color[0][0];
	color0[1] = self->color[0][1];
	color0[2] = self->color[0][2];
	color0[3] = 1.0f - fade / self->time_fade[0];
	fade = LIMAT_MAX (self->time_life[1] - self->time_fade[1], 0.0f);
	fade = LIMAT_MAX (self->time[1] - fade, 0.0f);
	color1[0] = self->color[1][0];
	color1[1] = self->color[1][1];
	color1[2] = self->color[1][2];
	color1[3] = 1.0f - fade / self->time_fade[1];
}

/** @} */
/** @} */
