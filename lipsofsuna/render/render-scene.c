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
private_init_lights (LIRenScene* self);

static int
private_init_particles (LIRenScene* self);

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
	if (!private_init_lights (self) ||
	    !private_init_particles (self))
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
		assert (self->groups->size == 0);
		lialg_ptrdic_free (self->groups);
	}
	if (self->objects != NULL)
	{
		assert (self->objects->size == 0);
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
 * \param position Position of the particle.
 * \param velocity Velocity of the particle.
 * \return Particle owned by the scene or NULL.
 */
LIParPoint*
liren_scene_insert_particle (LIRenScene*        self,
                             const LIMatVector* position,
                             const LIMatVector* velocity)
{
	return lipar_manager_insert_point (self->particles, position, velocity);
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
	LIRenContext context;

	pick = limat_matrix_pick (x, y, size, size, viewport);
	pick = limat_matrix_multiply (pick, *projection);
	liren_context_init (&context, self);
	liren_context_set_modelview (&context, modelview);
	liren_context_set_projection (&context, &pick);
	liren_context_set_frustum (&context, frustum);

	/* Pick scene. */
	glSelectBuffer (256, selection);
	glRenderMode (GL_SELECT);
	glInitNames ();
	glPushName (0);
	private_render (self, &context, liren_draw_picking, NULL);
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
 * \brief Renders the scene.
 *
 * \param self Scene.
 * \param framebuffer Framebuffer or NULL.
 * \param modelview Modelview matrix.
 * \param projection Projection matrix.
 * \param frustum Frustum used for culling.
 */
void
liren_scene_render (LIRenScene*    self,
                    LIRenDeferred* framebuffer,
                    LIMatMatrix*   modelview,
                    LIMatMatrix*   projection,
                    LIMatFrustum*  frustum)
{
	int i;
	LIRenContext context;
	GLfloat none[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	assert (modelview != NULL);
	assert (projection != NULL);
	assert (frustum != NULL);

	liren_check_errors ();
	liren_context_init (&context, self);
	liren_context_set_modelview (&context, modelview);
	liren_context_set_projection (&context, projection);
	liren_context_set_frustum (&context, frustum);

	/* Bind framebuffer. */
	if (!self->render->shader.enabled)
		framebuffer = NULL;
	if (framebuffer != NULL)
	{
		glPushAttrib (GL_VIEWPORT_BIT);
		glViewport (0, 0, framebuffer->width, framebuffer->height);
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, framebuffer->deferred_fbo);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

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
#ifdef LIREN_ENABLE_PROFILING
	self->render->profiling.objects = 0;
	self->render->profiling.materials = 0;
	self->render->profiling.faces = 0;
	self->render->profiling.vertices = 0;
#endif
	private_render (self, &context, liren_draw_opaque, NULL);
	private_render (self, &context, liren_draw_hair, NULL);
	private_render (self, &context, liren_draw_transparent, NULL);
	liren_check_errors ();
#ifdef LIREN_ENABLE_PROFILING
	printf ("RENDER PROFILING: objects=%d materials=%d polys=%d verts=%d\n",
		self->render->profiling.objects, self->render->profiling.materials,
		self->render->profiling.faces, self->render->profiling.vertices);
#endif

	/* Deferred lighting. */
	if (framebuffer != NULL)
	{
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, framebuffer->target_fbo);
		glClear (GL_COLOR_BUFFER_BIT);
		glPushAttrib (GL_SCISSOR_BIT);
		private_lighting_render (self, &context, framebuffer);
		glPopAttrib ();
	}
	liren_check_errors ();

	/* Render particles. */
	glDisable (GL_COLOR_MATERIAL);
	glDisable (GL_CULL_FACE);
	glDisable (GL_BLEND);
	glEnable (GL_DEPTH_TEST);
	glDepthMask (GL_FALSE);
	private_particle_render (self);
	private_render (self, &context, liren_draw_debug, NULL);
	liren_check_errors ();
	glDepthMask (GL_TRUE);

	/* Copy scene to screen. */
	if (framebuffer != NULL)
	{
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
		glUseProgramObjectARB (0);
		glClear (GL_COLOR_BUFFER_BIT);
		glDisable (GL_BLEND);
		glDisable (GL_LIGHTING);
		glDisable (GL_CULL_FACE);
		glDisable (GL_DEPTH_TEST);
		glColor3f (1.0f, 1.0f, 1.0f);
		glEnable (GL_TEXTURE_2D);
		glBindTexture (GL_TEXTURE_2D, framebuffer->target_texture);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();
		glPushMatrix ();
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glPushMatrix ();
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
		glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
		glPopMatrix ();
		glPopAttrib ();
	}
	liren_check_errors ();

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
	liren_check_errors ();
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
	LI_FOREACH_U32DIC (iter, self->objects)
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
private_init_lights (LIRenScene* self)
{
	self->lighting = liren_lighting_new (self->render);
	if (self->lighting == NULL)
		return 0;
	return 1;
}

static int
private_init_particles (LIRenScene* self)
{
	self->particles = lipar_manager_new (LIREN_PARTICLE_MAXIMUM_COUNT, LIREN_PARTICLE_MAXIMUM_COUNT);
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
	limat_matrix_project (context->projection, context->modelview, viewport, p, &tmp);
	min = max = tmp;
	for (i = 1 ; i < 8 ; i++)
	{
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
	LI_FOREACH_PTRDIC (iter, self->lighting->lights)
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

	liren_context_unbind (context);
	glDisable (GL_SCISSOR_TEST);

#ifdef DEBUG_LIGHT_BOUNDS
	matrix = limat_matrix_ortho (0.0, framebuffer->width, framebuffer->height, 0.0f, -1.0f, 1.0f);
	liren_context_unbind (context);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (matrix.m);
	glDisable (GL_LIGHTING);
	LI_FOREACH_PTRDIC (iter, self->lighting->lights)
	{
		light = iter.value;
		if (private_light_bounds (self, light, context, viewport, bounds))
		{
			glColor3f (0.3f, 0.0f, 0.0f);
			glBegin (GL_LINE_LOOP);
			glVertex2f (bounds[0], bounds[1]);
			glVertex2f (bounds[0] + bounds[2], bounds[1]);
			glVertex2f (bounds[0] + bounds[2], bounds[1] + bounds[3]);
			glVertex2f (bounds[0], bounds[1] + bounds[3]);
			glEnd ();
		}
	}
#endif
}

static void
private_particle_render (LIRenScene* self)
{
	float color0[4];
	float color1[4];
	float attenuation[] = { 1.0f, 0.0f, 0.02f };
	LIRenImage* image;
	LIParLine* line;
	LIParPoint* particle;

	/* Set particle graphics. */
	image = liren_render_find_image (self->render, "particle-000");
	if (image != NULL)
		glBindTexture (GL_TEXTURE_2D, image->texture->texture);
	if (livid_features.shader_model >= 3)
		glUseProgramObjectARB (0);
	glColor3f (1.0f, 1.0f, 1.0f);
	glEnable (GL_BLEND);

	/* Set point particle rendering mode. */
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
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

	/* Render point particles. */
	glBegin (GL_POINTS);
	for (particle = self->particles->points.used ; particle != NULL ; particle = particle->next)
	{
		lipar_point_get_color (particle, color0);
		glColor4fv (color0);
		glVertex3f (particle->position.x, particle->position.y, particle->position.z);
	}
	glEnd ();

	/* Set normal rendering mode. */
	if (GLEW_ARB_point_sprite)
	{
		glDisable (GL_POINT_SPRITE_ARB);
	}
	glEnable (GL_LIGHTING);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Render line particles. */
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

	LI_FOREACH_U32DIC (iter0, self->objects)
	{
		rndobj = iter0.value;
		if (!liren_object_get_realized (rndobj))
			continue;
		liren_object_get_bounds (rndobj, &aabb);
		if (limat_frustum_cull_aabb (&context->frustum, &aabb))
			continue;
		call (context, rndobj, data);
	}
	LI_FOREACH_PTRDIC (iter1, self->groups)
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

/** @} */
/** @} */
