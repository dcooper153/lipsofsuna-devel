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

#include "particle-point.h"

void
lipar_point_init (liparPoint*        self,
                  const limatVector* position,
                  const limatVector* velocity)
{
	self->position = *position;
	self->velocity = *velocity;
	self->acceleration = limat_vector_init (0.0f, 0.0f, 0.0f);
	self->time = 0.0f;
	self->time_fade = 0.0f;
	self->time_life = 5.0f;
	self->color[0] = 0.2f;
	self->color[1] = 0.15f;
	self->color[2] = 0.1f;
}

void
lipar_point_get_color (const liparPoint* self,
                       float*            color)
{
	float fade;

	fade = LI_MAX (self->time_life - self->time_fade, 0.0f);
	fade = LI_MAX (self->time - fade, 0.0f);
	color[0] = self->color[0];
	color[1] = self->color[1];
	color[2] = self->color[2];
	color[3] = 1.0f - fade / self->time_fade;
}

/** @} */
/** @} */
