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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndRender Render
 * @{
 */

#include <system/lips-system.h>
#include "render.h"
#include "render-draw.h"

#define LIRND_LIGHT_MAXIMUM_RATING 100.0f
#define LIRND_PARTICLE_MAXIMUM_COUNT 500000

static int
private_init_lights (lirndScene* self);

static int
private_init_particles (lirndScene* self);

static void
private_particle_remove (lirndScene*    self,
                         lirndParticle* particle);

static void
private_particle_render (lirndScene* self);

static void
private_particle_update (lirndScene* self,
                         float       secs);

static void
private_render (lirndScene*   self,
                lirndContext* context,
                lirndCallback call,
                void*         data);

/*****************************************************************************/

lirndScene*
lirnd_scene_new (lirndRender* render)
{
	lirndScene* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lirndScene));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Allocate object list. */
	self->objects = lialg_ptrdic_new ();
	if (self->objects == NULL)
		goto error;

	/* Initialize subsystems. */
	if (!private_init_lights (self) ||
	    !private_init_particles (self))
		goto error;

	return self;

error:
	lirnd_scene_free (self);
	return NULL;
}

void
lirnd_scene_free (lirndScene* self)
{
	/* Free sky. */
	if (self->sky.model != NULL)
		lirnd_object_free (self->sky.model);

	/* Free lights. */
	if (self->lighting != NULL)
		lirnd_lighting_free (self->lighting);

	/* Free particles. */
	free (self->particle.particles);

	/* Free objects. */
	if (self->objects != NULL)
	{
		assert (self->objects->size == 0);
		lialg_ptrdic_free (self->objects);
	}

	free (self);
}

/**
 * \brief Creates a new particle.
 *
 * \param self Scene.
 * \param position Position of the particle.
 * \param velocity Velocity of the particle.
 * \return Particle owned by the scene or NULL.
 */
lirndParticle*
lirnd_scene_insert_particle (lirndScene*        self,
                             const limatVector* position,
                             const limatVector* velocity)
{
	lirndParticle* particle;

	/* Get free particle. */
	particle = self->particle.particles_free;
	if (particle == NULL)
		return NULL;
	lirnd_particle_init (particle, position, velocity);

	/* Remove from free list. */
	self->particle.particles_free = particle->next;
	if (particle->next != NULL)
		particle->next->prev = NULL;

	/* Add to used list. */
	if (self->particle.particles_used != NULL)
		self->particle.particles_used->prev = particle;
	particle->next = self->particle.particles_used;
	self->particle.particles_used = particle;

	return particle;
}

/**
 * \brief Picks an object from the scene.
 *
 * \param self Scene.
 * \param modelview Modelview matrix.
 * \param projection Projection matrix.
 * \param frustum Frustum.
 * \param x Pick rectangle center.
 * \param y Pick rectangle center.
 * \param size Pick rectangle size.
 * \param result Return location for pick result.
 * \return Nonzero if something was picked.
 */
int
lirnd_scene_pick (lirndScene*     self,
                  limatMatrix*    modelview,
                  limatMatrix*    projection,
                  limatFrustum*   frustum,
                  int             x,
                  int             y,
                  int             size,
                  lirndSelection* result)
{
	int i;
	int count;
	GLuint id;
	GLuint dist;
	GLuint selection[256];
	GLint viewport[4];
	limatMatrix pick;
	lirndContext context;

	glGetIntegerv (GL_VIEWPORT, viewport);
	pick = limat_matrix_pick (x, y, size, size, viewport);
	pick = limat_matrix_multiply (pick, *projection);
	lirnd_context_init (&context, self);
	lirnd_context_set_modelview (&context, modelview);
	lirnd_context_set_projection (&context, &pick);
	lirnd_context_set_frustum (&context, frustum);

	/* Pick scene. */
	glSelectBuffer (256, selection);
	glRenderMode (GL_SELECT);
	glInitNames ();
	glPushName (0);
	private_render (self, &context, lirnd_draw_picking, NULL);
	count = glRenderMode (GL_RENDER);
	if (count <= 0)
		return 0;

	/* Analyze results. */
	id = selection[3];
	dist = selection[1];
	for (i = 1 ; i < count ; i++)
	{
		if (selection[4 * i + 1] < dist)
		{
			id = selection[4 * i + 3];
			dist = selection[4 * i + 1];
		}
	}

	/* Find hit. */
	if (id != 0)
	{
		result->type = LIRND_SELECT_OBJECT;
		result->depth = (float) dist / 0xFFFFFFFF;
		result->object = id;
	}
	else
	{
		result->type = LIRND_SELECT_SECTOR;
		result->depth = (float) dist / 0xFFFFFFFF;
		result->object = 0;
	}
 
	return 1;
}

