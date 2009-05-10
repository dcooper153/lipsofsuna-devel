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

#include <sector/lips-sector.h>
#include <system/lips-system.h>
#include "render.h"
#include "render-draw.h"
#include "render-scene.h"

#define LIRND_LIGHT_MAXIMUM_RATING 100.0f
#define LIRND_PARTICLE_MAXIMUM_COUNT 500000

static int
private_init_lights (lirndRender* self);

static int
private_init_particles (lirndRender* self);

static int
private_init_resources (lirndRender* self,
                        const char*  dir);

static int
private_init_shaders (lirndRender* self);

static liimgTexture*
private_load_image (lirndRender* self,
                    lirndImage*  texture);

static void
private_particle_remove (lirndRender*   self,
                         lirndParticle* particle);

static void
private_particle_render (lirndRender* self);

static void
private_particle_update (lirndRender* self,
                         float        secs);

static void
private_render (lirndRender*  self,
                lirndCallback call,
                void*         data);

/*****************************************************************************/

lirndRender*
lirnd_render_new (const char* dir)
{
	lirndRender* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lirndRender));
	if (self == NULL)
		return NULL;
	self->shader.enabled = 0;//(livid_features.shader_model >= 3);

	/* Load data. */
	self->config.dir = strdup (dir);
	if (self->config.dir == NULL)
		goto error;
	if (!private_init_shaders (self) ||
	    !private_init_lights (self) ||
	    !private_init_particles (self) ||
	    !private_init_resources (self, dir))
		goto error;

	return self;

error:
	lirnd_render_free (self);
	return NULL;
}

void
lirnd_render_free (lirndRender* self)
{
	lirndConstraint* constraint;
	lirndConstraint* constraint_next;

	/* Free constraints. */
	for (constraint = self->world.constraints ; constraint != NULL ; constraint = constraint_next)
	{
		constraint_next = constraint->next;
		free (constraint);
	}

	/* Free resources. */
	if (self->resources != NULL)
		lirnd_resources_free (self->resources);
	glDeleteTextures (1, &self->helpers.noise);

	/* Free sky. */
	if (self->sky.model != NULL)
		lirnd_object_free (self->sky.model);

	/* Free lights. */
	if (self->lighting != NULL)
		lirnd_lighting_free (self->lighting);

	/* Free particles. */
	free (self->particle.particles);

	/* Free shaders. */
	if (self->shader.fixed != NULL)
		lirnd_shader_free (self->shader.fixed);
	if (self->shader.shadowmap != NULL)
		lirnd_shader_free (self->shader.shadowmap);

	free (self->config.dir);
	free (self);
}

/**
 * \brief Finds a shader by name.
 *
 * If no matching shader is found, a fixed function fallback is returned.
 *
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader.
 */
lirndShader*
lirnd_render_find_shader (lirndRender* self,
                          const char*  name)
{
	lirndShader* shader;

	shader = lirnd_resources_find_shader (self->resources, name);
	if (shader == NULL)
		return self->shader.fixed;

	return shader;
}

/**
 * \brief Finds a texture by name.
 *
 * Searches for a texture from the texture cache and returns the match, if any.
 * If no match is found, the texture is loaded from a file, added to the cache,
 * and returned. NULL is returned if loading the texture fails.
 *
 * \param self Renderer.
 * \param name Name of the texture.
 * \return Texture or NULL.
 */
lirndImage*
lirnd_render_find_image (lirndRender* self,
                         const char*  name)
{
	lirndImage* image;

	image = lirnd_resources_find_image (self->resources, name);
	if (image == NULL)
		image = lirnd_resources_insert_image (self->resources, name);
	if (image == NULL || image->invalid)
		return NULL;
	if (image->texture == NULL)
	{
		image->texture = private_load_image (self, image);
		if (image->texture == NULL)
		{
			image->invalid = 1;
			lisys_error_report ();
		}
	}
	if (image->invalid)
		return NULL;

	return image;
}

/**
 * \brief Forces the engine to reload a texture image.
 *
 * Reloads the requested texture and updates any materials that reference it
 * to point to the new texture. Any other references to the texture become
 * invalid and need to be manually replaced.
 *
 * \param self Renderer.
 * \param name Texture name.
 * \return Nonzero on success.
 */
