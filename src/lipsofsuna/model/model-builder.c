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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlBuilder Builder
 * @{
 */

#include "model-builder.h"

static int private_realloc_array (
	void*  array,
	int*   capacity,
	int    required,
	size_t size);

/*****************************************************************************/

/**
 * \brief Creates a model builder.
 * \param model Model or NULL.
 * \return Model builder or NULL.
 */
LIMdlBuilder* limdl_builder_new (
	LIMdlModel* model)
{
	int i;
	LIMdlBuilder* self;
	LIMdlBuilderFaces* dstfaces;
	LIMdlFaces* srcfaces;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlBuilder));
	if (self == NULL)
		return NULL;

	/* Use or allocate model. */
	if (model == NULL)
	{
		self->model = limdl_model_new ();
		if (self->model == NULL)
		{
			limdl_builder_free (self);
			return NULL;
		}
	}
	else
		self->model = model;

	/* Separate indices of face groups. */
	lisys_assert (self->model->face_groups.count == self->model->materials.count);
	if (self->model->face_groups.count)
	{
		self->face_groups.array = lisys_calloc (self->model->face_groups.count, sizeof (LIMdlBuilderFaces));
		if (self->face_groups.array == NULL)
		{
			limdl_builder_free (self);
			return NULL;
		}
		self->face_groups.count = self->model->face_groups.count;
		self->face_groups.capacity = self->model->face_groups.count;
		for (i = 0 ; i < self->face_groups.count ; i++)
		{
			dstfaces = self->face_groups.array + i;
			srcfaces = self->model->face_groups.array + i;
			if (srcfaces->count)
			{
				dstfaces->indices.array = lisys_calloc (srcfaces->count, sizeof (LIMdlIndex));
				if (dstfaces->indices.array == NULL)
				{
					limdl_builder_free (self);
					return NULL;
				}
				memcpy (dstfaces->indices.array, self->model->indices.array + srcfaces->start, srcfaces->count * sizeof (LIMdlIndex));
				dstfaces->indices.count = srcfaces->count;
				dstfaces->indices.capacity = srcfaces->count;
			}
		}
	}

	/* Initialize builder state. */
	self->material_capacity = self->model->materials.count;
	self->weightgroup_capacity = self->model->weight_groups.count;
	self->vertex_capacity = self->model->vertices.count;

	return self;
}

/**
 * \brief Frees the model builder.
 * \param self Model builder.
 */
void limdl_builder_free (
	LIMdlBuilder* self)
{
	int i;

	for (i = 0 ; i < self->face_groups.count ; i++)
		lisys_free (self->face_groups.array[i].indices.array);
	lisys_free (self->face_groups.array);
	lisys_free (self);
}

/**
 * \brief Finishes building the model.
 * \param self Model builder.
 * \return Nonzero on success.
 */
int limdl_builder_finish (
	LIMdlBuilder* self)
{
	int i;
	int num_indices;
	int pos_indices;
	LIMdlBuilderFaces* srcfaces;
	LIMdlFaces* dstfaces;
	LIMdlFaces* new_faces;
	LIMdlIndex* new_indices;

	/* Recreate face groups. */
	num_indices = 0;
	if (self->face_groups.count)
	{
		new_faces = lisys_calloc (self->face_groups.count, sizeof (LIMdlFaces));
		if (new_faces == NULL)
			return 0;
		for (i = 0 ; i < self->face_groups.count ; i++)
		{
			srcfaces = self->face_groups.array + i;
			dstfaces = new_faces + i;
			dstfaces->start = num_indices;
			dstfaces->count = srcfaces->indices.count;
			num_indices += srcfaces->indices.count;
		}
		lisys_free (self->model->face_groups.array);
		self->model->face_groups.array = new_faces;
		self->model->face_groups.count = self->face_groups.count;
	}
	else
	{
		lisys_free (self->model->face_groups.array);
		self->model->face_groups.array = NULL;
		self->model->face_groups.count = 0;
	}
	lisys_assert (self->model->face_groups.count == self->face_groups.count);
	lisys_assert (self->model->face_groups.count == self->model->materials.count);

	/* Merge indices of face groups. */
	if (num_indices)
	{
		new_indices = lisys_calloc (num_indices, sizeof (LIMdlIndex));
		if (new_indices == NULL)
			return 0;
		pos_indices = 0;
		for (i = 0 ; i < self->face_groups.count ; i++)
		{
			srcfaces = self->face_groups.array + i;
			memcpy (new_indices + pos_indices, srcfaces->indices.array, srcfaces->indices.count * sizeof (LIMdlIndex));
			pos_indices += srcfaces->indices.count;
		}
		lisys_assert (pos_indices == num_indices);
		lisys_free (self->model->indices.array);
		self->model->indices.array = new_indices;
		self->model->indices.count = num_indices;
	}
	else
	{
		lisys_free (self->model->indices.array);
		self->model->indices.array = NULL;
		self->model->indices.count = 0;
	}

	/* Calculate the bounding box. */
	limdl_model_calculate_bounds (self->model);

	/* Calculate vertex tangents. */
	limdl_model_calculate_tangents (self->model);

	return 1;
}