/**
 * \brief Renders the scene.
 *
 * \param self Scene.
 * \param modelview Modelview matrix.
 * \param projection Projection matrix.
 * \param frustum Frustum used for culling.
 */
void
lirnd_scene_render (lirndScene*   self,
                    limatMatrix*  modelview,
                    limatMatrix*  projection,
                    limatFrustum* frustum)
{
	int i;
	lirndContext context;
	GLfloat none[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	assert (modelview != NULL);
	assert (projection != NULL);
	assert (frustum != NULL);

	lirnd_context_init (&context, self);
	lirnd_context_set_modelview (&context, modelview);
	lirnd_context_set_projection (&context, projection);
	lirnd_context_set_frustum (&context, frustum);

	/* Set default rendering mode. */
	glEnable (GL_LIGHTING);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_CULL_FACE);
	glFrontFace (GL_CCW);
	glDepthMask (GL_TRUE);
	glDepthFunc (GL_LEQUAL);
	glShadeModel (GL_SMOOTH);
	glEnable (GL_BLEND);
	glEnable (GL_NORMALIZE);
	glEnable (GL_COLOR_MATERIAL);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_2D, 0);
	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, none);
	glLightModelfv (GL_LIGHT_MODEL_AMBIENT, none);

	/* Render scene. */
#ifdef LIRND_ENABLE_PROFILING
	self->render->profiling.objects = 0;
	self->render->profiling.materials = 0;
	self->render->profiling.faces = 0;
	self->render->profiling.vertices = 0;
#endif
	private_render (self, &context, lirnd_draw_opaque, NULL);
	private_render (self, &context, lirnd_draw_hair, NULL);
	private_render (self, &context, lirnd_draw_transparent, NULL);
#ifdef LIRND_ENABLE_PROFILING
	printf ("RENDER PROFILING: objects=%d materials=%d polys=%d verts=%d\n",
		self->render->profiling.objects, self->render->profiling.materials,
		self->render->profiling.faces, self->render->profiling.vertices);
#endif

	/* Render particles. */
	glDisable (GL_COLOR_MATERIAL);
	glDisable (GL_CULL_FACE);
	glDisable (GL_BLEND);
	glDepthMask (GL_FALSE);
	private_particle_render (self);
	glDepthMask (GL_TRUE);

	/* Restore state. */
	if (self->render->shader.enabled)
		glUseProgramObjectARB (0);
	glMatrixMode (GL_TEXTURE);
	for (i = 7 ; i >= 0 ; i--)
	{
		glActiveTextureARB (GL_TEXTURE0 + i);
		glBindTexture (GL_TEXTURE_2D, 0);
		glDisable (GL_TEXTURE_2D);
	}
	glMatrixMode (GL_MODELVIEW);
}

/**
 * \brief Updates the scene.
 *
 * \param self Scene.
 * \param secs Number of seconds since the last update.
 */
void
lirnd_scene_update (lirndScene* self,
                    float       secs)
{
	lialgPtrdicIter iter;
	lirndObject* object;

	/* Update objects. */
	LI_FOREACH_PTRDIC (iter, self->objects)
	{
		object = iter.value;
		lirnd_object_update (object, secs);
	}

	/* Update lights. */
	lirnd_lighting_update (self->lighting);

	/* Update particles. */
	private_particle_update (self, secs);
}

/**
 * \brief Enables light sources that affect the provided point the most.
 *
 * \param self Scene.
 * \param point Point in world coordinates.
 */
void
lirnd_scene_set_light_focus (lirndScene*        self,
                             const limatVector* point)
{
	lirnd_lighting_set_center (self->lighting, point);
}

/**
 * \brief Sets the skybox model.
 *
 * \param self Scene.
 * \param model Model or NULL to unset.
 */