int
lirnd_render_load_image (lirndRender* self,
                         const char*  name)
{
	liimgTexture* texture;
	lirndImage* image;

	/* Find image info. */
	image = lirnd_resources_find_image (self->resources, name);
	if (image == NULL)
		image = lirnd_resources_insert_image (self->resources, name);
	if (image == NULL)
		return 0;
	if (image->texture == NULL)
		return 1;

	/* Reload image. */
	texture = private_load_image (self, image);
	if (texture == NULL)
		return 0;
	if (image->texture != NULL)
		liimg_texture_free (image->texture);
	image->texture = texture;

	return 1;
}

/**
 * \brief Register a constraint.
 *
 * \param self Renderer.
 * \param constraint Constraint.
 */
void
lirnd_render_insert_constraint (lirndRender*     self,
                                lirndConstraint* constraint)
{
	if (self->world.constraints != NULL)
		self->world.constraints->prev = constraint;
	constraint->next = self->world.constraints;
	self->world.constraints = constraint;
}

/**
 * \brief Creates a new particle.
 *
 * \param self Renderer.
 * \param position Position of the particle.
 * \param velocity Velocity of the particle.
 * \return Particle owned by the renderer or NULL.
 */
lirndParticle*
lirnd_render_insert_particle (lirndRender*       self,
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
 * \param self Renderer.
 * \param scene Scene to pick.
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
lirnd_render_pick (lirndRender*    self,
                   lirndScene*     scene,
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

	self->temporary.scene = scene;
	self->temporary.modelview = modelview;
	self->temporary.projection = projection;
	self->temporary.frustum = frustum;

	glGetIntegerv (GL_VIEWPORT, viewport);
	pick = limat_matrix_pick (x, y, size, size, viewport);
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (pick.m);
	glMultMatrixf (projection->m);
	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (modelview->m);

	/* Pick scene. */
	glSelectBuffer (256, selection);
	glRenderMode (GL_SELECT);
	glInitNames ();
	glPushName (0);
	private_render (self, lirnd_draw_picking, NULL);
	count = glRenderMode (GL_RENDER);

	/* Restore state. */
	self->temporary.scene = NULL;
	self->temporary.modelview = NULL;
	self->temporary.projection = NULL;
	self->temporary.frustum = NULL;
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
 * \brief Unregister a constraint.
 *
 * \param self Renderer.
 * \param constraint Constraint.
 */
void
lirnd_render_remove_constraint (lirndRender*     self,
                                lirndConstraint* constraint)
{
	if (constraint->next != NULL)
		constraint->next->prev = constraint->prev;
	if (constraint->prev != NULL)
		constraint->prev->next = constraint->next;
	else
		self->world.constraints = constraint->next;
	constraint->next = NULL;
	constraint->prev = NULL;
}

/**
 * \brief Renders the scene.
 *
 * \param self Renderer.
 * \param scene Rendered scene.
 * \param modelview Modelview matrix.
 * \param projection Projection matrix.
 * \param frustum Frustum used for culling.
 */
void
lirnd_render_render (lirndRender*  self,
                     lirndScene*   scene,
                     limatMatrix*  modelview,
                     limatMatrix*  projection,
                     limatFrustum* frustum)
{
	int i;

	assert (scene != NULL);
	assert (modelview != NULL);
	assert (projection != NULL);
	assert (frustum != NULL);

	/* Update lights. */
	self->temporary.scene = scene;
	lirnd_lighting_update (self->lighting, scene);

	/* Set default rendering mode. */
	self->temporary.scene = scene;
	self->temporary.modelview = modelview;
	self->temporary.projection = projection;
	self->temporary.frustum = frustum;
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (projection->m);
	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (modelview->m);
	glEnable (GL_LIGHTING);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_CULL_FACE);
	glCullFace (GL_CCW);
	glDepthFunc (GL_LEQUAL);
	glShadeModel (GL_SMOOTH);
	glEnable (GL_BLEND);
	glEnable (GL_NORMALIZE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_2D, 0);

	/* Render scene. */
	glEnable (GL_COLOR_MATERIAL);
	private_render (self, lirnd_draw_opaque, NULL);
	private_render (self, lirnd_draw_transparent, NULL);
	glDisable (GL_COLOR_MATERIAL);
	glDisable (GL_CULL_FACE);
	glDisable (GL_BLEND);
	glDepthMask (GL_TRUE);

	/* Render particles. */
	private_particle_render (self);

	/* Render debug. */
#ifndef NDEBUG
	private_render (self, lirnd_draw_debug, NULL);
#endif

	/* Restore state. */
	if (self->shader.enabled)
		glUseProgramObjectARB (0);
	glMatrixMode (GL_TEXTURE);
	for (i = 7 ; i >= 0 ; i--)
	{
		glActiveTextureARB (GL_TEXTURE0 + i);
		glBindTexture (GL_TEXTURE_2D, 0);
		glDisable (GL_TEXTURE_2D);
	}
	glMatrixMode (GL_MODELVIEW);
	self->temporary.scene = NULL;
	self->temporary.modelview = NULL;
	self->temporary.projection = NULL;
	self->temporary.frustum = NULL;
}

/**
 * \brief Renders the scene.
 *
 * \param self Renderer.
 * \param scene Rendered scene.
 * \param modelview Modelview matrix.
 * \param projection Projection matrix.
 * \param frustum Frustum used for culling.
 * \param call Rendering call.
 * \param data Data passed to rendering call.
 */
void
lirnd_render_render_custom (lirndRender*  self,
                            lirndScene*   scene,
                            limatMatrix*  modelview,
                            limatMatrix*  projection,
                            limatFrustum* frustum,
                            lirndCallback call,
                            void*         data)
{
	assert (scene != NULL);
	assert (modelview != NULL);
	assert (projection != NULL);
	assert (frustum != NULL);
	assert (call != NULL);

	self->temporary.scene = scene;
	self->temporary.modelview = modelview;
	self->temporary.projection = projection;
	self->temporary.frustum = frustum;
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (projection->m);
	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (modelview->m);
	private_render (self, call, data);
	self->temporary.scene = NULL;
	self->temporary.modelview = NULL;
	self->temporary.projection = NULL;
	self->temporary.frustum = NULL;
}

/**
 * \brief Updates the renderer state.
 *
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void
lirnd_render_update (lirndRender* self,
                     float        secs)
{
	lirndConstraint* constraint;

	/* Update particles. */
	private_particle_update (self, secs);

	/* Update constraints. */
	for (constraint = self->world.constraints ;
	     constraint != NULL ;
	     constraint = constraint->next)
	{
		lirnd_constraint_update (constraint, secs);
	}

	/* Update time. */
	self->helpers.time += secs;
}

