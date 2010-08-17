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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenRender Render
 * @{
 */

#include <lipsofsuna/system.h>
#include "render.h"
#include "render-draw.h"
#include "render-group.h"

#define LIREN_LIGHT_MAXIMUM_RATING 100.0f
#define LIREN_PARTICLE_MAXIMUM_COUNT 1000

static int
private_init (LIRenScene* self);

static int
private_light_bounds (LIRenScene*   self,
                      LIRenLight*   light,
                      LIRenContext* context,
                      int*          viewport,
                      float*        result);

static void
private_lighting_render (LIRenScene*    self,
                         LIRenContext*  context,
                         LIRenDeferred* framebuffer);

static void
private_particle_render (LIRenScene* self);

static void
private_render (LIRenScene*   self,
                LIRenContext* context,
                lirenCallback call,
                void*         data);

static int
private_sort_objects (const void* first,
                      const void* second);

static int
private_sort_scene (LIRenScene*   self,
                    LIRenContext* context,
                    LIRenObject** result_list,
                    int*          result_count);

/*****************************************************************************/

LIRenScene*
liren_scene_new (LIRenRender* render)
{
	LIRenScene* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenScene));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Register self. */
	if (!lialg_ptrdic_insert (render->scenes, self, self))
		goto error;

	/* Allocate object list. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
		goto error;
	self->groups = lialg_ptrdic_new ();
	if (self->groups == NULL)
		goto error;

	/* Initialize subsystems. */
	if (!private_init (self))
		goto error;

	return self;

error:
	liren_scene_free (self);
	return NULL;
}

void
liren_scene_free (LIRenScene* self)
{
	/* Free sky. */
	if (self->sky.model != NULL)
		liren_object_free (self->sky.model);

	/* Free lights. */
	if (self->lighting != NULL)
		liren_lighting_free (self->lighting);

	/* Free particles. */
	if (self->particles != NULL)
		lipar_manager_free (self->particles);

	/* Free objects. */
	if (self->groups != NULL)
	{
		lisys_assert (self->groups->size == 0);
		lialg_ptrdic_free (self->groups);
	}
	if (self->objects != NULL)
	{
		lisys_assert (self->objects->size == 0);
		lialg_u32dic_free (self->objects);
	}

	/* Unregister self. */
	lialg_ptrdic_remove (self->render->scenes, self);

	lisys_free (self);
}

LIRenObject*
liren_scene_find_object (LIRenScene* self,
                         int         id)
{
	return lialg_u32dic_find (self->objects, id);
}

/**
 * \brief Creates a new particle.
 *
 * \param self Scene.
 * \param texture Texture of the particle.
 * \param position Position of the particle.
 * \param velocity Velocity of the particle.
 * \return Particle owned by the scene or NULL.
 */
LIParPoint*
liren_scene_insert_particle (LIRenScene*        self,
                             const char*        texture,
                             const LIMatVector* position,
                             const LIMatVector* velocity)
{
	return lipar_manager_insert_point (self->particles, texture, position, velocity);
}

/**
 * \brief Picks an object from the scene.
 *
 * \param self Scene.
 * \param modelview Modelview matrix.
 * \param projection Projection matrix.
 * \param frustum Frustum.
 * \param viewport Viewport.
 * \param x Pick rectangle center.
 * \param y Pick rectangle center.
 * \param size Pick rectangle size.
 * \param result Return location for pick result.
 * \return Nonzero if something was picked.
 */
