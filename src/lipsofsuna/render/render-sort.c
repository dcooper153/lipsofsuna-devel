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
 * \addtogroup LIRenSort Sort
 * @{
 */

#include "render-sort.h"

static int private_resize_faces (
	LIRenSort* self,
	int        count);

/*****************************************************************************/

LIRenSort* liren_sort_new (
	LIRenRender* render)
{
	LIRenSort* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenSort));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Allocate buffer for opaque material groups. */
	self->groups.capacity = 1024;
	self->groups.array = lisys_calloc (self->groups.capacity, sizeof (LIRenSortgroup));
	if (self->groups.array == NULL)
	{
		liren_sort_free (self);
		return NULL;
	}

	/* Allocate depth sort buckets for transparent faces. */
	self->buckets.count = 4096;
	self->buckets.array = lisys_calloc (self->buckets.count, sizeof (LIRenSortface*));
	if (self->buckets.array == NULL)
	{
		liren_sort_free (self);
		return NULL;
	}

	/* Allocate buffer for transparent faces. */
	self->faces.capacity = 1024;
	self->faces.array = lisys_calloc (self->faces.capacity, sizeof (LIRenSortface));
	if (self->faces.array == NULL)
	{
		liren_sort_free (self);
		return NULL;
	}

	return self;
}

void liren_sort_free (
	LIRenSort* self)
{
	lisys_free (self->groups.array);
	lisys_free (self->buckets.array);
	lisys_free (self->faces.array);
	lisys_free (self);
}

int liren_sort_add_group (
	LIRenSort*     self,
	LIMatAabb*     bounds,
	LIMatMatrix*   matrix,
	int            index,
	int            count,
	LIRenBuffer*   buffer,
	LIRenMaterial* material,
	int            transparent)
{
	int num;
	LIRenSortgroup* tmp;

	/* Resize the buffer if necessary. */
	if (self->groups.capacity == self->groups.count)
	{
		num = self->groups.capacity << 1; 
		tmp = lisys_realloc (self->groups.array, num * sizeof (LIRenSortgroup));
		if (tmp == NULL)
			return 0;
		self->groups.array = tmp;
		self->groups.capacity = num;
	}

	/* Append the group to the buffer. */
	num = self->groups.count;
	self->groups.array[num].index = index;
	self->groups.array[num].count = count;
	self->groups.array[num].transparent = transparent;
	self->groups.array[num].bounds = *bounds;
	self->groups.array[num].matrix = *matrix;
	self->groups.array[num].buffer = buffer;
	self->groups.array[num].material = material;
	self->groups.count++;

	if (transparent)
		return liren_sort_add_faces (self, bounds, matrix, index, count, buffer, material);

	return 1;
}

int liren_sort_add_faces (
	LIRenSort*     self,
	LIMatAabb*     bounds,
	LIMatMatrix*   matrix,
	int            index,
	int            count,
	LIRenBuffer*   buffer,
	LIRenMaterial* material)
{
	int i;
	int num;
	int bucket;
	float dist;
	void* vtxdata;
	uint32_t* idxdata;
	LIMatMatrix mat;
	LIMatVector vtx[3];
	LIMatVector center;
	LIMatVector diff;
	LIMatVector eye;
	LIRenFormat* fmt;

	/* Resize the buffer if necessary. */
	if (!private_resize_faces (self, self->faces.count + count / 3))
		return 0;

	/* Calculate camera position. */
	mat = limat_matrix_invert (self->modelview);
	eye = limat_matrix_transform (mat, limat_vector_init (0.0f, 0.0f, 0.0f));

	/* Add each face in the group. */
	num = self->faces.count;
	fmt = &buffer->vertex_format;
	vtxdata = liren_buffer_lock_vertices (buffer, 0);
	idxdata = liren_buffer_lock_indices (buffer, 0);
	for (i = 0 ; i < count ; i += 3, num++)
	{
		/* Append the face to the buffer. */
		self->faces.array[num].type = LIREN_SORT_TYPE_FACE;
		self->faces.array[num].face.index = index + i;
		self->faces.array[num].face.bounds = *bounds;
		self->faces.array[num].face.matrix = *matrix;
		self->faces.array[num].face.buffer = buffer;
		self->faces.array[num].face.material = material;

		/* Calculate the center of the triangle. */
		vtx[0] = *((LIMatVector*)(vtxdata + fmt->vtx_offset + fmt->size * idxdata[i + 0]));
		vtx[1] = *((LIMatVector*)(vtxdata + fmt->vtx_offset + fmt->size * idxdata[i + 1]));
		vtx[2] = *((LIMatVector*)(vtxdata + fmt->vtx_offset + fmt->size * idxdata[i + 2]));
		center = limat_vector_add (limat_vector_add (vtx[0], vtx[1]), vtx[2]);
		center = limat_vector_multiply (center, 1.0f / 3.0f);
		center = limat_matrix_transform (*matrix, center);

		/* Calculate bucket index based on distance to camera. */
		/* TODO: Would be better to use far plane distance here? */
		/* TODO: Non-linear mapping might work better for details closer to the camera? */
		diff = limat_vector_subtract (center, eye);
		dist = limat_vector_get_length (diff);
		bucket = dist / 50.0f * (self->buckets.count - 1);
		bucket = LIMAT_CLAMP (bucket, 0, self->buckets.count - 1);

		/* Insert to the depth bucket. */
		self->faces.array[num].next = self->buckets.array[bucket];
		self->buckets.array[bucket] = self->faces.array + num;
	}
	liren_buffer_unlock_indices (buffer, idxdata);
	liren_buffer_unlock_vertices (buffer, vtxdata);
	self->faces.count = num;

	return 1;
}

