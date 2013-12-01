/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_PARTICLE_MODEL_PARTICLES_H__
#define __EXT_PARTICLE_MODEL_PARTICLES_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"

typedef struct _LIExtModelParticle LIExtModelParticle;
struct _LIExtModelParticle
{
	int buffer_start;
	int buffer_end;
	int frame_start;
	int frame_end;
};

typedef struct _LIExtModelParticleFrame LIExtModelParticleFrame;
struct _LIExtModelParticleFrame
{
	LIMatVector coord;
};

typedef struct _LIExtModelParticleSystem LIExtModelParticleSystem;
struct _LIExtModelParticleSystem
{
	int frame_start;
	int frame_end;
	int frame_end_emit;
	int buffer_start;
	int buffer_end;
	int particle_start;
	int particle_end;
	float particle_size;
	char* material;
	char* texture;
};

typedef struct _LIExtModelParticles LIExtModelParticles;
struct _LIExtModelParticles
{
	struct
	{
		int count;
		LIExtModelParticleFrame* array;
	} frames;
	struct
	{
		int count;
		LIExtModelParticle* array;
	} particles;
	struct
	{
		int count;
		LIExtModelParticleSystem* array;
	} systems;
};

LIAPICALL (int, liext_model_particles_init, (
	LIExtModelParticles* self,
	const LIMdlModel*    model));

LIAPICALL (void, liext_model_particles_clear, (
	LIExtModelParticles* self));

LIAPICALL (int, liext_model_particles_evaluate_particle, (
	LIExtModelParticles* self,
	int                  system,
	int                  particle,
	float                time,
	int                  loop,
	LIMatVector*         position,
	float*               color));

#endif
