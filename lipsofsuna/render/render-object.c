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
 * \addtogroup lirndObject Object
 * @{
 */

#include <network/lips-network.h>
#include <system/lips-system.h>
#include "render-draw.h"
#include "render-object.h"

static void
private_clear_envmap (lirndObject* self);

static void
private_clear_lights (lirndObject* self);

static int
private_init_envmap (lirndObject* self);

static int
private_init_lights (lirndObject* self,
                     limdlPose*   pose);

static void
private_update_envmap (lirndObject* self);

static void
private_update_lights (lirndObject* self);

/*****************************************************************************/

/**
 * \brief Creates a new render object and adds it to the scene.
 *
 * \param scene Scene.
 * \param id Unique identifier.
 * \return New object or NULL.
 */
lirndObject*
lirnd_object_new (lirndScene* scene,
                  int         id)
{
	lirndObject* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndObject));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	self->transform = limat_transform_identity ();
	self->orientation.matrix = limat_matrix_identity ();
	limat_aabb_init (&self->aabb);

	/* Choose unique ID. */
	while (!id)
	{
		id = (int)((LINET_RANGE_RENDER_END - LINET_RANGE_RENDER_START) *
			((float) rand () / RAND_MAX)) + LINET_RANGE_RENDER_START;
		if (lirnd_scene_find_object (scene, id))
			id = 0;
	}
	self->id = id;
	assert (!lirnd_scene_find_object (scene, id));

	/* Add to renderer. */
	if (!lialg_u32dic_insert (scene->objects, id, self))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the render object.
 *
 * The object is removed from its renderer and its memory is freed.
 *
 * \param self Object.
 */
void
lirnd_object_free (lirndObject* self)
{
	lialg_u32dic_remove (self->scene->objects, self->id);
	private_clear_envmap (self);
	private_clear_lights (self);
	if (self->instance != NULL)
		lirnd_model_free (self->instance);
	lisys_free (self);
}

/**
 * \brief Deforms the object.
 *
 * \param self Object.
 */
void
lirnd_object_deform (lirndObject* self)
{
	int i;
	void* vertices;
	lirndBuffer* buffer;

	if (self->instance == NULL)
		return;
	for (i = 0 ; i < self->instance->buffers.count ; i++)
	{
		buffer = self->instance->buffers.array + i;
		vertices = lirnd_buffer_lock (buffer, 1);
		if (vertices != NULL)
		{
			limdl_pose_transform_group (self->pose, i, vertices);
			lirnd_buffer_unlock (buffer, vertices);
		}
	}
	private_update_lights (self);
}

void
lirnd_object_emit_particles (lirndObject* self)
{
#warning FIXME: Emitting particles is disabled
#if 0
	int i;
	limatMatrix vtxmat;
	limatMatrix nmlmat;
	limatVector position;
	limatVector velocity;
	limdlVertex* vertex;
	limdlVertex* vertices;
	lirndParticle* particle;

	if (self->model == NULL)
		return;

	vtxmat = self->orientation.matrix;
	nmlmat = limat_matrix_get_rotation (vtxmat);
	if (self->vertices != NULL)
		vertices = self->vertices;
	else
		vertices = self->model->model->vertex.vertices;

	for (i = 0 ; i < self->model->model->vertex.count ; i++)
	{
		vertex = vertices + i;
		position = limat_matrix_transform (vtxmat, vertex->coord);
		velocity = limat_matrix_transform (nmlmat, vertex->normal);
		velocity = limat_vector_multiply (velocity, 3.0f/*0.2f*/);
		/*
			random()/(0.5*RAND_MAX)-1.0,
			random()/(3.0*RAND_MAX)+3.0f,
			random()/(0.5*RAND_MAX)-1.0
		*/
		particle = lirnd_scene_insert_particle (self->scene, &position, &velocity);
		if (particle != NULL)
		{
			particle->time_life = 2.0f;
			particle->time_fade = 1.0f;
			particle->acceleration = limat_vector_init (0.0, -100.0, 5.0);
		}
	}
#endif
}

/**
 * \brief Advances the timer of the object and deforms its mesh.
 *
 * \param self Object.
 * \param secs Number of seconds since the last update.
 */
