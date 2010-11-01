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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenParticles Particles
 * @{
 */

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "render.h"
#include "render-particles.h"

#define TIMESCALE 0.02f

int liren_particles_init (
	LIRenParticles* self,
	LIRenRender*    render,
	LIMdlModel*     model)
{
	int i;
	int j;
	int k;
	int l;
	LIMdlParticle* srcparticle;
	LIMdlParticleFrame* srcframe;
	LIMdlParticleSystem* srcsystem;
	LIRenParticle* dstparticle;
	LIRenParticleFrame* dstframe;
	LIRenParticleSystem* dstsystem;

	memset (self, 0, sizeof (LIRenParticles));

	/* Create particle systems and count particles. */
	if (model->particlesystems.count)
	{
		/* Allocate particle system information. */
		self->systems.count = model->particlesystems.count;
		self->systems.array = lisys_calloc (self->systems.count, sizeof (LIRenParticleSystem));
		if (self->systems.array == NULL)
		{
			liren_particles_clear (self);
			return 0;
		}

		/* Copy particle system information. */
		for (i = 0 ; i < model->particlesystems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particlesystems.array + i;
			dstsystem->particle_start = self->particles.count;
			dstsystem->particle_size = srcsystem->particle_size;
			dstsystem->image = liren_render_find_image (render, srcsystem->texture);
			if (dstsystem->image == NULL)
			{
				liren_render_load_image (render, srcsystem->texture);
				dstsystem->image = liren_render_find_image (render, srcsystem->texture);
			}
			self->particles.count += srcsystem->particles.count;
			dstsystem->particle_end = self->particles.count;
		}
	}

	/* Create particles and count frames. */
	if (self->particles.count)
	{
		/* Allocate particle information. */
		self->particles.array = lisys_calloc (self->particles.count, sizeof (LIRenParticle));
		if (self->particles.array == NULL)
		{
			liren_particles_clear (self);
			return 0;
		}

		/* Copy particle information. */
		for (l = i = 0 ; i < model->particlesystems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particlesystems.array + i;
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

	/* Create frames. */
	if (self->frames.count)
	{
		/* Allocate particle information. */
		self->frames.array = lisys_calloc (self->frames.count, sizeof (LIRenParticleFrame));
		if (self->frames.array == NULL)
		{
			liren_particles_clear (self);
			return 0;
		}

		/* Copy particle frame information. */
		for (l = i = 0 ; i < model->particlesystems.count ; i++)
		{
			dstsystem = self->systems.array + i;
			srcsystem = model->particlesystems.array + i;
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

	/* Calculate bounding box. */
	if (self->frames.count)
	{
		self->bounds.min = self->frames.array[0].coord;
		self->bounds.max = self->frames.array[0].coord;
		for (i = 0 ; i < self->frames.count ; i++)
		{
			dstframe = self->frames.array + i;
			if (self->bounds.min.x < dstframe->coord.x)
				self->bounds.min.x = dstframe->coord.x;
			if (self->bounds.min.y < dstframe->coord.y)
				self->bounds.min.y = dstframe->coord.y;
			if (self->bounds.min.z < dstframe->coord.z)
				self->bounds.min.z = dstframe->coord.z;
			if (self->bounds.max.x < dstframe->coord.x)
				self->bounds.max.x = dstframe->coord.x;
			if (self->bounds.max.y < dstframe->coord.y)
				self->bounds.max.y = dstframe->coord.y;
			if (self->bounds.max.z < dstframe->coord.z)
				self->bounds.max.z = dstframe->coord.z;
		}
	}

	return 1;
}

void liren_particles_clear (
	LIRenParticles* self)
{
	lisys_free (self->particles.array);
	lisys_free (self->frames.array);
	lisys_free (self->systems.array);
	memset (self, 0, sizeof (LIRenParticles));
}

int liren_particles_evaluate_particle (
	LIRenParticles* self,
	int             particle,
	float           time,
	int             loop,
	LIMatVector*    position,
	float*          color)
{
	int last;
	float frame;
	LIRenParticle* p;
	LIRenParticleFrame* frame0;
	LIRenParticleFrame* frame1;

	p = self->particles.array + particle;
	frame = time / TIMESCALE;
	frame -= p->frame_start;
	if (frame < 0.0)
		return 0;
	last = p->frame_end - 1;
	if (frame >= last)
	{
		if (!loop)
			return 0;
		frame -= (int)(frame / last) * last;
		lisys_assert (frame >= 0.0f);
		lisys_assert (frame < last);
	}

	frame0 = self->frames.array + p->buffer_start + (int) frame;
	frame1 = self->frames.array + p->buffer_start + (int) frame + 1;
	*position = limat_vector_lerp (frame1->coord, frame0->coord, frame - (int) frame);
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f - frame / last;
	return 1;
}

void liren_particles_sort (
	LIRenParticles*       self,
	float                 time,
	int                   loop,
	const LIMatTransform* transform,
	LIRenShader*          shader,
	LIRenSort*            sort)
{
	int i;
	int j;
	float color[4];
	LIMatVector coord;
	LIRenParticleSystem* system;

	for (i = 0 ; i < self->systems.count ; i++)
	{
		system = self->systems.array + i;
		if (system->image == NULL)
			continue;
		for (j = system->particle_start ; j < system->particle_end ; j++)
		{
			if (liren_particles_evaluate_particle (self, j, time, loop, &coord, color))
			{
				coord = limat_transform_transform (*transform, coord);
				liren_sort_add_particle (sort, &coord, system->particle_size, color, system->image, shader);
			}
		}
	}
}