int
liren_scene_pick (LIRenScene*     self,
                  LIMatMatrix*    modelview,
                  LIMatMatrix*    projection,
                  LIMatFrustum*   frustum,
                  const int*      viewport,
                  int             x,
                  int             y,
                  int             size,
                  LIRenSelection* result)
{
	int i;
	int count;
	GLuint id;
	GLuint dist;
	GLuint selection[256];
	LIMatMatrix pick;
	LIMatVector window;
	LIRenContext* context;

	pick = limat_matrix_pick (x, y, size, size, viewport);
	pick = limat_matrix_multiply (pick, *projection);
	context = liren_render_get_context (self->render);
	liren_context_set_scene (context, self);
	liren_context_set_modelview (context, modelview);
	liren_context_set_projection (context, &pick);
	liren_context_set_frustum (context, frustum);

	/* Pick scene. */
	glSelectBuffer (256, selection);
	glRenderMode (GL_SELECT);
	glInitNames ();
	glPushName (0);
	private_render (self, context, liren_draw_picking, NULL);
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
		result->type = LIREN_SELECT_OBJECT;
		result->depth = dist / 4294967295.0f;
		result->object = id;
		window = limat_vector_init (x, y, result->depth);
		result->point = limat_vector_init (0.0f, 0.0f, 0.0f);
		limat_matrix_unproject (*projection, *modelview, viewport, &window, &result->point);
	}
	else
	{
		result->type = LIREN_SELECT_SECTOR;
		result->depth = dist / 4294967295.0f;
		result->object = 0;
		window = limat_vector_init (x, y, result->depth);
		result->point = limat_vector_init (0.0f, 0.0f, 0.0f);
		limat_matrix_unproject (*projection, *modelview, viewport, &window, &result->point);
	}
 
	return 1;
}

/**
 * \brief Initializes rendering.
 *
 * Stores the passed parameters for future use and formats the rendering
 * state. Sets the rendering bits so that other rendering calls know they
 * have a valid render state.
 *
 * \param self Scene.
 * \param framebuffer Deferred and post-processing framebuffers.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projection matrix of the camera.
 * \param frustum Frustum to use for frustum culling.
 * \return Nonzero on success.
 */
