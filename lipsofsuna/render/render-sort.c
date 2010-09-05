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
 * \addtogroup LIRenSort Sort
 * @{
 */

#include "render-sort.h"

LIRenSort* liren_sort_new (
	LIRenRender* render)
{
	LIRenSort* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenSort));
	if (self == NULL)
		return NULL;

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
	LIRenBuffer*   indices,
	LIRenBuffer*   vertices,
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
	self->groups.array[num].transparent = transparent;
	self->groups.array[num].bounds = *bounds;
	self->groups.array[num].matrix = *matrix;
	self->groups.array[num].indices = indices;
	self->groups.array[num].vertices = vertices;
	self->groups.array[num].material = material;
	self->groups.count++;

	if (transparent)
		return liren_sort_add_faces (self, bounds, matrix, indices, vertices, material);

	return 1;
}

int liren_sort_add_faces (
	LIRenSort*     self,
	LIMatAabb*     bounds,
	LIMatMatrix*   matrix,
	LIRenBuffer*   indices,
	LIRenBuffer*   vertices,
	LIRenMaterial* material)
{
	int i;
	int need;
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
	LIRenSortface* ptr;
	LIRenSortface* tmp;

	/* Resize the buffer if necessary. */
	need = self->faces.count + indices->elements.count / 3;
	if (self->faces.capacity <= need)
	{
		num = self->faces.capacity << 1;
		while (num < need)
			num <<= 1;
		tmp = lisys_realloc (self->faces.array, num * sizeof (LIRenSortface));
		if (tmp == NULL)
			return 0;
		if (tmp != self->faces.array)
		{
			/* If the address of the face buffer changed, we need to adjust
			   the pointers in the buckets to match the new address. */
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
		self->faces.array = tmp;
		self->faces.capacity = num;
	}

	/* Calculate camera position. */
	mat = limat_matrix_invert (self->modelview);
	eye = limat_matrix_transform (mat, limat_vector_init (0.0f, 0.0f, 0.0f));

	/* Add each face in the group. */
	num = self->faces.count;
	fmt = &vertices->format;
	vtxdata = liren_buffer_lock (vertices, 0);
	idxdata = liren_buffer_lock (indices, 0);
	for (i = 0 ; i < indices->elements.count ; i += 3, num++)
	{
		/* Append the face to the buffer. */
		self->faces.array[num].index = i;
		self->faces.array[num].bounds = *bounds;
		self->faces.array[num].matrix = *matrix;
		self->faces.array[num].indices = indices;
		self->faces.array[num].vertices = vertices;
		self->faces.array[num].material = material;

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
	liren_buffer_unlock (indices, idxdata);
	liren_buffer_unlock (vertices, vtxdata);
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
	LIRenBuffer* buffer;
	LIRenMaterial* material;

	for (i = 0 ; i < model->materials.count ; i++)
	{
		material = model->materials.array[i];
		buffer = model->buffers.array + i;
		if (material->flags & LIREN_MATERIAL_FLAG_TRANSPARENCY)
			ret &= liren_sort_add_group (self, bounds, matrix, buffer, model->vertices, material, 1);
		else
			ret &= liren_sort_add_group (self, bounds, matrix, buffer, model->vertices, material, 0);
	}

	return ret;
}

int liren_sort_add_object (
	LIRenSort*   self,
	LIRenObject* object)
{
	LIMatAabb bounds;

	liren_object_get_bounds (object, &bounds);
	return liren_sort_add_model (self, &bounds, &object->orientation.matrix, object->model);
}

void liren_sort_clear (
	LIRenSort* self)
{
	self->groups.count = 0;
	self->faces.count = 0;
	memset (self->buckets.array, 0, self->buckets.count * sizeof (LIRenSortface*));
}

/** @} */
/** @} */
