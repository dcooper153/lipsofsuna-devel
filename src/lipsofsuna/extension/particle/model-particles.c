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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtParticle Particle
 * @{
 */

#include "model-particles.h"

#define TIMESCALE 0.02f

static int private_evaluate_frame (
	LIExtModelParticles* self,
	int                  system,
	int                  particle,
	int                  frame,
	int                  loop);

/*****************************************************************************/

/**
 * \brief Initializes the model particle effect evaluator.
 * \param self Model particles.
 * \param model Model.
 * \return One on success. Zero otherwise.
 */
int liext_model_particles_init (
	LIExtModelParticles* self,
	const LIMdlModel*    model)
{
	int i;
	int j;
	int k;
	int l;
	LIMdlParticle* srcparticle;
	LIMdlParticleFrame* srcframe;
	LIMdlParticleSystem* srcsystem;
	LIExtModelParticle* dstparticle;
	LIExtModelParticleFrame* dstframe;
	LIExtModelParticleSystem* dstsystem;

	memset (self, 0, sizeof (LIExtModelParticles));

	/* Create the particle systems and count particles. */
	if (model->particle_systems.count)
	{
		/* Allocate the particle system information. */
		self->systems.count = model->particle_systems.count;
		self->systems.array = lisys_calloc (self->systems.count, sizeof (LIExtModelParticleSystem));
		if (self->systems.array == NULL)
		{
			liext_model_particles_clear (self);
			return 0;
		}

		/* Copy the particle system information. */
		for (i = 0 ; i < model->particle_systems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particle_systems.array + i;
			dstsystem->frame_start = srcsystem->frame_start;
			dstsystem->frame_end = srcsystem->frame_end;
			dstsystem->frame_end_emit = srcsystem->frame_end_emit;
			dstsystem->particle_start = self->particles.count;
			dstsystem->particle_size = srcsystem->particle_size;
			dstsystem->material = lisys_string_dup (srcsystem->shader);
			if (dstsystem->material == NULL)
			{
				liext_model_particles_clear (self);
				return 0;
			}
			dstsystem->texture = lisys_string_dup (srcsystem->texture);
			if (dstsystem->texture == NULL)
			{
				liext_model_particles_clear (self);
				return 0;
			}
			self->particles.count += srcsystem->particles.count;
			dstsystem->particle_end = self->particles.count;
		}
	}

	/* Create the particles and count frames. */
	if (self->particles.count)
	{
		/* Allocate particle information. */
		self->particles.array = lisys_calloc (self->particles.count, sizeof (LIExtModelParticle));
		if (self->particles.array == NULL)
		{
			liext_model_particles_clear (self);
			return 0;
		}

		/* Copy particle information. */
		for (l = i = 0 ; i < model->particle_systems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particle_systems.array + i;
			dstsystem->buffer_start = self->frames.count;
			for (j = 0 ; j < srcsystem->particles.count ; j++)
			{
				dstparticle = self->particles.array + l++;
				srcparticle = srcsystem->particles.array + j;
				dstparticle->frame_start = srcparticle->frame_start;
				dstparticle->frame_end = srcparticle->frame_end;
				dstparticle->buffer_start = self->frames.count;
				self->frames.count += srcparticle->frames.count;
				dstparticle->buffer_end = self->frames.count;
			}
			dstsystem->buffer_end = self->frames.count;
		}
	}

	/* Create the frames. */
	if (self->frames.count)
	{
		/* Allocate particle information. */
		self->frames.array = lisys_calloc (self->frames.count, sizeof (LIExtModelParticleFrame));
		if (self->frames.array == NULL)
		{
			liext_model_particles_clear (self);
			return 0;
		}

		/* Copy particle frame information. */
		for (l = i = 0 ; i < model->particle_systems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particle_systems.array + i;
			for (j = 0 ; j < srcsystem->particles.count ; j++)
			{
				srcparticle = srcsystem->particles.array + j;
				for (k = 0 ; k < srcparticle->frames.count ; k++)
				{
					srcframe = srcparticle->frames.array + k;
					dstframe = self->frames.array + l++;
					dstframe->coord = srcframe->position;
				}
			}
		}
	}

	return 1;
}

/**
 * \brief Frees the model particle effect evaluator.
 * \param self Model particles.
 */
void liext_model_particles_clear (
	LIExtModelParticles* self)
{
	int i;

	for (i = 0 ; i < self->systems.count ; i++)
	{
		lisys_free (self->systems.array[i].material);
		lisys_free (self->systems.array[i].texture);
	}
	lisys_free (self->particles.array);
	lisys_free (self->frames.array);
	lisys_free (self->systems.array);
	memset (self, 0, sizeof (LIExtModelParticles));
}

/**
 * \brief Evaluates the state of an individual particle.
 * \param self Model particles.
 * \param system Particle system number.
 * \param particle Particle number.
 * \param time Time offset in seconds.
 * \param loop One to enable looping. Zero otherwise.
 * \param position Return location for the position.
 * \param color Return location for the diffuse color.
 * \return One if the particle is alive. Zero otherwise.
 */
int liext_model_particles_evaluate_particle (
	LIExtModelParticles* self,
	int                  system,
	int                  particle,
	float                time,
	int                  loop,
	LIMatVector*         position,
	float*               color)
{
	int index0;
	int index1;
	float frame;
	LIExtModelParticle* p;
	LIExtModelParticleFrame* frame0;
	LIExtModelParticleFrame* frame1;
	LIExtModelParticleSystem* s;

	/* Calculate the offset into the whole particle animation. */
	s = self->systems.array + system;
	p = self->particles.array + s->particle_start + particle;
	lisys_assert (p->frame_start >= 0.0f);
	lisys_assert (p->frame_start < self->frames.count);
	lisys_assert (p->frame_end <= self->frames.count);
	frame = time / TIMESCALE;
	if (loop && frame > s->frame_end_emit)
		frame = fmodf (frame, s->frame_end_emit);

	/* Get the nearest frames. */
	index0 = private_evaluate_frame (self, system, particle, (int) frame, loop);
	if (index0 == -1)
		return 0;
	index1 = private_evaluate_frame (self, system, particle, (int) frame + 1, loop);
	if (index1 == -1)
		return 0;
	frame0 = self->frames.array + p->buffer_start + (index0 - p->frame_start);
	frame1 = self->frames.array + p->buffer_start + (index1 - p->frame_start);

	/* Interpolate between the frames. */
	*position = limat_vector_lerp (frame1->coord, frame0->coord, frame - (int) frame);

	/* Calculate the diffuse color. */
	if (frame < p->frame_start)
	{
		frame += s->frame_end_emit;
		lisys_assert (frame >= 0.0f);
	}
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f - (frame - p->frame_start) / (p->frame_end - p->frame_start);

	return 1;
}

/*****************************************************************************/

static int private_evaluate_frame (
	LIExtModelParticles* self,
	int                  system,
	int                  particle,
	int                  frame,
	int                  loop)
{
	LIExtModelParticle* p;
	LIExtModelParticleSystem* s;

	/* Get the particle. */
	s = self->systems.array + system;
	p = self->particles.array + s->particle_start + particle;

	/* Handle looping. */
	/* If looping is enabled and the frame range of the particle extends
	   beyond the wrap position of the animation, the frames outside of
	   the animation wrap and continue from the beginning. */
	if (loop && frame < p->frame_start)
	{
		if (p->frame_end <= s->frame_end_emit)
			return -1;
		frame += s->frame_end_emit;
	}

	/* Check that the frame exists. */
	if (frame < p->frame_start || frame >= p->frame_end)
		return -1;

	return frame;
}

/** @} */
/** @} */
