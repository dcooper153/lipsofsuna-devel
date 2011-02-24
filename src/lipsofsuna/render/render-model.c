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
 * \addtogroup LIRenModel Model
 * @{
 */

#include <lipsofsuna/system.h>
#include "render-model.h"

static void private_clear_materials (
	LIRenModel* self);

static void private_clear_model (
	LIRenModel* self);

static int private_init_materials (
	LIRenModel* self,
	LIMdlModel* model);

static int private_init_model (
	LIRenModel* self,
	LIMdlModel* model);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 * \return New model or NULL.
 */
LIRenModel* liren_model_new (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id)
{
	LIRenModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenModel));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->id = id;

	/* Create model data. */
	if (!liren_model_set_model (self, model))
	{
		lisys_free (self);
		return NULL;
	}

	/* Add to dictionary. */
	if (self->id)
	{
		if (!lialg_u32dic_insert (render->models, id, self))
		{
			private_clear_materials (self);
			private_clear_model (self);
			lisys_free (self);
			return NULL;
		}
	}
	if (!lialg_ptrdic_insert (render->models_ptr, self, self))
	{
		private_clear_materials (self);
		private_clear_model (self);
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees a model.
 *
 * \param self Model.
 */
void
liren_model_free (LIRenModel* self)
{
	/* Remove from dictionary. */
	if (self->id)
		lialg_u32dic_remove (self->render->models, self->id);
	lialg_ptrdic_remove (self->render->models_ptr, self);

	/* Free self. */
	private_clear_materials (self);
	private_clear_model (self);
	lisys_free (self);
}

int liren_model_deform (
	LIRenModel*      self,
	const char*      shader,
	const LIMdlPose* pose)
{
	int i;
	int j;
	int count;
	GLfloat* data;
	LIMdlPoseGroup* group;
	LIRenBufferTexture tmp;
	LIRenContext* context;
	LIRenShader* shader_;

	/* Find the transform feedback shader. */
	shader_ = liren_render_find_shader (self->render, shader);
	if (shader_ == NULL)
		return 0;

	/* Collect pose data. */
	/* The first transformation in the list is the fallback indentity transform
	   referred to by the vertices that don't have all four weights. */
	count = 12 * (pose->groups.count + 1);
	data = lisys_calloc (count, sizeof (GLfloat));
	if (data == NULL)
		return 0;
	j = 0;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 1.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 0.0f;
	data[j++] = 1.0f;
	for (i = 0 ; i < pose->groups.count ; i++)
	{
		group = pose->groups.array + i;
		data[j++] = group->head_rest.x;
		data[j++] = group->head_rest.y;
		data[j++] = group->head_rest.z;
		data[j++] = 0.0;
		data[j++] = group->head_pose.x;
		data[j++] = group->head_pose.y;
		data[j++] = group->head_pose.z;
		data[j++] = group->scale_pose;
		data[j++] = group->rotation.x;
		data[j++] = group->rotation.y;
		data[j++] = group->rotation.z;
		data[j++] = group->rotation.w;
	}

	/* Deform the mesh. */
	context = liren_render_get_context (self->render);
	liren_context_init (context);
	liren_context_set_shader (context, 0, shader_);
	liren_context_bind (context);
	liren_buffer_texture_init (&tmp, data, count * sizeof (GLfloat));
	lisys_free (data);
	glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_BUFFER_TEXTURE);
	glBindTexture (GL_TEXTURE_BUFFER, tmp.texture);
	liren_mesh_deform (&self->mesh);
	liren_buffer_texture_clear (&tmp);

	return 1;
}

/**
 * \brief Calculates the first intersection of the model and a ray.
 * \param self Model.
 * \param ray0 Start point of the ray, in model space.
 * \param ray1 End point of the ray, in model space.
 * \param result Return location for the intersection point.
 * \return Nonzero if an intersection was found, zero if not.
 */