int liren_sort_add_model (
	LIRenSort*   self,
	LIMatAabb*   bounds,
	LIMatMatrix* matrix,
	LIRenModel*  model)
{
	int i;
	int ret = 1;
	int transp;
	LIRenMaterial* material;

	/* Frustum culling. */
	if (limat_frustum_cull_aabb (&self->frustum, bounds))
		return 1;

	/* Add each material group. */
	for (i = 0 ; i < model->materials.count ; i++)
	{
		material = model->materials.array[i];
		transp = (material->flags & LIREN_MATERIAL_FLAG_TRANSPARENCY);
		ret &= liren_sort_add_group (self, bounds, matrix,
			model->groups.array[i].start, model->groups.array[i].count,
			model->buffer, material, transp);
	}

	return ret;
}

int liren_sort_add_object (
	LIRenSort*   self,
	LIRenObject* object)
{
	int i;
	int j;
	float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	LIMatAabb bounds;
	LIMatVector position;
	LIMdlModel* model;
	LIMdlParticle* part;
	LIMdlParticleSystem* system;
	LIRenImage* image;
	LIRenShader* shader;

	/* Add each face group of the model. */
	liren_object_get_bounds (object, &bounds);
	if (!liren_sort_add_model (self, &bounds, &object->orientation.matrix, object->model))
		return 0;

	/* TODO: Frustum culling for particles. */

	/* Check for particles. */
	model = object->model->model;
	if (!model->particlesystems.count)
		return 1;
	shader = liren_render_find_shader (self->render, "particle");
	if (shader == NULL)
		return 1;

	/* Add each particle system. */
	for (i = 0 ; i < model->particlesystems.count ; i++)
	{
		/* Find texture image of the system. */
		system = model->particlesystems.array + i;
		image = liren_render_find_image (self->render, system->texture);
		if (image == NULL)
		{
			liren_render_load_image (self->render, system->texture);
			image = liren_render_find_image (self->render, system->texture);
			if (image == NULL)
				continue;
		}

		/* Add each alive particle in the system. */
		for (j = 0 ; j < system->particles.count ; j++)
		{
			part = system->particles.array + j;
			if (limdl_particle_get_state (part, object->particle.time, object->particle.loop, &position, diffuse + 3))
			{
				position = limat_transform_transform (object->transform, position);
				liren_sort_add_particle (self, &position, system->particle_size, diffuse, image, shader);
			}
		}
	}

	return 1;
}

int liren_sort_add_particle (
	LIRenSort*         self,
	const LIMatVector* position,
	float              size,
	const float*       diffuse,
	LIRenImage*        image,
	LIRenShader*       shader)
{
	int num;
	int bucket;
	float dist;
	LIMatMatrix mat;
	LIMatVector diff;
	LIMatVector eye;

	/* Resize the buffer if necessary. */
	if (!private_resize_faces (self, self->faces.count + 1))
		return 0;

	/* Calculate camera position. */
	mat = limat_matrix_invert (self->modelview);
	eye = limat_matrix_transform (mat, limat_vector_init (0.0f, 0.0f, 0.0f));

	/* Append the particle to the buffer. */
	num = self->faces.count;
	self->faces.array[num].type = LIREN_SORT_TYPE_PARTICLE;
	self->faces.array[num].particle.position = *position;
	self->faces.array[num].particle.size = size;
	self->faces.array[num].particle.image = image;
	self->faces.array[num].particle.shader = shader;
	memcpy (self->faces.array[num].particle.diffuse, diffuse, 4 * sizeof (float));

	/* Calculate bucket index based on distance to camera. */
	/* TODO: Would be better to use far plane distance here? */
	/* TODO: Non-linear mapping might work better for details closer to the camera? */
	diff = limat_vector_subtract (*position, eye);
	dist = limat_vector_get_length (diff);
	bucket = dist / 50.0f * (self->buckets.count - 1);
	bucket = LIMAT_CLAMP (bucket, 0, self->buckets.count - 1);

	/* Insert to the depth bucket. */
	self->faces.array[num].next = self->buckets.array[bucket];
	self->buckets.array[bucket] = self->faces.array + num;
	self->faces.count = ++num;

	return 1;
}

void liren_sort_clear (
	LIRenSort*         self,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection)
{
	self->groups.count = 0;
	self->faces.count = 0;
	memset (self->buckets.array, 0, self->buckets.count * sizeof (LIRenSortface*));
	self->modelview = *modelview;
	self->projection = *projection;
	limat_frustum_init (&self->frustum, modelview, projection);
}

/*****************************************************************************/

static int private_resize_faces (
	LIRenSort* self,
	int        count)
{
	int i;
	int num;
	LIRenSortface* ptr;
	LIRenSortface* tmp;

	if (self->faces.capacity > count)
		return 1;

	/* Reallocate the face array. */
	num = self->faces.capacity << 1;
	while (num < count)
		num <<= 1;
	tmp = lisys_realloc (self->faces.array, num * sizeof (LIRenSortface));
	if (tmp == NULL)
		return 0;

	/* If the address of the face buffer changed, we need to adjust
	   the pointers in the buckets to match the new address. */
	if (tmp != self->faces.array)
	{
		for (i = 0 ; i < self->buckets.count ; i++)
		{
			if (self->buckets.array[i] != NULL)
			{
				self->buckets.array[i] = self->buckets.array[i] - self->faces.array + tmp;
				for (ptr = self->buckets.array[i] ; ptr != NULL ; ptr = ptr->next)
				{
					if (ptr->next)
						ptr->next = ptr->next - self->faces.array + tmp;
				}
			}
		}
	}

	/* Use the new face array. */
	self->faces.array = tmp;
	self->faces.capacity = num;

	return 1;
}

/** @} */
/** @} */