/**
 * \brief Inserts a triangle to the model.
 *
 * Inserts vertices, vertex weights, and indices to the model, merging the
 * new vertex with existing vertices, if possible.
 *
 * \param self Model builder.
 * \param material Material index.
 * \param vertices Array of three vertices.
 * \param bone_mapping Bone index mapping array or NULL.
 * \return Nonzero on success.
 */
int limdl_builder_insert_face (
	LIMdlBuilder*      self,
	int                material,
	const LIMdlVertex* vertices,
	const int*         bone_mapping)
{
	LIMdlIndex indices[3];

	/* Insert vertices. */
	if (!limdl_builder_insert_vertices (self, vertices, 3, bone_mapping))
		return 0;

	/* Insert indices. */
	indices[0] = self->model->vertices.count - 3;
	indices[1] = self->model->vertices.count - 2;
	indices[2] = self->model->vertices.count - 1;
	if (!limdl_builder_insert_indices (self, material, indices, 3, 0))
	{
		self->model->vertices.count -= 3;
		return 0;
	}

	return 1;
}

/**
 * \brief Inserts indices to the model.
 * \param self Model builder.
 * \param material Material index.
 * \param indices Array of indices.
 * \param count Number of indices.
 * \param vertex_start_remap Offset of the first vertex for remap purposes.
 * \return Nonzero on success.
 */
int limdl_builder_insert_indices (
	LIMdlBuilder*     self,
	int               material,
	const LIMdlIndex* indices,
	int               count,
	int               vertex_start_remap)
{
	int i;
	LIMdlBuilderFaces* group;

	lisys_assert (material >= 0);
	lisys_assert (material < self->face_groups.count);
	group = self->face_groups.array + material;

	/* Allocate space for indices. */
	if (!private_realloc_array (&group->indices.array, &group->indices.capacity,
	    group->indices.count + count, sizeof (LIMdlIndex)))
		return 0;

	/* Insert indices. */
	if (vertex_start_remap)
	{
		for (i = 0 ; i < count ; i++)
			group->indices.array[group->indices.count++] = indices[i] + vertex_start_remap;
	}
	else
	{
		memcpy (group->indices.array + group->indices.count, indices, count * sizeof (LIMdlIndex));
		group->indices.count += count;
	}

	return 1;
}

/**
 * \brief Inserts a material to the model.
 * \param self Model builder.
 * \param material Material.
 * \return Nonzero on success.
 */
int limdl_builder_insert_material (
	LIMdlBuilder*        self,
	const LIMdlMaterial* material)
{
	LIMdlMaterial* tmp;

	lisys_assert (self->model->materials.count == self->face_groups.count);

	/* Allocate space for materials. */
	if (!private_realloc_array (&self->model->materials.array,
	    &self->material_capacity, self->model->materials.count + 1, sizeof (LIMdlMaterial)))
		return 0;

	/* Allocate space for face groups. */
	if (!private_realloc_array (&self->face_groups.array,
	    &self->face_groups.capacity, self->face_groups.count + 1, sizeof (LIMdlBuilderFaces)))
		return 0;
	memset (self->face_groups.array + self->face_groups.count, 0, sizeof (LIMdlBuilderFaces));

	/* Copy material. */
	tmp = self->model->materials.array + self->model->materials.count;
	if (!limdl_material_init_copy (tmp, material))
		return 0;
	self->model->materials.count++;
	self->face_groups.count++;

	return 1;
}