int liren_model_intersect_ray (
	const LIRenModel*  self,
	const LIMatVector* ray0,
	const LIMatVector* ray1,
	LIMatVector*       result)
{
	int i;
	int found = 0;
	float d;
	float best_dist = 0.0f;
	void* vtxdata;
	LIMatTriangle triangle;
	LIMatVector best_point = { 0.0f, 0.0f, 0.0f };
	LIMatVector p;
	LIRenFormat format;

	/* Fast exit if no intersection with the bounding box. */
	if (!limat_intersect_aabb_line_near (&self->bounds, ray0, ray1, &p))
		return 0;

	/* Test for intersection for each face in the model. */
	liren_mesh_get_format (&self->mesh, &format);
	vtxdata = liren_mesh_lock_vertices (&self->mesh, 0, self->mesh.counts[2]);
	if (vtxdata == NULL)
		return 0;
	for (i = 0 ; i < self->mesh.counts[2] ; i += 3)
	{
		/* Get the next triangle. */
		limat_triangle_set_from_points (&triangle,
			(LIMatVector*)(vtxdata + format.vtx_offset + format.size * (i + 0)),
			(LIMatVector*)(vtxdata + format.vtx_offset + format.size * (i + 1)),
			(LIMatVector*)(vtxdata + format.vtx_offset + format.size * (i + 2)));

		/* Check for an intersection. */
		if (limat_triangle_intersects_ray (&triangle, ray0, ray1, &p))
		{
			d = limat_vector_get_length (limat_vector_subtract (p, *ray0));
			if (!found || d < best_dist)
			{
				best_dist = d;
				best_point = p;
				found = 1;
			}
		}
	}
	liren_mesh_unlock_vertices (&self->mesh);
	if (found)
		*result = best_point;

	return found;
}

void
liren_model_replace_image (LIRenModel* self,
                           LIRenImage* image)
{
	int i;
	int j;
	LIRenMaterial* material;
	LIRenTexture* texture;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.array[i];
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			if (texture->image == image)
				liren_texture_set_image (texture, image);
		}
	}
}

/**
 * \brief Caches transparent faces for fast depth sorting.
 *
 * Recalculates the center points of transparent triangles after an animation
 * has deformed the mesh. For maximum correctness of transparency, the function
 * should be called every time the mesh is deformed, but since the update
 * involves an expensive download from video memory, the user might choose to
 * trade transparency quality for better performance.
 *
 * \param self Model.
 */
void liren_model_update_transparency (
	LIRenModel* self)
{
	int i;
	int j;
	void* vtxdata;
	LIRenFormat format;
	LIRenModelGroup* group;
	LIRenMaterial* material;

	/* Update each material group. */
	for (i = 0 ; i < self->groups.count ; i++)
	{
		/* Check if transparency center is needed. */
		group = self->groups.array + i;
		group->center = limat_vector_init (0.0f, 0.0f, 0.0f);
		material = self->materials.array[i];
		if (material->shader == NULL)
			continue;
		if (!material->shader->sort)
			continue;

		/* Download the deformed vertices from video memory. */
		liren_mesh_get_format (&self->mesh, &format);
		vtxdata = liren_mesh_lock_vertices (&self->mesh, group->start, group->count);
		if (vtxdata == NULL)
			continue;

		/* Calculate the center of the group. */
		if (group->count > 0)
		{
			for (j = 0 ; j < group->count ; j++)
			{
				group->center = limat_vector_add (group->center,
					*((LIMatVector*)(vtxdata + format.vtx_offset + format.size * j)));
			}
			group->center = limat_vector_multiply (group->center, 1.0f / group->count);
		}

		/* Unmap the deformed vertices. */
		liren_mesh_unlock_vertices (&self->mesh);
	}
}

void liren_model_get_bounds (
	LIRenModel* self,
	LIMatAabb*  aabb)
{
	*aabb = self->bounds;
}

int liren_model_set_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	LIAlgPtrdicIter iter0;
	LIAlgU32dicIter iter1;
	LIRenModel backup;
	LIRenObject* object;
	LIRenScene* scene;

	/* Create new model data and erase the old data. */
	backup = *self;
	self->bounds = model->bounds;
	if (!private_init_materials (self, model) ||
	    !private_init_model (self, model))
	{
		private_clear_materials (self);
		private_clear_model (self);
		*self = backup;
		return 0;
	}
	private_clear_materials (&backup);
	private_clear_model (&backup);

	/* We need to refresh any objects using the model since lights reference
	   the nodes of the model directly and those might have changed. */
	LIALG_PTRDIC_FOREACH (iter0, self->render->scenes)
	{
		scene = iter0.value;
		LIALG_U32DIC_FOREACH (iter1, scene->objects)
		{
			object = iter1.value;
			if (object->model == self)
				liren_object_set_model (object, self);
		}
	}

	return 1;
}

