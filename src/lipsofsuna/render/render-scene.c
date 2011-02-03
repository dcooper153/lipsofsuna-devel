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
 * \addtogroup LIRenRender Render
 * @{
 */

#include <lipsofsuna/system.h>
#include "render.h"
#include "render-draw.h"

#define LIREN_LIGHT_MAXIMUM_RATING 100.0f
#define LIREN_PARTICLE_MAXIMUM_COUNT 1000

static int private_init (
	LIRenScene* self);

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
		//lisys_assert (self->objects->size == 0);
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
int liren_scene_render_begin (
	LIRenScene*    self,
	LIRenDeferred* framebuffer,
	LIMatMatrix*   modelview,
	LIMatMatrix*   projection,
	LIMatFrustum*  frustum)
{
	LIAlgPtrdicIter iter;
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
	liren_context_set_frustum (context, frustum);
	liren_context_set_projection (context, projection);
	liren_context_set_viewmatrix (context, modelview);

	/* Depth sort scene. */
	if (!private_sort_scene (self, context))
		return 0;

	/* Precalculate light settings. */
	LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
		liren_light_update_cache (iter.value, context);

	/* Reset profiling. */
#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.objects = 0;
	self->render->profiling.materials = 0;
	self->render->profiling.faces = 0;
	self->render->profiling.vertices = 0;
#endif

	/* Update state. */
	self->state.rendering = 1;
	self->state.postproc_passes = 0;
	self->state.context = context;
	self->state.framebuffer = framebuffer;
	glGetIntegerv (GL_VIEWPORT, self->state.original_viewport);

	/* Enable backbuffer viewport. */
	glBindFramebuffer (GL_FRAMEBUFFER, framebuffer->render_framebuffer);
	glViewport (0, 0, framebuffer->width, framebuffer->height);
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
void liren_scene_render_end (
	LIRenScene* self)
{
	GLint* viewport;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Disable backbuffer viewport. */
	viewport = self->state.original_viewport;
	glViewport (viewport[0], viewport[1], viewport[1], viewport[2]);

	/* Blit to the screen. */
	/* If no post-processing steps were done, we blit directly from the render
	   buffer. Otherwise, the final image is in the first post-processing
	   buffer and we need to blit from there. */
	if (self->state.postproc_passes)
		glBindFramebuffer (GL_READ_FRAMEBUFFER, self->state.framebuffer->postproc_framebuffers[0]);
	else
		glBindFramebuffer (GL_READ_FRAMEBUFFER, self->state.framebuffer->render_framebuffer);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer (0, 0, viewport[2], viewport[3], viewport[0], viewport[1],
		viewport[0] + viewport[2], viewport[1] + viewport[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);

	/* Profiling report. */
#ifdef LIREN_ENABLE_PROFILING
	printf ("RENDER PROFILING: objects=%d materials=%d polys=%d verts=%d\n",
		self->render->profiling.objects, self->render->profiling.materials,
		self->render->profiling.faces, self->render->profiling.vertices);
#endif
}

/**
 * \brief Renders a pass.
 *
 * Renders materials whose shaders implement the selected pass. Each pass
 * can be rendered either with or without lighting and sorting and the
 * output can be written either to the deferred of forward targets.
 *
 * \param self Scene.
 * \param pass Pass number.
 * \param lighting Nonzero to enable lighting.
 * \param sorting Nonzero to enable sorting.
 */
void liren_scene_render_pass (
	LIRenScene* self,
	int         pass,
	int         lighting,
	int         sorting)
{
	int i;
	const float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float equation[3] = { 1.0f, 0.0f, 0.0f };
	LIAlgPtrdicIter iter;
	LIMatAabb aabb;
	LIMatMatrix identity;
	LIMatVector position;
	LIRenContext* context;
	LIRenLight* ambient_light;
	LIRenLight* light;
	LIRenSortface* face;
	LIRenSortgroup* group;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Initialize pass. */
	identity = limat_matrix_identity ();
	context = self->state.context;
	ambient_light = liren_light_new (self, black, black, black, equation, M_PI, 0.0f, 0);
	liren_light_set_ambient (ambient_light, black);
	liren_light_update_cache (ambient_light, self->state.context);

	/* Render each group. */
	if (!lighting && !sorting)
	{
		/* Render unsorted groups once without lighting. */
		for (i = 0 ; i < self->sort->groups.count ; i++)
		{
			group = self->sort->groups.array + i;
			if (!group->material->shader->passes[pass].program)
				continue;
			liren_context_set_material (context, group->material);
			liren_context_set_modelmatrix (context, &group->matrix);
			liren_context_set_shader (context, pass, group->material->shader);
			liren_context_set_textures (context, group->material->textures.array, group->material->textures.count);
			liren_context_set_mesh (context, group->mesh);
			liren_context_bind (context);
			liren_context_render_array (context, GL_TRIANGLES, group->index, group->count);
		}
	}
	else if (!sorting)
	{
		/* Render unsorted groups once for each light. */
		LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
		{
			light = iter.value;
			if (!light->enabled)
				continue;
			liren_context_set_light (context, light);
			liren_light_get_bounds (light, &aabb);
			for (i = 0 ; i < self->sort->groups.count ; i++)
			{
				group = self->sort->groups.array + i;
				if (!group->material->shader->passes[pass].program)
					continue;
				if (!limat_aabb_intersects_aabb (&aabb, &group->bounds))
					continue;
				liren_context_set_material (context, group->material);
				liren_context_set_modelmatrix (context, &group->matrix);
				liren_context_set_shader (context, pass, group->material->shader);
				liren_context_set_textures (context, group->material->textures.array, group->material->textures.count);
				liren_context_set_mesh (context, group->mesh);
				liren_context_bind (context);
				liren_context_render_array (context, GL_TRIANGLES, group->index, group->count);
			}
		}
	}
	else if (!lighting)
	{
		/* Render sorted groups once without lighting. */
		for (i = self->sort->buckets.count - 1 ; i >= 0 ; i--)
		{
			for (face = self->sort->buckets.array[i] ; face != NULL ; face = face->next)
			{
				if (face->type == LIREN_SORT_TYPE_FACE)
				{
					/* Render a single transparent triangle. */
					if (!face->face.material->shader->passes[pass].program)
						continue;
					liren_context_set_material (context, face->face.material);
					liren_context_set_modelmatrix (context, &face->face.matrix);
					liren_context_set_shader (context, pass, face->face.material->shader);
					liren_context_set_textures (context, face->face.material->textures.array, face->face.material->textures.count);
					liren_context_set_mesh (context, face->face.mesh);
					liren_context_bind (context);
					liren_context_render_array (context, GL_TRIANGLES, face->face.index, 3);
				}
				else if (face->type == LIREN_SORT_TYPE_GROUP)
				{
					/* Render a group of transparent triangles. */
					if (!face->group.material->shader->passes[pass].program)
						continue;
					liren_context_set_material (context, face->group.material);
					liren_context_set_modelmatrix (context, &face->group.matrix);
					liren_context_set_shader (context, pass, face->group.material->shader);
					liren_context_set_textures (context, face->group.material->textures.array, face->group.material->textures.count);
					liren_context_set_mesh (context, face->group.mesh);
					liren_context_bind (context);
					liren_context_render_array (context, GL_TRIANGLES, face->group.index, face->group.count);
				}
				else if (face->type == LIREN_SORT_TYPE_PARTICLE)
				{
					/* Render a particle. */
					if (!face->particle.shader->passes[pass].program)
						continue;
					liren_context_set_modelmatrix (self->state.context, &identity);
					liren_context_set_shader (self->state.context, pass, face->particle.shader);
					liren_context_set_textures_raw (self->state.context, &face->particle.image->texture->texture, 1);
					liren_context_bind (self->state.context);
					glBegin (GL_TRIANGLES);
					glVertexAttrib2f (LIREN_ATTRIBUTE_TEXCOORD, face->particle.diffuse[3], face->particle.size);
					position = face->particle.position;
					glVertexAttrib3fv (LIREN_ATTRIBUTE_NORMAL, face->particle.diffuse);
					glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
					glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
					glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
					glEnd ();
				}
			}
		}
	}
	else
	{
		/* Render sorted groups once for each light. */
		for (i = self->sort->buckets.count - 1 ; i >= 0 ; i--)
		{
			for (face = self->sort->buckets.array[i] ; face != NULL ; face = face->next)
			{
				if (face->type == LIREN_SORT_TYPE_FACE)
				{
					/* Render a single transparent triangle. */
					/* First set the base color with an ambient pass. */
					if (!face->face.material->shader->passes[pass].program)
						continue;
					liren_context_set_light (self->state.context, ambient_light);
					liren_context_set_material (context, face->face.material);
					liren_context_set_modelmatrix (context, &face->face.matrix);
					liren_context_set_shader (context, pass, face->face.material->shader);
					liren_context_set_textures (context, face->face.material->textures.array, face->face.material->textures.count);
					liren_context_set_mesh (context, face->face.mesh);
					liren_context_set_blend (context, 1, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					liren_context_bind (context);
					liren_context_render_array (context, GL_TRIANGLES, face->face.index, 3);

					/* Render light contributions with shader's native blend mode. */
					liren_context_set_shader (context, pass, face->group.material->shader);
					LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
					{
						light = iter.value;
						if (!light->enabled)
							continue;
						liren_light_get_bounds (light, &aabb);
						if (!limat_aabb_intersects_aabb (&aabb, &face->face.bounds))
							continue;
						liren_context_set_light (self->state.context, light);
						liren_context_bind (context);
						liren_context_render_array (context, GL_TRIANGLES, face->face.index, 3);
					}
				}
				else if (face->type == LIREN_SORT_TYPE_GROUP)
				{
					/* Render a group of transparent triangles. */
					/* First set the base color with an ambient pass. */
					if (!face->group.material->shader->passes[pass].program)
						continue;
					liren_context_set_light (self->state.context, ambient_light);
					liren_context_set_material (context, face->group.material);
					liren_context_set_modelmatrix (context, &face->group.matrix);
					liren_context_set_shader (context, pass, face->group.material->shader);
					liren_context_set_textures (context, face->group.material->textures.array, face->group.material->textures.count);
					liren_context_set_mesh (context, face->group.mesh);
					liren_context_set_blend (context, 1, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					liren_context_bind (context);
					liren_context_render_array (context, GL_TRIANGLES, face->group.index, face->group.count);

					/* Render light contributions with shader's native blend mode. */
					liren_context_set_shader (context, pass, face->group.material->shader);
					LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
					{
						light = iter.value;
						if (!light->enabled)
							continue;
						liren_light_get_bounds (light, &aabb);
						if (!limat_aabb_intersects_aabb (&aabb, &face->group.bounds))
							continue;
						liren_context_set_light (self->state.context, light);
						liren_context_bind (context);
						liren_context_render_array (context, GL_TRIANGLES, face->group.index, face->group.count);
					}
				}
				else if (face->type == LIREN_SORT_TYPE_PARTICLE)
				{
					/* Render a particle. */
					/* First set the base color with an ambient pass. */
					if (!face->particle.shader->passes[pass].program)
						continue;
					liren_context_set_light (self->state.context, ambient_light);
					liren_context_set_modelmatrix (context, &identity);
					liren_context_set_shader (context, pass, face->particle.shader);
					liren_context_set_textures_raw (context, &face->particle.image->texture->texture, 1);
					liren_context_set_blend (context, 1, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					liren_context_bind (context);
					position = face->particle.position;
					glVertexAttrib2f (LIREN_ATTRIBUTE_TEXCOORD, face->particle.diffuse[3], face->particle.size);
					glVertexAttrib3fv (LIREN_ATTRIBUTE_NORMAL, face->particle.diffuse);
					glBegin (GL_TRIANGLES);
					glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
					glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
					glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
					glEnd ();

					/* Render light contributions with shader's native blend mode. */
					liren_context_set_shader (context, pass, face->group.material->shader);
					LIALG_PTRDIC_FOREACH (iter, self->lighting->lights)
					{
						light = iter.value;
						if (!light->enabled)
							continue;
						liren_context_set_light (self->state.context, light);
						liren_context_bind (context);
						glBegin (GL_TRIANGLES);
						glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
						glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
						glVertexAttrib3f (LIREN_ATTRIBUTE_COORD, position.x, position.y, position.z);
						glEnd ();
					}
				}
			}
		}
	}

	/* Free pass. */
	liren_context_set_light (self->state.context, NULL);
	liren_light_free (ambient_light);
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
	LIRenDeferred* framebuffer;
	LIRenShader* shader;

	/* Validate state. */
	if (!self->state.rendering)
		return;

	/* Find the shader. */
	shader = liren_render_find_shader (self->render, name);
	if (shader == NULL)
		return;
	framebuffer = self->state.framebuffer;

	/* Blit to the post-processing buffer. */
	/* This is needed because the render buffer is multisampled but the
	   post-processing buffer isn't due to fragment shader performance.
	   Multisamples needs to be resolved before post-processing. */
	if (!self->state.postproc_passes)
	{
		glBindFramebuffer (GL_READ_FRAMEBUFFER, framebuffer->render_framebuffer);
		glBindFramebuffer (GL_DRAW_FRAMEBUFFER, framebuffer->postproc_framebuffers[0]);
		glBlitFramebuffer (0, 0, framebuffer->width, framebuffer->height, 0, 0,
			framebuffer->width, framebuffer->height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	/* Calculate the pixel size in texture units. */
	param[0] = 1.0f / framebuffer->width;
	param[1] = 1.0f / framebuffer->height;
	param[2] = 0.0;
	param[3] = 0.0;

	/* Render from the first buffer to the second. */
	glBindFramebuffer (GL_FRAMEBUFFER, framebuffer->postproc_framebuffers[1]);
	liren_context_set_buffer (self->state.context, self->render->helpers.unit_quad);
	liren_context_set_blend (self->state.context, 0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	liren_context_set_cull (self->state.context, 0, GL_CCW);
	liren_context_set_param (self->state.context, param);
	liren_context_set_shader (self->state.context, 0, shader);
	liren_context_set_textures_raw (self->state.context, framebuffer->postproc_textures, 1);
	liren_context_bind (self->state.context);
	liren_context_render_indexed (self->state.context, 0, 6);
	self->state.postproc_passes++;

	/* Swap the buffers so that we can chain passes. */
	tmp = framebuffer->postproc_framebuffers[0];
	framebuffer->postproc_framebuffers[0] = framebuffer->postproc_framebuffers[1];
	framebuffer->postproc_framebuffers[1] = tmp;
	tmp = framebuffer->postproc_textures[0];
	framebuffer->postproc_textures[0] = framebuffer->postproc_textures[1];
	framebuffer->postproc_textures[1] = tmp;
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
}

/*****************************************************************************/

static int private_init (
	LIRenScene* self)
{
	self->lighting = liren_lighting_new (self->render);
	if (self->lighting == NULL)
		return 0;
	self->sort = liren_sort_new (self->render);
	if (self->sort == NULL)
		return 0;
	return 1;
}

static int private_sort_scene (
	LIRenScene*   self,
	LIRenContext* context)
{
	LIAlgU32dicIter iter0;
	LIMatAabb aabb;
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

	return 1;
}

/** @} */
/** @} */