void
lirnd_object_update (lirndObject* self,
                     float        secs)
{
	if (self->instance != NULL && self->instance->buffers.count)
	{
		private_update_envmap (self);
		if (self->pose != NULL)
			lirnd_object_deform (self);
	}
}

/**
 * \brief Gets the bounding box of the object.
 *
 * \param self Object.
 * \param result Return location for the bounding box.
 */
void
lirnd_object_get_bounds (const lirndObject* self,
                         limatAabb*         result)
{
	*result = limat_aabb_transform (self->aabb, &self->orientation.matrix);
}

/**
 * \brief Gets the center position of the object.
 *
 * The returned position depends on the current matrix.
 *
 * \param self Object.
 * \param center Return location for the position.
 */
void
lirnd_object_get_center (const lirndObject* self,
                         limatVector*       center)
{
	*center = self->orientation.center;
}

/**
 * \brief Sets the model of the object.
 *
 * Changes the model of the object and takes care of automatically registering
 * and unregistering lights to match the new apperance.
 *
 * \param self Object.
 * \param model Model.
 * \return Nonzero on success.
 */
int
lirnd_object_set_model (lirndObject* self,
                        lirndModel*  model)
{
	/* Replace model. */
	self->model = model;

	/* Replace instance. */
	if (self->instance != NULL)
		lirnd_model_free (self->instance);
	if (model != NULL && self->pose != NULL)
		self->instance = lirnd_model_new_instance (model);
	else
		self->instance = NULL;

	/* Update bounds. */
	if (model != NULL)
		self->aabb = model->aabb;
	else
		limat_aabb_init (&self->aabb);

	/* Replace lights and environment map. */
	private_clear_lights (self);
	private_clear_envmap (self);
	private_init_lights (self, self->pose);
	private_init_envmap (self);

	return 1;
}

/**
 * \brief Sets the pose of the object.
 *
 * \param self Object.
 * \param pose Pose.
 * \return Nonzero on success.
 */
int
lirnd_object_set_pose (lirndObject* self,
                       limdlPose*   pose)
{
	if (self->pose != pose)
	{
		self->pose = pose;
		return lirnd_object_set_model (self, self->model);
	}

	return 1;
}

/**
 * \brief Checks if the object is renderable.
 *
 * Returns nonzero if the realized flag of the object is set and the object
 * has a model assigned to it. Otherwise returns zero.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int
lirnd_object_get_realized (const lirndObject* self)
{
	return self->realized && (self->model != NULL || self->instance != NULL);
}

/**
 * \brief Sets the realized flag of the object.
 *
 * If the flag is set and the object has a model set, the model will be rendered
 * in subsequent frames. The light sources that are a part of the model are also
 * added to the scene.
 *
 * If the flag is cleared, the model assigned to the object is not rendered from
 * now on. The light sources associated with the model are also removed from the
 * scene.
 *
 * \param self Object.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int
lirnd_object_set_realized (lirndObject* self,
                           int          value)
{
	int i;

	if (self->realized == value)
		return 1;
	self->realized = value;
	if (value && self->model != NULL)
	{
		for (i = 0 ; i < self->lights.count ; i++)
		{
			if (self->lights.array[i] != NULL)
				lirnd_lighting_insert_light (self->scene->lighting, self->lights.array[i]);
		}
	}
	else
	{
		for (i = 0 ; i < self->lights.count ; i++)
		{
			if (self->lights.array[i] != NULL)
				lirnd_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
		}
	}

	return 1;
}

/**
 * \brief Gets the transformation of the object.
 *
 * \param self Object.
 * \param value Return location for the transformation.
 */
void
lirnd_object_get_transform (lirndObject*    self,
                            limatTransform* value)
{
	*value = self->transform;
}

/**
 * \brief Sets the transformation of the object.
 *
 * \param self Object.
 * \param value Transformation.
 */
void
lirnd_object_set_transform (lirndObject*          self,
                            const limatTransform* value)
{
	limatVector center;
	limdlModel* model;

	/* Set transformation. */
	self->transform = *value;
	self->orientation.matrix = limat_convert_transform_to_matrix (*value);

	/* Set box center. */
	if (self->model != NULL)
	{
		/* FIXME: Incorrect for rotated objects. */
		model = self->model->model;
		center = limat_vector_add (model->bounds.min, model->bounds.max);
		center = limat_vector_multiply (center, 0.5f);
		center = limat_vector_add (center, value->position);
		self->orientation.center = center;
	}
	else
		self->orientation.center = value->position;
}