/*****************************************************************************/

static void private_clear_materials (
	LIRenModel* self)
{
	int i;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		if (self->materials.array[i] != NULL)
			liren_material_free (self->materials.array[i]);
	}
	lisys_free (self->materials.array);
	self->materials.array = NULL;
	self->materials.count = 0;
}

static void private_clear_model (
	LIRenModel* self)
{
	liren_particles_clear (&self->particles);
	liren_mesh_clear (&self->mesh);
	lisys_free (self->groups.array);
	self->groups.array = NULL;
	self->groups.count = 0;
}

static int private_init_materials (
	LIRenModel* self,
	LIMdlModel* model)
{
	uint32_t i;
	LIMdlMaterial* src;
	LIRenMaterial* dst;

	/* Allocate materials. */
	self->materials.count = model->materials.count;
	if (self->materials.count)
	{
		self->materials.array = lisys_calloc (self->materials.count, sizeof (LIRenMaterial*));
		if (self->materials.array == NULL)
			return 0;
	}

	/* Resolve materials. */
	for (i = 0 ; i < self->materials.count ; i++)
	{
		src = model->materials.array + i;
		dst = liren_material_new_from_model (self->render, src);
		if (dst == NULL)
			return 0;
		self->materials.array[i] = dst;
	}

	return 1;
}

static int private_init_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	int c;
	int i;
	int ok;
	void* vertices;
	uint32_t* indices;
	GLint restore;
	LIMdlFaces* group;
	LIMdlPose* pose;

	/* Allocate face groups. */
	self->groups.count = model->facegroups.count;
	if (self->groups.count)
	{
		self->groups.array = lisys_calloc (self->groups.count, sizeof (LIRenModelGroup));
		if (self->groups.array == NULL)
			return 0;
	}

	/* Calculate face group offsets. */
	for (c = i = 0 ; i < self->groups.count ; i++)
	{
		group = model->facegroups.array + i;
		self->groups.array[i].start = c;
		self->groups.array[i].count = group->indices.count;
		c += group->indices.count;
	}

	/* Combine the index lists. */
	if (c)
	{
		indices = lisys_calloc (c, sizeof (uint32_t));
		if (indices == NULL)
			return 0;
		for (c = i = 0 ; i < self->groups.count ; i++)
		{
			group = model->facegroups.array + i;
			memcpy (indices + c, group->indices.array, group->indices.count * sizeof (uint32_t));
			c += group->indices.count;
		}
	}
	else
		indices = NULL;

	/* Initialize particles. */
	liren_particles_init (&self->particles, self->render, model);
	if (self->particles.frames.count)
		self->bounds = limat_aabb_union (self->bounds, self->particles.bounds);

	/* Initialize the render buffer. */
	if (!liren_mesh_init (&self->mesh, indices, c,
	     model->vertices.array, model->vertices.count))
	{
		lisys_free (indices);
		return 0;
	}
	lisys_free (indices);

	/* Transform the default pose. */
	pose = limdl_pose_new ();
	if (pose != NULL)
	{
		ok = limdl_pose_set_model (pose, model) &&
		     liren_model_deform (self, "skeletal", pose);
		limdl_pose_free (pose);
	}
	else
		ok = 0;

	/* If transforming the default pose failed, our transform feedback buffer
	   is full of random data since we never initialized it. To avoid an ugly
	   polygon mess when the shader is missing, we zero the buffer as a fallback. */
	if (!ok && self->mesh.sizes[2])
	{
		glGetIntegerv (GL_VERTEX_ARRAY_BINDING, &restore);
		glBindVertexArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, self->mesh.buffers[2]);
		vertices = glMapBuffer (GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (vertices != NULL)
		{
			memset (vertices, 0, self->mesh.sizes[2]);
			glUnmapBuffer (GL_ARRAY_BUFFER);
		}
		glBindVertexArray (restore);
	}

	/* Initialize face sorting for transparent material groups. */
	liren_model_update_transparency (self);

	return 1;
}

/** @} */
/** @} */