void
lirnd_render_set_global_shadows (lirndRender* self,
                                 int          value)
{
	self->lighting->config.global_shadows = value;
}

int
lirnd_render_get_light_count (const lirndRender* self)
{
	return self->lighting->lights.capacity;
}

void
lirnd_render_set_light_count (lirndRender* self,
                              int          count)
{
	self->lighting->lights.capacity = LI_MIN (8, count);
}

/**
 * \brief Gets the most relevant light sources arond the provided point.
 *
 * \param self Renderer.
 * \param point Point in world coordinates.
 */
void
lirnd_render_set_light_focus (lirndRender*       self,
                              const limatVector* point)
{
	lirnd_lighting_set_center (self->lighting, point);
}

void
lirnd_render_set_local_shadows (lirndRender* self,
                                int          value)
{
	self->lighting->config.local_shadows = value;
}

int
lirnd_render_get_shaders_enabled (const lirndRender* self)
{
	return self->shader.enabled;
}

void
lirnd_render_set_shaders_enabled (lirndRender* self,
                                  int          value)
{
	if (livid_features.shader_model >= 3)
		self->shader.enabled = value;
	else
		self->shader.enabled = 0;
}

/**
 * \brief Sets the skybox model.
 *
 * \param self Renderer.
 * \param model Model or NULL to unset.
 */
int
lirnd_render_set_sky (lirndRender* self,
                      lirndModel*  model)
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

/**
 * \brief Sets the sun direction.
 *
 * \param self Renderer.
 * \param direction Light direction.
 */
