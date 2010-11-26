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
	LIRenMesh*     mesh,
	LIRenMaterial* material,
	LIMatVector*   face_sort_coords)
{
	int num;
	LIRenSortgroup* tmp;

	/* Don't add groups with no valid shader. */
	if (material->shader == NULL)
		return 1;

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
	self->groups.array[num].transparent = (face_sort_coords != NULL);
	self->groups.array[num].bounds = *bounds;
	self->groups.array[num].matrix = *matrix;
	self->groups.array[num].mesh = mesh;
	self->groups.array[num].material = material;
	self->groups.count++;

	/* Depth sort transparent groups. */
	if (face_sort_coords != NULL)
		return liren_sort_add_faces (self, bounds, matrix, index, count, mesh, material, face_sort_coords);

	return 1;
}

int liren_sort_add_faces (
	LIRenSort*     self,
	LIMatAabb*     bounds,
	LIMatMatrix*   matrix,
	int            index,
	int            count,
	LIRenMesh*     mesh,
	LIRenMaterial* material,
	LIMatVector*   face_sort_coords)
{
	int i;
	int n;
	int bucket;
	float dist;
	LIMatMatrix mat;
	LIMatVector center;
	LIMatVector diff;
	LIMatVector eye;

	/* Calculate camera position. */
	mat = limat_matrix_invert (self->modelview);
	eye = limat_matrix_transform (mat, limat_vector_init (0.0f, 0.0f, 0.0f));

	/* Add the whole group or individual faces depending on material settings. */
	if (material->flags & LIREN_MATERIAL_FLAG_SORTFACES)
	{
		/* Resize the buffer if necessary. */
		if (!private_resize_faces (self, self->faces.count + count / 3))
			return 0;

		/* Add each face of the group. */
		for (i = 0, n = self->faces.count ; i < count / 3 ; i++, n++)
		{
			/* Append the face to the buffer. */
			self->faces.array[n].type = LIREN_SORT_TYPE_FACE;
			self->faces.array[n].face.index = index + 3 * i;
			self->faces.array[n].face.bounds = *bounds;
			self->faces.array[n].face.matrix = *matrix;
			self->faces.array[n].face.mesh = mesh;
			self->faces.array[n].face.material = material;

			/* Calculate the center of the triangle. */
			center = limat_matrix_transform (*matrix, face_sort_coords[i]);

			/* Calculate bucket index based on distance to camera. */
			/* TODO: Would be better to use far plane distance here? */
			/* TODO: Non-linear mapping might work better for details closer to the camera? */
			diff = limat_vector_subtract (center, eye);
			dist = limat_vector_get_length (diff);
			bucket = dist / 50.0f * (self->buckets.count - 1);
			bucket = LIMAT_CLAMP (bucket, 0, self->buckets.count - 1);

			/* Insert to the depth bucket. */
			self->faces.array[n].next = self->buckets.array[bucket];
			self->buckets.array[bucket] = self->faces.array + n;
			self->faces.count++;
		}
	}
	else
	{
		/* Resize the buffer if necessary. */
		if (!private_resize_faces (self, self->faces.count + 1))
			return 0;

		/* Calculate the center of the group. */
		center = limat_vector_init (0.0f, 0.0f, 0.0f);
		for (i = 0 ; i < count / 3 ; i++)
		{
			center = limat_vector_multiply (limat_vector_add (center,
				limat_matrix_transform (*matrix, face_sort_coords[i])), 1.0f / count);
		}

		/* Calculate bucket index based on distance to camera. */
		/* TODO: Would be better to use far plane distance here? */
		/* TODO: Non-linear mapping might work better for details closer to the camera? */
		diff = limat_vector_subtract (center, eye);
		dist = limat_vector_get_length (diff);
		bucket = dist / 50.0f * (self->buckets.count - 1);
		bucket = LIMAT_CLAMP (bucket, 0, self->buckets.count - 1);

		/* Add the whole group to the depth bucket. */
		n = self->faces.count;
		self->faces.array[n].next = self->buckets.array[bucket];
		self->faces.array[n].type = LIREN_SORT_TYPE_GROUP;
		self->faces.array[n].group.bounds = *bounds;
		self->faces.array[n].group.matrix = *matrix;
		self->faces.array[n].group.index = index;
		self->faces.array[n].group.count = count;
		self->faces.array[n].group.mesh = mesh;
		self->faces.array[n].group.material = material;
		self->buckets.array[bucket] = self->faces.array + n;
		self->faces.count++;
	}

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
	LIRenMaterial* material;

	/* Frustum culling. */
	if (limat_frustum_cull_aabb (&self->frustum, bounds))
		return 1;

	/* Add each material group. */
	for (i = 0 ; i < model->materials.count ; i++)
	{
		material = model->materials.array[i];
		ret &= liren_sort_add_group (self, bounds, matrix,
			model->groups.array[i].start, model->groups.array[i].count,
			&model->mesh, material, model->groups.array[i].face_sort_coords);
	}

	return ret;
}

int liren_sort_add_object (
	LIRenSort*   self,
	LIRenObject* object)
{
	LIMatAabb bounds;
	LIRenShader* shader;

	/* Add each face group of the model. */
	liren_object_get_bounds (object, &bounds);
	if (!liren_sort_add_model (self, &bounds, &object->orientation.matrix, object->model))
		return 0;

	/* Add particle systems of the object. */
	shader = liren_render_find_shader (self->render, "particle");
	if (shader != NULL)
	{
		liren_particles_sort (
			&object->model->particles,
			object->particle.time,
			object->particle.loop,
			&object->transform,
			shader, self);
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

	/* Don't add particles with no valid shader. */
	if (shader == NULL)
		return 1;

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