void*
lirnd_object_get_userdata (const lirndObject* self)
{
	return self->userdata;
}

void
lirnd_object_set_userdata (lirndObject* self,
                           void*        value)
{
	self->userdata = value;
}

/*****************************************************************************/

static void
private_clear_envmap (lirndObject* self)
{
	int i;
	int j;
	lirndMaterial* material;
	lirndTexture* texture;

	if (self->instance == NULL)
		return;
	glDeleteFramebuffersEXT (6, self->cubemap.fbo);
	glDeleteTextures (1, &self->cubemap.depth);
	glDeleteTextures (1, &self->cubemap.map);
	memset (self->cubemap.fbo, 0, 6 * sizeof (GLuint));
	self->cubemap.depth = 0;
	self->cubemap.map = 0;
	for (i = 0 ; i < self->instance->materials.count ; i++)
	{
		material = self->instance->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			if (texture->type == LIMDL_TEXTURE_TYPE_ENVMAP)
				texture->texture = 0;
		}
	}
}

static void
private_clear_lights (lirndObject* self)
{
	int i;

	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->lights.array[i] != NULL)
		{
			lirnd_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
			lirnd_light_free (self->lights.array[i]);
		}
	}
	lisys_free (self->lights.array);
	self->lights.array = NULL;
	self->lights.count = 0;
}

static int
private_init_envmap (lirndObject* self)
{
	int i;
	int j;
	int width = 0;
	int height = 0;
	lirndMaterial* material;
	lirndTexture* texture;

	/* Check for requirements. */
	if (!GLEW_ARB_depth_texture ||
	    !GLEW_ARB_texture_cube_map ||
	    !GLEW_EXT_framebuffer_object)
		return 1;

	/* Check if needed by textures. */
	if (self->instance == NULL)
		return 1;
	for (i = 0 ; i < self->instance->materials.count ; i++)
	{
		material = self->instance->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			if (texture->type == LIMDL_TEXTURE_TYPE_ENVMAP)
			{
				width = LI_MAX (width, texture->width);
				height = LI_MAX (height, texture->height);
			}
		}
	}
	if (width < 2 || height < 2)
		return 1;

	/* Create depth texture. */
	glGenTextures (1, &self->cubemap.depth);
	glBindTexture (GL_TEXTURE_2D, self->cubemap.depth);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* Create cubemap texture. */
	glGenTextures (1, &self->cubemap.map);
	glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, self->cubemap.map);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (i = 0 ; i < 6 ; i++)
	{
		glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	/* Create framebuffer objects. */
	glGenFramebuffersEXT (6, self->cubemap.fbo);
	for (i = 0 ; i < 6 ; i++)
	{
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->cubemap.fbo[i]);
		glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, self->cubemap.map, 0);
		glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_TEXTURE_2D, self->cubemap.depth, 0);
		glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
		switch (glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT))
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				lisys_error_set (EINVAL, "incomplete framebuffer attachment");
				goto error;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				lisys_error_set (EINVAL, "incomplete framebuffer dimensions");
				goto error;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				lisys_error_set (EINVAL, "missing framebuffer attachment");
				goto error;
			default:
				lisys_error_set (ENOTSUP, "unsupported framebuffer format");
				goto error;
		}
	}
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

	/* Setup virtual image to make binding easier. */
	self->cubemap.width = width;
	self->cubemap.height = height;

	/* Bind it to environment map textures. */
	for (i = 0 ; i < self->instance->materials.count ; i++)
	{
		material = self->instance->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			if (texture->type == LIMDL_TEXTURE_TYPE_ENVMAP)
				texture->texture = self->cubemap.map;
		}
	}

	return 1;

error:
	lisys_error_report ();
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT (6, self->cubemap.fbo);
	glDeleteTextures (1, &self->cubemap.depth);
	glDeleteTextures (1, &self->cubemap.map);
	memset (self->cubemap.fbo, 0, 6 * sizeof (GLuint));
	self->cubemap.depth = 0;
	self->cubemap.map = 0;
	return 1;
}