int
liren_scene_render_begin (LIRenScene*    self,
                          LIRenDeferred* framebuffer,
                          LIMatMatrix*   modelview,
                          LIMatMatrix*   projection,
                          LIMatFrustum*  frustum)
{
	int count;
	LIRenContext* context;
	LIRenObject* objects = NULL;

	lisys_assert (modelview != NULL);
	lisys_assert (projection != NULL);
	lisys_assert (frustum != NULL);
	if (framebuffer == NULL)
		return 0;
	liren_check_errors ();

	/* Initialize context. */
	context = liren_render_get_context (self->render);
	liren_context_set_scene (context, self);
	liren_context_set_modelview (context, modelview);
	liren_context_set_projection (context, projection);
	liren_context_set_frustum (context, frustum);

	/* Depth sort scene. */
	if (!private_sort_scene (self, context, &objects, &count))
		return 0;

	/* Change render state. */
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glFrontFace (GL_CCW);
	glDepthMask (GL_TRUE);
	glDepthFunc (GL_LEQUAL);
	glEnable (GL_NORMALIZE);
	glEnable (GL_COLOR_MATERIAL);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Reset profiling. */
#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.objects = 0;
	self->render->profiling.materials = 0;
	self->render->profiling.faces = 0;
	self->render->profiling.vertices = 0;
#endif

	/* Update state. */
	self->state.rendering = 1;
	self->state.context = context;
	self->state.framebuffer = framebuffer;
	self->state.objects = objects;
	self->state.objectn = count;

	/* Enable backbuffer viewport. */
	glPushAttrib (GL_VIEWPORT_BIT);
	glViewport (0, 0, framebuffer->width, framebuffer->height);

	/* Clear the post-processing buffer. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, framebuffer->postproc_fbo[0]);
	glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	return 1;
}

/**
 * \brief Finishes rendering.
 *
 * Draws the contents of the post-processing buffer to the framebuffer and
 * resets rendering state and rendering bits.
 *
 * \param self Scene.
 */
void
liren_scene_render_end (LIRenScene* self)
{
	int i;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Disable backbuffer viewport. */
	glPopAttrib ();

	/* Setup copy to screen. */
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();
	glUseProgramObjectARB (0);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glBindTexture (GL_TEXTURE_2D, self->state.framebuffer->postproc_texture[0]);
	glColor3f (1.0f, 1.0f, 1.0f);

	/* Copy results to screen. */
	glBegin (GL_QUADS);
	glTexCoord2i (0, 0);
	glVertex2i (-1, -1);
	glTexCoord2i (0, 1);
	glVertex2i (-1, 1);
	glTexCoord2i (1, 1);
	glVertex2i (1, 1);
	glTexCoord2i (1, 0);
	glVertex2i (1, -1);
	glEnd ();

	/* Disable copy to screen. */
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
	liren_check_errors ();

	/* Update state. */
	lisys_free (self->state.objects);
	memset (&self->state, 0, sizeof (self->state));

	/* Change render state. */
	glUseProgramObjectARB (0);
	glMatrixMode (GL_TEXTURE);
	for (i = 7 ; i >= 0 ; i--)
	{
		glActiveTextureARB (GL_TEXTURE0 + i);
		glBindTexture (GL_TEXTURE_2D, 0);
		glDisable (GL_TEXTURE_2D);
	}
	glMatrixMode (GL_MODELVIEW);
	liren_check_errors ();
	self->state.rendering = 0;

	/* Profiling report. */
#ifdef LIREN_ENABLE_PROFILING
	printf ("RENDER PROFILING: objects=%d materials=%d polys=%d verts=%d\n",
		self->render->profiling.objects, self->render->profiling.materials,
		self->render->profiling.faces, self->render->profiling.vertices);
#endif
}

/**
 * \brief Begins deferred rendering.
 *
 * \param self Scene.
 * \param alphatest Nonzero for drawing transparent faces using alpha test.
 * \param threshold Alpha test threshold.
 */
void liren_scene_render_deferred_begin (
	LIRenScene* self,
	int         alphatest,
	float       threshold)
{
	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Change render state. */
	liren_context_set_deferred (self->state.context, 1);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->state.framebuffer->deferred_fbo);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_ALPHA_TEST);
	glDisable (GL_BLEND);
	glAlphaFunc (GL_GEQUAL, threshold);
	self->state.alphatest = alphatest;
}

/**
 * \brief Renders lit fragments to the post-processing buffer.
 *
 * Using the data in the G-buffer and the lights of the scene, renders lit
 * fragments to the post-processing buffer.
 *
 * \param self Scene.
 */
void liren_scene_render_deferred_end (
	LIRenScene* self)
{
	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Change render state. */
	liren_check_errors ();
	glDisable (GL_ALPHA_TEST);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[0]);
	glPushAttrib (GL_SCISSOR_BIT);

	/* Render lit fragments to post-processing buffer. */
	private_lighting_render (self, self->state.context, self->state.framebuffer);
	liren_check_errors ();

	/* Change render state. */
	glPopAttrib ();
	liren_context_set_deferred (self->state.context, 0);
}

/**
 * \brief Renders opaque objects to the post-processing buffer.
 *
 * Draws the visible opaque objects, and optionally transparent objects
 * using alpha test, to the G-buffer.
 *
 * \param self Scene.
 */
void liren_scene_render_deferred_opaque (
	LIRenScene* self)
{
	int i;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Render opaque faces to G-buffer. */
	if (self->state.alphatest)
	{
		for (i = 0 ; i < self->state.objectn ; i++)
			liren_draw_all (self->state.context, self->state.objects + i, NULL);
		liren_check_errors ();
	}
	else
	{
		for (i = 0 ; i < self->state.objectn ; i++)
			liren_draw_opaque (self->state.context, self->state.objects + i, NULL);
		liren_check_errors ();
	}
}

/**
 * \brief Renders opaque objects to the post-processing buffer.
 *
 * Draws the visible opaque objects, and optionally transparent objects
 * using alpha test, to the post-processing input buffer.
 *
 * \param self Scene.
 */