/**
 * \brief Inserts a node to the model.
 * \param self Model builder.
 * \param node Node.
 * \return Nonzero on success.
 */
int limdl_builder_insert_node (
	LIMdlBuilder*    self,
	const LIMdlNode* node)
{
	LIMdlNode** tmp;

	tmp = realloc (self->model->nodes.array, (self->model->nodes.count + 1) * sizeof (LIMdlNode*));
	if (tmp == NULL)
		return 0;
	self->model->nodes.array = tmp;
	tmp += self->model->nodes.count;

	*tmp = limdl_node_copy (node);
	if (*tmp == NULL)
		return 0;
	self->model->nodes.count++;

	return 1;
}

/**
 * \brief Inserts vertices to the model.
 * \param self Model builder.
 * \param vertices Array of vertex.
 * \param count Number of vertices.
 * \param bone_mapping Bone index mapping array or NULL.
 * \return Nonzero on success.
 */
int limdl_builder_insert_vertices (
	LIMdlBuilder*      self,
	const LIMdlVertex* vertices,
	int                count,
	const int*         bone_mapping)
{
	int i;
	int j;
	int offset;
	LIMdlVertex* vertex;

	/* Allocate space for vertices. */
	if (!private_realloc_array (&self->model->vertices.array,
	    &self->vertex_capacity, self->model->vertices.count + count, sizeof (LIMdlVertex)))
		return 0;

	/* Append vertices. */
	offset = self->model->vertices.count;
	memcpy (self->model->vertices.array + offset, vertices, count * sizeof (LIMdlVertex));
	self->model->vertices.count += count;

	/* Map bone indices. */
	if (bone_mapping != NULL)
	{
		for (j = offset ; j < self->model->vertices.count ; j++)
		{
			vertex = self->model->vertices.array + j;
			for (i = 0 ; i < LIMDL_VERTEX_WEIGHTS_MAX ; i++)
			{
				if (vertex->bones[i])
					vertex->bones[i] = bone_mapping[vertex->bones[i] - 1] + 1;
			}
		}
	}

	return 1;
}

/**
 * \brief Inserts a weight group to the model.
 * \param self Model builder.
 * \param name Group name.
 * \param bone Bone name.
 * \return Nonzero on success.
 */
int limdl_builder_insert_weightgroup (
	LIMdlBuilder* self,
	const char*   name,
	const char*   bone)
{
	LIMdlWeightGroup* tmp;

	/* Allocate space for weight groups. */
	if (!private_realloc_array (&self->model->weight_groups.array,
	    &self->weightgroup_capacity, self->model->weight_groups.count + 1, sizeof (LIMdlWeightGroup)))
		return 0;

	/* Copy weight group. */
	tmp = self->model->weight_groups.array + self->model->weight_groups.count;
	tmp->name = lisys_string_dup (name);
	tmp->bone = lisys_string_dup (bone);
	if (tmp->name == NULL || tmp->bone == NULL)
	{
		lisys_free (tmp->name);
		lisys_free (tmp->bone);
		return 0;
	}
	self->model->weight_groups.count++;

	/* Map node. */
	tmp->node = limdl_model_find_node (self->model, tmp->bone);

	return 1;
}

/*****************************************************************************/

static int private_realloc_array (
	void*  array,
	int*   capacity,
	int    required,
	size_t size)
{
	int i;
	void* tmp;

	if (*capacity < required)
	{
		i = (*capacity > 64)? *capacity : 64;
		while (i < required)
			i <<= 1;
		tmp = lisys_realloc (*((void**) array), i * size);
		if (tmp == NULL)
			return 0;
		*((void**) array) = tmp;
		*capacity = i;
	}

	return 1;
}

/** @} */
/** @} */