int
private_init_lights (lirndObject* self,
                     limdlPose*   pose)
{
	int i;
	limdlNode* node;
	limdlNodeIter iter;
	lirndLight* light;

	/* Create light sources. */
	if (pose != NULL)
	{
		LIMDL_FOREACH_NODE (iter, &pose->nodes)
		{
			node = iter.value;
			if (node->type != LIMDL_NODE_LIGHT)
				continue;
			light = lirnd_light_new_from_model (self->scene, node);
			if (light == NULL)
				return 0;
			if (!lialg_array_append (&self->lights, &light))
			{
				lirnd_light_free (light);
				return 0;
			}
		}
	}

	/* Register light sources. */
	if (self->realized)
	{
		for (i = 0 ; i < self->lights.count ; i++)
		{
			light = self->lights.array[i];
			if (!lirnd_lighting_insert_light (self->scene->lighting, light))
			{
				while (i--)
				{
					light = self->lights.array[i];
					lirnd_lighting_remove_light (self->scene->lighting, light);
				}
				return 0;
			}
		}
	}

	return 1;
}

static void
private_update_envmap (lirndObject* self)
{
	int i;
	lialgU32dicIter iter;
	limatFrustum frustum;
	limatVector ctr;
	limatMatrix modelview;
	limatMatrix projection;
	lirndContext context;
	const limatVector dir[6] =
	{
		{ 1.0f, 0.0f, 0.0f }, /* Back. */
		{ -1.0f, 0.0f, 0.0f }, /* Front. */
		{ 0.0f, 1.0f, 0.0f }, /* Up. */
		{ 0.0f, -1.0f, 0.0f }, /* Down. */
		{ 0.0f, 0.0f, 1.0f }, /* Left. */
		{ 0.0f, 0.0f, -1.0f } /* Right. */
	};
	const limatVector up[6] =
	{
		{ 0.0f, -1.0f, 0.0f }, /* Back. */
		{ 0.0f, -1.0f, 0.0f }, /* Front. */
		{ 0.0f, 0.0f, 1.0f }, /* Up. */
		{ 0.0f, 0.0f, -1.0f }, /* Down. */
		{ 0.0f, -1.0f, 0.0f }, /* Left. */
		{ 0.0f, -1.0f, 0.0f } /* Right. */
	};

	if (!self->cubemap.map || !self->scene->render->shader.enabled)
		return;
	modelview = self->orientation.matrix;
	projection = limat_matrix_perspective (0.5 * M_PI, 1.0f, 1.0f, 100.0f);
	lirnd_object_get_center (self, &ctr);

	/* Enable cube map rendering mode. */
	glPushAttrib (GL_VIEWPORT_BIT);
	glViewport (0, 0, self->cubemap.width, self->cubemap.height);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glFrontFace (GL_CCW);
	glDepthFunc (GL_LEQUAL);
	glBindTexture (GL_TEXTURE_2D, 0);

	/* Render each cube face. */
	for (i = 0 ; i < 6 ; i++)
	{
		modelview = limat_matrix_look (
			ctr.x, ctr.y, ctr.z,
			dir[i].x, dir[i].y, dir[i].z,
			up[i].x, up[i].y, up[i].z);
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, self->cubemap.fbo[i]);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		limat_frustum_init (&frustum, &modelview, &projection);
		lirnd_context_init (&context, self->scene);
		lirnd_context_set_modelview (&context, &modelview);
		lirnd_context_set_projection (&context, &projection);
		lirnd_context_set_frustum (&context, &frustum);
		LI_FOREACH_U32DIC (iter, self->scene->objects)
			lirnd_draw_exclude (&context, iter.value, self);
	}

	/* Disable cube map rendering mode. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib ();
}

static void
private_update_lights (lirndObject* self)
{
	int i;
	limatTransform transform;
	lirndLight* light;

	for (i = 0 ; i < self->lights.count ; i++)
	{
		light = self->lights.array[i];
		if (light->node != NULL)
		{
			limdl_node_get_world_transform (light->node, &transform);
			transform = limat_transform_multiply (self->transform, transform);
			lirnd_light_set_transform (light, &transform);
		}
	}
}

/** @} */
/** @} */