void
liren_scene_render_forward_opaque (LIRenScene* self,
                                   int         alpha,
                                   float       threshold)
{
	int i;
	LIAlgPtrdicIter iter;
	LIMatAabb aabb0;
	LIMatAabb aabb1;
	LIRenLight* light;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Change render state. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[0]);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_ALPHA_TEST);
	glDisable (GL_BLEND);
	glAlphaFunc (GL_GEQUAL, threshold);

	/* Render opaque faces to post-processing buffer. */
	for (i = 0 ; i < self->state.objectn ; i++)
	{
		liren_object_get_bounds (self->state.objects + i, &aabb1);
		/* FIXME: Looping through all lights and objects is slow. */
		LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
		{
			light = iter.value;
			if (light->enabled)
			{
				liren_light_get_bounds (light, &aabb0);
				if (limat_aabb_intersects_aabb (&aabb0, &aabb1))
				{
					liren_context_set_lights (self->state.context, &light, 1);
					if (alpha)
						liren_draw_all (self->state.context, self->state.objects + i, NULL);
					else
						liren_draw_opaque (self->state.context, self->state.objects + i, NULL);
				}
			}
		}
	}

	/* Change render state. */
	glDisable (GL_ALPHA_TEST);
	liren_check_errors ();
}

/**
 * \brief Renders transparent objects to the post-processing buffer.
 *
 * Draws the depth sorted transparent objects to the post-processing input buffer.
 * Also draws particles to the post-processing input buffer. Depth writing is
 * disabled during this pass and, for now, no depth sorting of polygons is done.
 *
 * \param self Scene.
 */
void
liren_scene_render_forward_transparent (LIRenScene* self)
{
	int i;
	LIAlgPtrdicIter iter;
	LIMatAabb aabb0;
	LIMatAabb aabb1;
	LIRenLight* light;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Change render state. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[0]);
	glEnable (GL_COLOR_MATERIAL);
	glEnable (GL_BLEND);
	glEnable (GL_CULL_FACE);
	glEnable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Render transparent faces to post-processing buffer. */
	for (i = self->state.objectn - 1 ; i >= 0 ; i--)
	{
		liren_object_get_bounds (self->state.objects + i, &aabb1);
		/* FIXME: Looping through all lights and objects is slow. */
		LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
		{
			light = iter.value;
			if (light->enabled)
			{
				liren_light_get_bounds (light, &aabb0);
				if (limat_aabb_intersects_aabb (&aabb0, &aabb1))
				{
					liren_context_set_lights (self->state.context, &light, 1);
					liren_draw_transparent (self->state.context, self->state.objects + i, NULL);
				}
			}
		}
	}

	/* Change render state. */
	liren_context_set_lights (self->state.context, NULL, 0);
	liren_check_errors ();
	glDisable (GL_COLOR_MATERIAL);
	glDisable (GL_CULL_FACE);

	/* Render particles. */
	private_particle_render (self);

	/* Change render state. */
	glDepthMask (GL_TRUE);
	liren_check_errors ();
}

/**
 * \brief Performs post-processing for the post-processing buffer.
 *
 * Executes zero or more post-processing passes. Each pass takes the post-processing
 * buffer as an input, renders to a temporary buffer, and makes the temporary buffer
 * the input of the next operation.
 *
 * \param self Scene.
 */
