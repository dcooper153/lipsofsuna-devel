/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndParticle Particle
 * @{
 */

#ifndef __RENDER_PARTICLE_H__
#define __RENDER_PARTICLE_H__

#include <math/lips-math.h>

typedef struct _lirndParticle lirndParticle;
struct _lirndParticle
{
	lirndParticle* prev;
	lirndParticle* next;
	limatVector position;
	limatVector velocity;
	limatVector acceleration;
	float time;
	float time_fade;
	float time_life;
};

void lirnd_particle_init      (lirndParticle*       self,
                               const limatVector*      position,
                               const limatVector*      velocity);
void lirnd_particle_get_color (const lirndParticle* self,
                               float*               color);

#endif

/** @} */
/** @} */
