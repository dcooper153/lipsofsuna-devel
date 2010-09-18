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

static int private_init (
	LIRenScene* self);

static int private_light_bounds (
	LIRenScene*   self,
	LIRenLight*   light,
	LIRenContext* context,
	const int*    viewport,
	float*        result);

static void private_lighting_render (
	LIRenScene*   self,
	LIRenContext* context,
	const int*    viewport,
	int           type);

static int private_sort_scene (
	LIRenScene*   self,
	LIRenContext* context);

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

	if (self->sort != NULL)
		liren_sort_free (self->sort);

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
	LIRenContext* context;

	lisys_assert (modelview != NULL);
	lisys_assert (projection != NULL);
	lisys_assert (frustum != NULL);
	if (framebuffer == NULL)
		return 0;

	/* Initialize context. */
	context = liren_render_get_context (self->render);
	liren_context_init (context);
	liren_context_set_scene (context, self);
	liren_context_set_viewmatrix (context, modelview);
	liren_context_set_projection (context, projection);
	liren_context_set_frustum (context, frustum);

	/* Depth sort scene. */
	if (!private_sort_scene (self, context))
		return 0;

	/* Change render state. */
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glFrontFace (GL_CCW);
	glDepthMask (GL_TRUE);
	glDepthFunc (GL_LEQUAL);
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
	GLint viewport[4];

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Disable backbuffer viewport. */
	glPopAttrib ();
	glGetIntegerv (GL_VIEWPORT, viewport);

	/* Blit from the post-processing FBO to the screen. */
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER_EXT, 0);
	glBindFramebuffer (GL_READ_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[0]);
	glBlitFramebuffer (0, 0, viewport[2], viewport[3], viewport[0], viewport[1],
		viewport[0] + viewport[2], viewport[1] + viewport[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);

	/* Update state. */
	memset (&self->state, 0, sizeof (self->state));

	/* Change render state. */
	glUseProgramObjectARB (0);
	glBindVertexArray (0);
	glEnable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDepthMask (GL_FALSE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f (1.0f, 1.0f, 1.0f);
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
	glBindFramebuffer (GL_FRAMEBUFFER, self->state.framebuffer->deferred_fbo);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable (GL_BLEND);
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
	int viewport[4];
	LIMatMatrix matrix;
	LIRenTexture textures[4];

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Change render state. */
	glBindFramebuffer (GL_FRAMEBUFFER, self->state.framebuffer->postproc_fbo[0]);

	/* Render lit fragments to post-processing buffer. */
	glPushAttrib (GL_SCISSOR_BIT);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable (GL_BLEND);
	glGetIntegerv (GL_VIEWPORT, viewport);
	matrix = limat_matrix_identity ();
	textures[0].texture = self->state.framebuffer->diffuse_texture;
	textures[0].sampler = 0;
	textures[1].texture = self->state.framebuffer->specular_texture;
	textures[1].sampler = 0;
	textures[2].texture = self->state.framebuffer->normal_texture;
	textures[2].sampler = 0;
	textures[3].texture = self->state.framebuffer->depth_texture;
	textures[3].sampler = 0;
	liren_context_set_flags (self->state.context, LIREN_FLAG_LIGHTING | LIREN_FLAG_TEXTURING);
	liren_context_set_modelmatrix (self->state.context, &matrix);
	liren_context_set_lights (self->state.context, NULL, 0);
	liren_context_set_textures (self->state.context, textures, 4);
	liren_context_set_buffer (self->state.context, self->render->helpers.unit_quad);
	private_lighting_render (self, self->state.context, viewport, 0);
	private_lighting_render (self, self->state.context, viewport, 1);
	liren_context_set_lights (self->state.context, NULL, 0);
	glDisable (GL_SCISSOR_TEST);
	glPopAttrib ();

	/* Change render state. */
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
	LIRenSortgroup* group;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Render face groups to G-buffer. */
	for (i = 0 ; i < self->sort->groups.count ; i++)
	{
		group = self->sort->groups.array + i;
		if (!self->state.alphatest && group->transparent)
			continue;
		liren_draw_default (self->state.context, group->index, group->count,
			&group->matrix, group->material, group->buffer);
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
	LIMatAabb aabb;
	LIRenLight* light;
	LIRenSortgroup* group;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Change render state. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[0]);
	glEnable (GL_DEPTH_TEST);
	glDisable (GL_BLEND);

	/* Render face groups to post-processing buffer. */
	for (i = 0 ; i < self->sort->groups.count ; i++)
	{
		group = self->sort->groups.array + i;
		if (!alpha && group->transparent)
			continue;
		/* FIXME: Looping through all lights and objects is slow. */
		LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
		{
			light = iter.value;
			if (light->enabled)
			{
				liren_light_get_bounds (light, &aabb);
				if (limat_aabb_intersects_aabb (&aabb, &group->bounds))
				{
					liren_draw_default (self->state.context, group->index, group->count,
						&group->matrix, group->material, group->buffer);
				}
			}
		}
	}
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
	LIMatAabb aabb;
	LIMatMatrix identity;
	LIMatVector position;
	LIRenLight* light;
	LIRenSortface* face;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	identity = limat_matrix_identity ();

	/* Change render state. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[0]);
	glEnable (GL_BLEND);
	glEnable (GL_CULL_FACE);
	glEnable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Render transparent faces to post-processing buffer. */
	for (i = self->sort->buckets.count - 1 ; i >= 0 ; i--)
	{
		/* Render faces with lighting. */
		/* FIXME: Looping through all lights and faces is slow. */
		LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
		{
			light = iter.value;
			if (light->enabled)
			{
				for (face = self->sort->buckets.array[i] ; face != NULL ; face = face->next)
				{
					if (face->type != LIREN_SORT_TYPE_FACE)
						continue;
					liren_light_get_bounds (light, &aabb);
					if (!limat_aabb_intersects_aabb (&aabb, &face->face.bounds))
						continue;
					liren_context_set_lights (self->state.context, &light, 1);
					liren_draw_default (self->state.context, face->face.index, 3,
						&face->face.matrix, face->face.material, face->face.buffer);
				}
			}
		}

		/* Render particles without lighting. */
		for (face = self->sort->buckets.array[i] ; face != NULL ; face = face->next)
		{
			if (face->type != LIREN_SORT_TYPE_PARTICLE)
				continue;
			liren_context_set_modelmatrix (self->state.context, &identity);
			liren_context_set_shader (self->state.context, face->particle.shader);
			liren_context_set_lights (self->state.context, NULL, 0);
			liren_context_set_textures_raw (self->state.context, &face->particle.image->texture->texture, 1);
			liren_context_bind (self->state.context);
			glEnable (GL_DEPTH_TEST);
			glDisable (GL_CULL_FACE);
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE);
			glBegin (GL_TRIANGLES);
			glVertexAttrib2f (LIREN_ATTRIBUTE_TEXCOORD, face->particle.size, face->particle.size);
			position = face->particle.position;
			glVertexAttrib4fv (LIREN_ATTRIBUTE_NORMAL, face->particle.diffuse);
			glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
			glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
			glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
			glEnd ();
		}
	}

	/* Change render state. */
	liren_context_set_lights (self->state.context, NULL, 0);
	glDepthMask (GL_TRUE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * \brief Performs post-processing for the post-processing buffer.
 *
 * Executes zero or more post-processing passes. Each pass takes the post-processing
 * buffer as an input, renders to a temporary buffer, and makes the temporary buffer
 * the input of the next operation.
 *
 * \param self Scene.
 * \param name Shader name.
 */
void liren_scene_render_postproc (
	LIRenScene* self,
	const char* name)
{
	float param[4];
	GLuint tmp;
	LIRenShader* shader;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Find the shader. */
	shader = liren_render_find_shader (self->render, name);
	if (shader == NULL)
		return;

	/* Calculate the pixel size in texture units. */
	param[0] = 1.0f / self->state.framebuffer->width;
	param[1] = 1.0f / self->state.framebuffer->height;
	param[2] = 0.0;
	param[3] = 0.0;

	/* Change render state. */
	liren_context_set_buffer (self->state.context, self->render->helpers.unit_quad);
	liren_context_set_shader (self->state.context, shader);
	liren_context_set_textures_raw (self->state.context, self->state.framebuffer->postproc_texture, 1);
	liren_context_set_param (self->state.context, param);
	liren_context_bind (self->state.context);
	glDisable (GL_BLEND);
	glDisable (GL_CULL_FACE);
	glDisable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	glBindFramebuffer (GL_FRAMEBUFFER_EXT, self->state.framebuffer->postproc_fbo[1]);

	/* Render from the first buffer to the second. */
	liren_context_render_indexed (self->state.context, 0, 6);

	/* Swap the buffers so that we can chain passes. */
	tmp = self->state.framebuffer->postproc_fbo[0];
	self->state.framebuffer->postproc_fbo[0] = self->state.framebuffer->postproc_fbo[1];
	self->state.framebuffer->postproc_fbo[1] = tmp;
	tmp = self->state.framebuffer->postproc_texture[0];
	self->state.framebuffer->postproc_texture[0] = self->state.framebuffer->postproc_texture[1];
	self->state.framebuffer->postproc_texture[1] = tmp;
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

static int private_init (
	LIRenScene* self)
{
	self->lighting = liren_lighting_new (self->render);
	if (self->lighting == NULL)
		return 0;
	self->particles = lipar_manager_new (LIREN_PARTICLE_MAXIMUM_COUNT);
	if (self->particles == NULL)
		return 0;
	self->sort = liren_sort_new (self->render);
	if (self->sort == NULL)
		return 0;
	return 1;
}

static int private_light_bounds (
	LIRenScene*   self,
	LIRenLight*   light,
	LIRenContext* context,
	const int*    viewport,
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
	limat_matrix_project (context->matrix.projection, context->matrix.view, viewport, p, &tmp);
	min = max = tmp;
	for (i = 1 ; i < 8 ; i++)
	{
		tmp = limat_vector_init (0.0f, 0.0f, 0.0f);
		limat_matrix_project (context->matrix.projection, context->matrix.view, viewport, p + i, &tmp);
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

static void private_lighting_render (
	LIRenScene*   self,
	LIRenContext* context,
	const int*    viewport,
	int           type)
{
	int ltype;
	int index = 0;
	float bounds[4];
	LIAlgPtrdicIter iter;
	LIRenLight* light;
	LIRenShader* shader;

	if (!type)
		shader = liren_render_find_shader (self->render, "deferred");
	else
		shader = liren_render_find_shader (self->render, "deferred-spotlight");
	if (shader == NULL)
		return;
	liren_context_set_shader (context, shader);

	/* Let each light lit the scene. */
	LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
	{
		light = iter.value;
		ltype = liren_light_get_type (light);
		if (ltype == LIREN_UNIFORM_LIGHTTYPE_POINT)
		{
			if (type != 0)
				continue;
		}
		else
		{
			if (type != 1)
				continue;
		}
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
		liren_context_render_indexed (context, 0, 6);
	}
}

static int private_sort_scene (
	LIRenScene*   self,
	LIRenContext* context)
{
	LIAlgU32dicIter iter0;
	LIAlgPtrdicIter iter1;
	LIMatAabb aabb;
	LIMatMatrix matrix;
	LIRenGroup* group;
	LIRenGroupObject* grpobj;
	LIRenObject* rndobj;

	/* Initialize sorting. */
	liren_sort_clear (self->sort, &context->matrix.view, &context->matrix.projection);
	liren_context_bind (context);

	/* Collect scene objects. */
	LIALG_U32DIC_FOREACH (iter0, self->objects)
	{
		rndobj = iter0.value;
		if (!liren_object_get_realized (rndobj))
			continue;
		liren_object_get_bounds (rndobj, &aabb);
		if (limat_frustum_cull_aabb (&context->frustum, &aabb))
			continue;
		liren_sort_add_object (self->sort, rndobj);
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
			matrix = limat_convert_transform_to_matrix (grpobj->transform);
			liren_sort_add_model (self->sort, &aabb, &matrix, grpobj->model);
		}
	}

	return 1;
}

/** @} */
/** @} */