void
liren_scene_render_postproc (LIRenScene* self)
{
	int i;
	int src;
	char name[32];
	GLuint tmp;
	LIRenShader* shader;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Change render state. */
	glDisable (GL_BLEND);
	glDisable (GL_CULL_FACE);
	glDisable (GL_DEPTH_TEST);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();

	/* Call post-processing shaders. */
	for (i = src = 0 ; i < 1024 ; i++, src = !src)
	{
		/* Bind post-processing shader. */
		snprintf (name, 32, "postprocess%d", i);
		shader = liren_render_find_shader (self->render, name);
		if (shader == NULL)
		{
			src = !src;
			break;
		}

		/* Change render state. */
		glUseProgramObjectARB (shader->program);
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[1]);
		glBindTexture (GL_TEXTURE_2D, self->state.framebuffer->postproc_texture[0]);

		/* Render intermediate framebuffer to screen. */
		glBegin (GL_QUADS);
		glTexCoord2i (0, 0);
		glVertex2i (-1, -1);
		glTexCoord2i (0, 1);
		glVertex2i (-1, 1);
		glTexCoord2i (1, 1);
		glVertex2i (1, 1);
		glTexCoord2i (1, 0);
		glVertex2i (1, -1);
		glEnd ();
		liren_check_errors ();

		/* Swap input and output. */
		tmp = self->state.framebuffer->postproc_fbo[0];
		self->state.framebuffer->postproc_fbo[0] = self->state.framebuffer->postproc_fbo[1];
		self->state.framebuffer->postproc_fbo[1] = tmp;
		tmp = self->state.framebuffer->postproc_texture[0];
		self->state.framebuffer->postproc_texture[0] = self->state.framebuffer->postproc_texture[1];
		self->state.framebuffer->postproc_texture[1] = tmp;
	}

	/* Change render state. */
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

/**
 * \brief Updates the scene.
 *
 * \param self Scene.
 * \param secs Number of seconds since the last update.
 */
void
liren_scene_update (LIRenScene* self,
                    float       secs)
{
	LIAlgU32dicIter iter;
	LIRenObject* object;

	/* Update objects. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = iter.value;
		liren_object_update (object, secs);
	}

	/* Update lights. */
	liren_lighting_update (self->lighting);

	/* Update particles. */
	lipar_manager_update (self->particles, secs);
}

/**
 * \brief Sets the skybox model.
 *
 * \param self Scene.
 * \param model Model or NULL to unset.
 */
int
liren_scene_set_sky (LIRenScene* self,
                     LIRenModel* model)
{
#warning Skybox is disabled.
#if 0
	LIRenObject* inst;

	if (model != NULL)
	{
		inst = liren_object_new (model, 0);
		if (inst == NULL)
			return 0;
		if (self->sky.model != NULL)
			liren_object_free (self->sky.model);
		self->sky.model = inst;
	}
	else
	{
		if (self->sky.model != NULL)
			liren_object_free (self->sky.model);
		self->sky.model = NULL;
	}
#endif
	return 1;
}

/*****************************************************************************/

static int
private_init (LIRenScene* self)
{
	self->lighting = liren_lighting_new (self->render);
	if (self->lighting == NULL)
		return 0;
	self->particles = lipar_manager_new (LIREN_PARTICLE_MAXIMUM_COUNT);
	if (self->particles == NULL)
		return 0;
	return 1;
}

static int
private_light_bounds (LIRenScene*   self,
                      LIRenLight*   light,
                      LIRenContext* context,
                      int*          viewport,
                      float*        result)
{
	int i;
	LIMatAabb bounds;
	LIMatVector min;
	LIMatVector max;
	LIMatVector tmp;
	LIMatVector p[8];

	if (!liren_light_get_bounds (light, &bounds))
		return 0;
	p[0] = limat_vector_init (bounds.min.x, bounds.min.y, bounds.min.z);
	p[1] = limat_vector_init (bounds.min.x, bounds.min.y, bounds.max.z);
	p[2] = limat_vector_init (bounds.min.x, bounds.max.y, bounds.min.z);
	p[3] = limat_vector_init (bounds.min.x, bounds.max.y, bounds.max.z);
	p[4] = limat_vector_init (bounds.max.x, bounds.min.y, bounds.min.z);
	p[5] = limat_vector_init (bounds.max.x, bounds.min.y, bounds.max.z);
	p[6] = limat_vector_init (bounds.max.x, bounds.max.y, bounds.min.z);
	p[7] = limat_vector_init (bounds.max.x, bounds.max.y, bounds.max.z);
	tmp = limat_vector_init (0.0f, 0.0f, 0.0f);
	limat_matrix_project (context->projection, context->modelview, viewport, p, &tmp);
	min = max = tmp;
	for (i = 1 ; i < 8 ; i++)
	{
		tmp = limat_vector_init (0.0f, 0.0f, 0.0f);
		limat_matrix_project (context->projection, context->modelview, viewport, p + i, &tmp);
		if (tmp.x < min.x) min.x = tmp.x;
		if (tmp.y < min.y) min.y = tmp.y;
		if (tmp.z < min.z) min.z = tmp.z;
		if (tmp.x > max.x) max.x = tmp.x;
		if (tmp.y > max.y) max.y = tmp.y;
		if (tmp.z > max.z) max.z = tmp.z;
	}
	result[0] = min.x;
	result[1] = min.y;
	result[2] = max.x - min.x;
	result[3] = max.y - min.y;

	return 1;
}