void
lirnd_render_set_sun (lirndRender*       self,
                      const limatVector* direction)
{
	lirnd_lighting_set_sun_direction (self->lighting, direction);
}

/*****************************************************************************/

static int
private_init_lights (lirndRender* self)
{
	self->lighting = lirnd_lighting_new (self);
	if (self->lighting == NULL)
		return 0;
	return 1;
}

static int
private_init_particles (lirndRender* self)
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

static int
private_init_resources (lirndRender* self,
                        const char*  dir)
{
	int x;
	int y;
	unsigned char value;
	unsigned char* pixel;
	unsigned char* pixels;
	static const int perlin_grad[16][3] =
	{
		{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
		{1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
		{1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0},
		{1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}
	};
	static const int perlin_perm[256] =
	{
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
		142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,
		219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,
		68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,
		133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161,1,216,
		80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,
		100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,
		255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
		119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,
		19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,
		193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,
		214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,
		236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	/* Initialize resource list. */
	self->resources = lirnd_resources_new (self);
	if (self->resources == NULL)
		return 0;

	/* Initialize noise texture. */
	pixels = malloc (4 * 256 * 256);
	pixel = pixels;
	if (pixels == NULL)
		return 0;
	for (y = 0 ; y < 256 ; y++)
	{
		for (x = 0 ; x < 256 ; x++)
		{
			value = perlin_perm[(x + perlin_perm[y]) & 0xFF];
			*(pixel++) = perlin_grad[value & 0x0F][0] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][1] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][2] * 64 + 64;
			*(pixel++) = value;
		}
	}
	glGenTextures (1, &self->helpers.noise);
	glBindTexture (GL_TEXTURE_2D, self->helpers.noise);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	free (pixels);

	return 1;
}

static int
private_init_shaders (lirndRender* self)
{
	char* path;
	liReader* reader;

	self->shader.fixed = lirnd_shader_new (self);
	if (self->shader.fixed == NULL)
		return 0;
	if (livid_features.shader_model >= 3)
	{
		path = lisys_path_concat (self->config.dir, "shaders", "shadowmap", NULL);
		if (path == NULL)
			return 0;
		reader = li_reader_new_from_file (path);
		free (path);
		if (reader == NULL)
			return 0;
		self->shader.shadowmap = lirnd_shader_new_from_data (self, reader);
		li_reader_free (reader);
		if (self->shader.shadowmap == NULL)
			return 0;
	}

	return 1;
}

static liimgTexture*
private_load_image (lirndRender* self,
                      lirndImage*  image)
{
	char* path;
	liimgTexture* result;

	path = lisys_path_format (self->config.dir,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, image->name, ".dds", NULL);
	if (path == NULL)
		return NULL;
	result = liimg_texture_new_from_file (path);
	free (path);

	return result;
}

static void
private_particle_remove (lirndRender*   self,
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
private_particle_render (lirndRender* self)
{
	float color[4];
    float attenuation[] = { 1.0f, 0.0f, 0.02f };
	lirndImage* image;
	lirndParticle* particle;

	/* Set particle graphics. */
	image = lirnd_render_find_image (self, "particle-000");
	if (image != NULL)
		glBindTexture (GL_TEXTURE_2D, image->texture->texture);
	if (livid_features.shader_model >= 3)
		glUseProgramObjectARB (0);
	glColor3f (1.0f, 1.0f, 1.0f);
	glEnable (GL_BLEND);

	/* Set particle rendering mode. */
	glBlendFunc (GL_ONE, GL_ONE);
	glDisable (GL_LIGHTING);
	if (livid_features.ARB_point_sprite)
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
	if (livid_features.ARB_point_sprite)
	{
		glDisable (GL_POINT_SPRITE_ARB);
	}
	glEnable (GL_LIGHTING);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void
private_particle_update (lirndRender* self,
                         float        secs)
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
private_render (lirndRender*  self,
                lirndCallback call,
                void*         data)
{
	lirndObject* object;
	lirndSceneIter iter;

	LIRND_FOREACH_SCENE (iter, self->temporary.scene)
	{
		object = iter.value;
		if (object->model == NULL)
			continue;
		call (data, self, object);
	}
}

/** @} */
/** @} */