int
lirnd_scene_set_sky (lirndScene* self,
                     lirndModel* model)
{
#warning Skybox is disabled.
#if 0
	lirndObject* inst;

	if (model != NULL)
	{
		inst = lirnd_object_new (model, 0);
		if (inst == NULL)
			return 0;
		if (self->sky.model != NULL)
			lirnd_object_free (self->sky.model);
		self->sky.model = inst;
	}
	else
	{
		if (self->sky.model != NULL)
			lirnd_object_free (self->sky.model);
		self->sky.model = NULL;
	}
#endif
	return 1;
}

/*****************************************************************************/

static int
private_init_lights (lirndScene* self)
{
	self->lighting = lirnd_lighting_new (self->render);
	if (self->lighting == NULL)
		return 0;
	return 1;
}

static int
private_init_particles (lirndScene* self)
{
	int i;
	const int count = LIRND_PARTICLE_MAXIMUM_COUNT;

	/* Allocate particles. */
	self->particle.particles = calloc (count, sizeof (lirndParticle));
	if (self->particle.particles == NULL)
		return 0;

	/* Add to free list. */
	self->particle.count_free = count;
	self->particle.particles_free = self->particle.particles;
	for (i = 0 ; i < count ; i++)
	{
		self->particle.particles[i].prev = self->particle.particles + i - 1;
		self->particle.particles[i].next = self->particle.particles + i + 1;
	}
	self->particle.particles[0].prev = NULL;
	self->particle.particles[count - 1].next = NULL;

	return 1;
}

static void
private_particle_remove (lirndScene*    self,
                         lirndParticle* particle)
{
	/* Remove from used list. */
	if (particle->prev != NULL)
		particle->prev->next = particle->next;
	else
		self->particle.particles_used = particle->next;
	if (particle->next != NULL)
		particle->next->prev = particle->prev;

	/* Add to free list. */
	if (self->particle.particles_free != NULL)
		self->particle.particles_free->prev = particle;
	particle->next = self->particle.particles_free;
	self->particle.particles_free = particle;
}

static void
private_particle_render (lirndScene* self)
{
	float color[4];
    float attenuation[] = { 1.0f, 0.0f, 0.02f };
	lirndImage* image;
	lirndParticle* particle;

	/* Set particle graphics. */
	image = lirnd_render_find_image (self->render, "particle-000");
	if (image != NULL)
		glBindTexture (GL_TEXTURE_2D, image->texture->texture);
	if (livid_features.shader_model >= 3)
		glUseProgramObjectARB (0);
	glColor3f (1.0f, 1.0f, 1.0f);
	glEnable (GL_BLEND);

	/* Set particle rendering mode. */
	glBlendFunc (GL_ONE, GL_ONE);
	glDisable (GL_LIGHTING);
	if (GLEW_ARB_point_sprite)
	{
		glEnable (GL_POINT_SPRITE_ARB);
		glTexEnvi (GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
		glPointParameterfvARB (GL_POINT_DISTANCE_ATTENUATION_ARB, attenuation);
		glPointSize (15.0f);
	}
	else
		glPointSize (4.0f);

	/* Render particles. */
	glBegin (GL_POINTS);
	for (particle = self->particle.particles_used ; particle != NULL ; particle = particle->next)
	{
		lirnd_particle_get_color (particle, color);
		glColor4fv (color);
		glVertex3f (
			particle->position.x,
			particle->position.y,
			particle->position.z);
	}
	glEnd ();

	/* Set normal rendering mode. */
	if (GLEW_ARB_point_sprite)
	{
		glDisable (GL_POINT_SPRITE_ARB);
	}
	glEnable (GL_LIGHTING);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void
private_particle_update (lirndScene* self,
                         float       secs)
{
	lirndParticle* next;
	lirndParticle* particle;

	for (particle = self->particle.particles_used ; particle != NULL ; particle = next)
	{
		next = particle->next;

		/* FIXME: Inaccurate. */
		particle->velocity = limat_vector_add (particle->velocity,
			limat_vector_multiply (particle->acceleration, secs * secs));
		particle->position = limat_vector_add (particle->position,
			limat_vector_multiply (particle->velocity, secs));
		particle->time += secs;

		if (particle->time > particle->time_life)
			private_particle_remove (self, particle);
	}
}

static void
private_render (lirndScene*   self,
                lirndContext* context,
                lirndCallback call,
                void*         data)
{
	lialgPtrdicIter iter;
	lirndObject* object;

	LI_FOREACH_PTRDIC (iter, self->objects)
	{
		object = iter.value;
		call (context, object, data);
	}
}

/** @} */
/** @} */
