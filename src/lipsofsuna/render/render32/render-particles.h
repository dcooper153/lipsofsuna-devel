/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __RENDER_PARTICLES_H__
#define __RENDER_PARTICLES_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "../render-image.h"

typedef struct _LIRenParticle LIRenParticle;
struct _LIRenParticle
{
	int buffer_start;
	int buffer_end;
	int frame_start;
	int frame_end;
};

typedef struct _LIRenParticleFrame LIRenParticleFrame;
struct _LIRenParticleFrame
{
	LIMatVector coord;
};

typedef struct _LIRenParticleSystem LIRenParticleSystem;
struct _LIRenParticleSystem
{
	int frame_start;
	int frame_end;
	int frame_end_emit;
	int buffer_start;
	int buffer_end;
	int particle_start;
	int particle_end;
	float particle_size;
	char* shader;
	LIRenImage* image;
};

typedef struct _LIRenParticles LIRenParticles;
struct _LIRenParticles
{
	LIMatAabb bounds;
	struct
	{
		int count;
		LIRenParticleFrame* array;
	} frames;
	struct
	{
		int count;
		LIRenParticle* array;
	} particles;
	struct
	{
		int count;
		LIRenParticleSystem* array;
	} systems;
};

LIAPICALL (int, liren_particles_init, (
	LIRenParticles* self,
	LIRenRender*    render,
	LIMdlModel*     model));

LIAPICALL (void, liren_particles_clear, (
	LIRenParticles* self));

LIAPICALL (int, liren_particles_evaluate_particle, (
	LIRenParticles* self,
	int             system,
	int             particle,
	float           time,
	int             loop,
	LIMatVector*    position,
	float*          color));

LIAPICALL (void, liren_particles_sort, (
	LIRenParticles*       self,
	float                 time,
	int                   loop,
	const LIMatTransform* transform,
	LIRenSort*            sort));

#endif