static void
private_lighting_render (LIRenScene*    self,
                         LIRenContext*  context,
                         LIRenDeferred* framebuffer)
{
	int index = 0;
	int viewport[4];
	float bounds[4];
	LIAlgPtrdicIter iter;
	LIMatMatrix matrix;
	LIRenLight* light;
	LIRenShader* shader;
	LIRenTexture textures[4];

	shader = liren_render_find_shader (self->render, "deferred");
	if (shader == NULL)
		return;

	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable (GL_BLEND);
	glGetIntegerv (GL_VIEWPORT, viewport);
	matrix = limat_matrix_identity ();
	textures[0].texture = framebuffer->diffuse_texture;
	textures[0].params.magfilter = GL_NEAREST;
	textures[0].params.minfilter = GL_NEAREST;
	textures[0].params.wraps = GL_CLAMP_TO_EDGE;
	textures[0].params.wrapt = GL_CLAMP_TO_EDGE;
	textures[1].texture = framebuffer->specular_texture;
	textures[1].params.magfilter = GL_NEAREST;
	textures[1].params.minfilter = GL_NEAREST;
	textures[1].params.wraps = GL_CLAMP_TO_EDGE;
	textures[1].params.wrapt = GL_CLAMP_TO_EDGE;
	textures[2].texture = framebuffer->normal_texture;
	textures[2].params.magfilter = GL_NEAREST;
	textures[2].params.minfilter = GL_NEAREST;
	textures[2].params.wraps = GL_CLAMP_TO_EDGE;
	textures[2].params.wrapt = GL_CLAMP_TO_EDGE;
	textures[3].texture = framebuffer->depth_texture;
	textures[3].params.magfilter = GL_NEAREST;
	textures[3].params.minfilter = GL_NEAREST;
	textures[3].params.wraps = GL_CLAMP_TO_EDGE;
	textures[3].params.wrapt = GL_CLAMP_TO_EDGE;
	liren_context_set_flags (context, LIREN_FLAG_LIGHTING | LIREN_FLAG_TEXTURING);
	liren_context_set_matrix (context, &matrix);
	liren_context_set_shader (context, shader);
	liren_context_set_lights (context, NULL, 0);
	liren_context_set_textures (context, textures, 4);

	/* Let each light lit the scene. */
	LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
	{
		light = iter.value;
		liren_context_set_lights (context, &light, 1);
		liren_context_bind (context);

		/* FIXME: It should be possible to avoid these state changes. */
		glDisable (GL_CULL_FACE);
		if (index++ >= 1)
		{
			glBlendFunc (GL_ONE, GL_ONE);
			glEnable (GL_BLEND);
		}

		/* Calculate light rectangle. */
		if (private_light_bounds (self, light, context, viewport, bounds))
		{
			glScissor (bounds[0], bounds[1], bounds[2], bounds[3]);
			glEnable (GL_SCISSOR_TEST);
		}
		else
			glDisable (GL_SCISSOR_TEST);

		/* Shade light rectangle. */
		glBegin (GL_QUADS);
		glTexCoord2i (0, 0);
		glVertex2i (-1, -1);
		glTexCoord2i (0, 1);
		glVertex2i (-1, 1);
		glTexCoord2i (1, 1);
		glVertex2i (1, 1);
		glTexCoord2i (1, 0);
		glVertex2i (1, -1);
		glEnd ();
	}

	liren_context_set_lights (context, NULL, 0);
	liren_context_unbind (context);
	glDisable (GL_SCISSOR_TEST);
}

static void
private_particle_render (LIRenScene* self)
{
	float color0[4];
	float color1[4];
	float attenuation[] = { 1.0f, 1.0f, 0.0f };
	LIAlgStrdicIter iter;
	LIParGroup* group;
	LIParLine* line;
	LIParPoint* particle;
	LIRenImage* image;

	/* Set point particle rendering mode. */
	if (livid_features.shader_model >= 3)
		glUseProgramObjectARB (0);
	glColor3f (1.0f, 1.0f, 1.0f);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	if (GLEW_ARB_point_sprite)
	{
		glEnable (GL_POINT_SPRITE_ARB);
		glTexEnvi (GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
		glPointParameterfvARB (GL_POINT_DISTANCE_ATTENUATION_ARB, attenuation);
		glPointSize (64.0f);
	}
	else
		glPointSize (4.0f);

	/* Render point particles. */
	LIALG_STRDIC_FOREACH (iter, self->particles->groups)
	{
		group = iter.value;
		image = liren_render_find_image (self->render, group->texture);
		if (image == NULL)
		{
			liren_render_load_image (self->render, group->texture);
			image = liren_render_find_image (self->render, group->texture);
		}
		if (image != NULL)
		{
			glBindTexture (GL_TEXTURE_2D, image->texture->texture);
			glBegin (GL_POINTS);
			for (particle = group->points.used ; particle != NULL ; particle = particle->next)
			{
				lipar_point_get_color (particle, color0);
				glColor4fv (color0);
				glVertex3f (particle->position.x, particle->position.y, particle->position.z);
			}
			glEnd ();
		}
	}
	if (GLEW_ARB_point_sprite)
		glDisable (GL_POINT_SPRITE_ARB);

	/* Render line particles. */
	glBindTexture (GL_TEXTURE_2D, 0);
	glBegin (GL_LINES);
	for (line = self->particles->lines.used ; line != NULL ; line = line->next)
	{
		lipar_line_get_colors (line, color0, color1);
		glColor4fv (color0);
		glVertex3f (line->position[0].x, line->position[0].y, line->position[0].z);
		glColor4fv (color1);
		glVertex3f (line->position[1].x, line->position[1].y, line->position[1].z);
	}
	glEnd ();

	/* Set normal rendering mode. */
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void
private_render (LIRenScene*   self,
                LIRenContext* context,
                lirenCallback call,
                void*         data)
{
	LIAlgU32dicIter iter0;
	LIAlgPtrdicIter iter1;
	LIMatAabb aabb;
	LIRenGroup* group;
	LIRenObject tmpobj;
	LIRenObject* rndobj;
	LIRenGroupObject* grpobj;

	LIALG_U32DIC_FOREACH (iter0, self->objects)
	{
		rndobj = iter0.value;
		if (!liren_object_get_realized (rndobj))
			continue;
		liren_object_get_bounds (rndobj, &aabb);
		if (limat_frustum_cull_aabb (&context->frustum, &aabb))
			continue;
		call (context, rndobj, data);
	}
	LIALG_PTRDIC_FOREACH (iter1, self->groups)
	{
		group = iter1.value;
		if (!liren_group_get_realized (group))
			continue;
		liren_group_get_bounds (group, &aabb);
		if (limat_frustum_cull_aabb (&context->frustum, &aabb))
			continue;
		for (grpobj = group->objects ; grpobj != NULL ; grpobj = grpobj->next)
		{
			memset (&tmpobj, 0, sizeof (LIRenObject));
			tmpobj.transform = grpobj->transform;
			tmpobj.scene = self;
			tmpobj.model = grpobj->model;
			tmpobj.orientation.center = grpobj->transform.position;
			tmpobj.orientation.matrix = limat_convert_transform_to_matrix (grpobj->transform);
			call (context, &tmpobj, data);
		}
	}
}

static int
private_sort_objects (const void* first,
                      const void* second)
{
	const LIRenObject* a = first;
	const LIRenObject* b = second;

	if (a->sort < b->sort) return -1;
	if (a->sort > b->sort) return 1;
	return 0;
}

static int
private_sort_scene (LIRenScene*   self,
                    LIRenContext* context,
                    LIRenObject** result_list,
                    int*          result_count)
{
	int i;
	int capacity;
	int count;
	LIAlgU32dicIter iter0;
	LIAlgPtrdicIter iter1;
	LIMatAabb aabb;
	LIMatVector center0;
	LIMatVector center1;
	LIRenGroup* group;
	LIRenGroupObject* grpobj;
	LIRenObject* tmp;
	LIRenObject* objects;
	LIRenObject* rndobj;

	count = 0;
	capacity = 256;
	objects = lisys_calloc (capacity, sizeof (LIRenObject));
	if (objects == NULL)
		return 0;

	/* Collect scene objects. */
	LIALG_U32DIC_FOREACH (iter0, self->objects)
	{
		rndobj = iter0.value;
		if (!liren_object_get_realized (rndobj))
			continue;
		liren_object_get_bounds (rndobj, &aabb);
		if (limat_frustum_cull_aabb (&context->frustum, &aabb))
			continue;
		if (count == capacity)
		{
			capacity <<= 1;
			tmp = lisys_realloc (objects, capacity * sizeof (LIRenObject));
			if (tmp == NULL)
			{
				lisys_free (objects);
				return 0;
			}
			objects = tmp;
		}
		tmp = objects + count;
		*tmp = *rndobj;
		count++;
	}

	/* Collect group objects. */
	LIALG_PTRDIC_FOREACH (iter1, self->groups)
	{
		group = iter1.value;
		if (!liren_group_get_realized (group))
			continue;
		liren_group_get_bounds (group, &aabb);
		if (limat_frustum_cull_aabb (&context->frustum, &aabb))
			continue;
		for (grpobj = group->objects ; grpobj != NULL ; grpobj = grpobj->next)
		{
			if (count == capacity)
			{
				capacity <<= 1;
				tmp = lisys_realloc (objects, capacity * sizeof (LIRenObject));
				if (tmp == NULL)
				{
					lisys_free (objects);
					return 0;
				}
				objects = tmp;
			}
			tmp = objects + count;
			memset (tmp, 0, sizeof (LIRenObject));
			tmp->transform = grpobj->transform;
			tmp->scene = self;
			tmp->model = grpobj->model;
			tmp->orientation.center = grpobj->transform.position;
			tmp->orientation.matrix = limat_convert_transform_to_matrix (grpobj->transform);
			count++;
		}
	}

	/* Calculate distances to camera. */
	liren_context_bind (context);
	center0 = limat_matrix_transform (context->modelviewinverse, limat_vector_init (0.0f, 0.0f, 0.0f));
	for (i = 0 ; i < count ; i++)
	{
		rndobj = objects + i;
		liren_object_get_center (rndobj, &center1);
		rndobj->sort = limat_vector_get_length (limat_vector_subtract (center0, center1));
	}

	/* Sort the list. */
	qsort (objects, count, sizeof (LIRenObject), private_sort_objects);

	/* Return the list. */
	*result_list = objects;
	*result_count = count;

	return 1;
}

/** @} */
/** @} */
