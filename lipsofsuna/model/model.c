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
 * \addtogroup limdl Model
 * @{
 * \addtogroup LIMdlModel Model
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "model.h"

typedef int (*LIMdlWriteFunc)(const LIMdlModel*, LIArcWriter*);

static void private_build (
	LIMdlModel* self);

static int private_read (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_animations (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_bounds (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_faces (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_hairs (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_materials (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_nodes (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_particles (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_shapes (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_vertices (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_weights (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_vertex_weights (
	LIMdlModel*   self,
	LIMdlWeights* weights,
	LIArcReader*  reader);

static int private_write (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_block (
	const LIMdlModel* self,
	const char*       name,
	LIMdlWriteFunc    func,
	LIArcWriter*      writer);

static int private_write_animations (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_bounds (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_faces (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_hairs (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_header (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_materials (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_nodes (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_particles (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_shapes (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_vertices (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_weights (
	const LIMdlModel* self,
	LIArcWriter*      writer);

/*****************************************************************************/

LIMdlModel*
limdl_model_new ()
{
	LIMdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlModel));
	if (self == NULL)
		return NULL;

	return self;
}

LIMdlModel*
limdl_model_new_copy (LIMdlModel* model)
{
	LIMdlModel* self;
	LIArcReader* reader;
	LIArcWriter* writer;

	/* FIXME: This is lousy. */
	writer = liarc_writer_new ();
	if (writer == NULL)
		return NULL;
	if (!limdl_model_write (model, writer))
	{
		liarc_writer_free (writer);
		return NULL;
	}
	reader = liarc_reader_new (
		liarc_writer_get_buffer (writer),
		liarc_writer_get_length (writer));
	if (reader == NULL)
	{
		liarc_writer_free (writer);
		return NULL;
	}
	self = limdl_model_new_from_data (reader);
	liarc_reader_free (reader);
	liarc_writer_free (writer);

	return self;
}

/**
 * \brief Loads a model from uncompressed data.
 *
 * \param reader A reader.
 * \return A new model or NULL.
 */
LIMdlModel*
limdl_model_new_from_data (LIArcReader* reader)
{
	LIMdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlModel));
	if (self == NULL)
	{
		lisys_error_append ("cannot load model");
		return NULL;
	}

	/* Read from stream. */
	if (!private_read (self, reader))
	{
		lisys_error_append ("cannot load model");
		limdl_model_free (self);
		return NULL;
	}

	/* Construct the rest pose. */
	private_build (self);
	return self;
}

/**
 * \brief Loads a model from a file.
 *
 * \param path The path to the file.
 * \return A new model or NULL.
 */
LIMdlModel*
limdl_model_new_from_file (const char* path)
{
	LIArcReader* reader;
	LIMdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlModel));
	if (self == NULL)
	{
		lisys_error_append ("cannot load model `%s'", path);
		return NULL;
	}

	/* Open the file. */
	reader = liarc_reader_new_from_file (path);
	if (reader == NULL)
		goto error;

	/* Read from stream. */
	if (!private_read (self, reader))
		goto error;
	liarc_reader_free (reader);

	/* Construct the rest pose. */
	private_build (self);
	return self;

error:
	lisys_error_append ("cannot load model `%s'", path);
	limdl_model_free (self);
	if (reader != NULL)
		liarc_reader_free (reader);
	return NULL;
}

/**
 * \brief Frees the model.
 *
 * \param self A model.
 */
void
limdl_model_free (LIMdlModel* self)
{
	int i;
	LIMdlMaterial* material;

	/* Free materials. */
	if (self->materials.array != NULL)
	{
		for (i = 0 ; i < self->materials.count ; i++)
		{
			material = self->materials.array + i;
			limdl_material_free (material);
		}
		lisys_free (self->materials.array);
	}

	/* Free face groups. */
	if (self->facegroups.array != NULL)
	{
		for (i = 0 ; i < self->facegroups.count ; i++)
			limdl_faces_free (self->facegroups.array + i);
		lisys_free (self->facegroups.array);
	}

	/* Free weight groups. */
	if (self->weightgroups.array != NULL)
	{
		for (i = 0 ; i < self->weightgroups.count ; i++)
		{
			lisys_free (self->weightgroups.array[i].name);
			lisys_free (self->weightgroups.array[i].bone);
		}
		lisys_free (self->weightgroups.array);
	}
	lisys_free (self->vertices.array);

	/* Free vertex weights. */
	for (i = 0 ; i < self->weights.count ; i++)
		lisys_free (self->weights.array[i].weights);
	lisys_free (self->weights.array);

	/* Free nodes. */
	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		lisys_free (self->nodes.array);
	}

	/* Free animations. */
	if (self->animations.array != NULL)
	{
		for (i = 0 ; i < self->animations.count ; i++)
			limdl_animation_clear (self->animations.array + i);
		lisys_free (self->animations.array);
	}

	/* Free particles. */
	if (self->particlesystems.array != NULL)
	{
		for (i = 0 ; i < self->particlesystems.count ; i++)
			limdl_particle_system_clear (self->particlesystems.array + i);
		lisys_free (self->particlesystems.array);
	}
	if (self->hairs.array != NULL)
	{
		for (i = 0 ; i < self->hairs.count ; i++)
			limdl_hairs_free (self->hairs.array + i);
		lisys_free (self->hairs.array);
	}

	/* Free shapes. */
	if (self->shapes.array != NULL)
	{
		for (i = 0 ; i < self->shapes.count ; i++)
			limdl_shape_clear (self->shapes.array + i);
		lisys_free (self->shapes.array);
	}

	lisys_free (self);
}

/**
 * \brief Recalculates the bounding box of the model.
 *
 * Loops through all vertices of the model and calculates the minimum and
 * maximum axis values used.
 *
 * \param self Model.
 */
void
limdl_model_calculate_bounds (LIMdlModel* self)
{
	int j;
	LIMatVector v;

	self->bounds.min = limat_vector_init (2.0E10, 2.0E10, 2.0E10);
	self->bounds.max = limat_vector_init (-2.0E10, -2.0E10, -2.0E10);
	for (j = 0 ; j < self->vertices.count ; j++)
	{
		v = self->vertices.array[j].coord;
		if (self->bounds.min.x > v.x)
			self->bounds.min.x = v.x;
		if (self->bounds.min.y > v.y)
			self->bounds.min.y = v.y;
		if (self->bounds.min.z > v.z)
			self->bounds.min.z = v.z;
		if (self->bounds.max.x < v.x)
			self->bounds.max.x = v.x;
		if (self->bounds.max.y < v.y)
			self->bounds.max.y = v.y;
		if (self->bounds.max.z < v.z)
			self->bounds.max.z = v.z;
	}
	if (self->bounds.min.x > self->bounds.max.x)
	{
		self->bounds.min = limat_vector_init (0.0f, 0.0f, 0.0f);
		self->bounds.max = limat_vector_init (0.0f, 0.0f, 0.0f);
	}
}

/**
 * \brief Finds an animation by name.
 *
 * \param self Model.
 * \param name Animation name.
 * \return Animation or NULL.
 */
LIMdlAnimation*
limdl_model_find_animation (LIMdlModel* self,
                            const char* name)
{
	int i;

	for (i = 0 ; i < self->animations.count ; i++)
	{
		if (!strcmp (self->animations.array[i].name, name))
			return self->animations.array + i;
	}

	return NULL;
}

/**
 * \brief Finds a face group.
 *
 * \param self Model.
 * \param material Material index.
 * \return Face group index or -1.
 */
int
limdl_model_find_facegroup (LIMdlModel* self,
                            int         material)
{
	int i;

	for (i = 0 ; i < self->facegroups.count ; i++)
	{
		if (self->facegroups.array[i].material == material)
			return i;
	}

	return -1;
}

/**
 * \brief Finds the material index of the material.
 *
 * Finds the first material that matches the passed material.
 * The materials are considered to match if they have the same shader,
 * shininess, diffuse color, specular color, and texture images.
 *
 * \param self Model.
 * \param material Material.
 * \return Material index or -1.
 */
int
limdl_model_find_material (const LIMdlModel*    self,
                           const LIMdlMaterial* material)
{
	int i;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		if (limdl_material_compare (self->materials.array + i, material))
			return i;
	}

	return -1;
}

/**
 * \brief Finds a node by name.
 *
 * \param self Model.
 * \param name Name of the node to find.
 * \return Node or NULL.
 */
LIMdlNode*
limdl_model_find_node (const LIMdlModel* self,
                       const char*       name)
{
	int i;
	LIMdlNode* node;

	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		node = limdl_node_find_node (node, name);
		if (node != NULL)
			return node;
	}

	return NULL;
}

/**
 * \brief Finds the index of a matching vertex.
 *
 * \param self Model.
 * \param vertex Vertex.
 * \return Index in vertex array or -1 if not found.
 */
int limdl_model_find_vertex (
	LIMdlModel*        self,
	const LIMdlVertex* vertex)
{
	int i;

	for (i = 0 ; i < self->vertices.count ; i++)
	{
		if (limdl_vertex_compare (self->vertices.array + i, vertex) == 0)
			return i;
	}

	return -1;
}

/**
 * \brief Finds the index of a matching vertex.
 *
 * \param self Model.
 * \param vertex Vertex.
 * \param weights Weight group influences.
 * \param mapping Maps group indices of the weights array.
 * \return Index in vertex array or -1 if not found.
 */
int limdl_model_find_vertex_weighted (
	LIMdlModel*         self,
	const LIMdlVertex*  vertex,
	const LIMdlWeights* weights,
	const int*          mapping)
{
	int i;
	int j;
	int k;
	LIMdlWeights* weights1;

	for (i = 0 ; i < self->vertices.count ; i++)
	{
		/* Match vertex. */
		if (limdl_vertex_compare (self->vertices.array + i, vertex) != 0)
			continue;

		/* Match weight counts. */
		weights1 = self->weights.array + i;
		if (weights1 == NULL && weights == NULL)
			return 1;
		if (weights1->count != weights->count)
			continue;

		/* Match weight influences. */
		for (j = 0 ; j < weights1->count ; j++)
		{
			for (k = 0 ; k < weights->count ; k++)
			{
				if (weights1->weights[j].group == weights->weights[mapping[k]].group &&
				    weights1->weights[j].weight == weights->weights[mapping[k]].weight)
					break;
			}
			if (k == weights->count)
				break;
		}
		if (j == weights->count)
			return i;
	}

	return -1;
}

/**
 * \brief Finds a matching weight group.
 *
 * \param self Model.
 * \param name Group name.
 * \param bone Bone name.
 * \return Index in weight group array or -1 if not found.
 */
int limdl_model_find_weightgroup (
	LIMdlModel* self,
	const char* name,
	const char* bone)
{
	int i;
	LIMdlWeightGroup* group;

	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		group = self->weightgroups.array + i;
		if (!strcmp (group->name, name) &&
		    !strcmp (group->bone, bone))
			return i;
	}

	return -1;
}

/**
 * \brief Inserts a triangle to the model.
 *
 * Inserts vertices, vertex weights, and indices to the model, merging the
 * new vertex with existing vertices, if possible.
 *
 * \param self Model.
 * \param group Face group index.
 * \param vertices Array of three vertices.
 * \param weights Array of three weights or NULL.
 * \return Nonzero on success.
 */
int
limdl_model_insert_face (LIMdlModel*         self,
                         int                 group,
                         const LIMdlVertex*  vertices,
                         const LIMdlWeights* weights)
{
	int i;
	uint32_t index;
	uint32_t* indices;
	LIMdlFaces* group_;

	group_ = self->facegroups.array + group;

	/* Allocate space. */
	if (group_->indices.capacity < group_->indices.count + 3)
	{
		if (group_->indices.capacity)
			i = group_->indices.capacity << 1;
		else
			i = 64;
		indices = lisys_realloc (group_->indices.array, i * sizeof (uint32_t));
		if (indices == NULL)
			return 0;
		group_->indices.array = indices;
		group_->indices.capacity = i;
	}

	/* Insert vertices and indices. */
	for (i = 0 ; i < 3 ; i++)
	{
		/* Insert vertex. */
		index = limdl_model_find_vertex (self, vertices + i);
		if (index == -1)
		{
			index = self->vertices.count;
			if (!limdl_model_insert_vertex (self, vertices + i))
				return 0;
			if (!limdl_model_insert_vertex_weights (self, weights, NULL))
			{
				self->vertices.count--;
				return 0;
			}
		}

		/* Insert index. */
		group_->indices.array[group_->indices.count] = index;
		group_->indices.count++;
	}

	return 1;
}

/**
 * \brief Inserts a face group to the model.
 *
 * \param self Model.
 * \param material Material index.
 * \return Nonzero on success.
 */
int
limdl_model_insert_facegroup (LIMdlModel* self,
                              int         material)
{
	int count;
	LIMdlFaces* tmp;

	lisys_assert (material >= 0);
	lisys_assert (material < self->materials.count);

	/* Resize buffer. */
	count = self->facegroups.count + 1;
	tmp = lisys_realloc (self->facegroups.array, count * sizeof (LIMdlFaces));
	if (tmp == NULL)
		return 0;
	self->facegroups.array = tmp;
	tmp += self->facegroups.count;
	self->facegroups.count++;

	/* Initialize group. */
	memset (tmp, 0, sizeof (LIMdlFaces));
	tmp->material = material;

	return 1;
}

/**
 * \brief Inserts indices to the model.
 *
 * \param self Model.
 * \param group Face group index.
 * \param indices Array of indices.
 * \param count Number of indices.
 * \return Nonzero on success.
 */
int
limdl_model_insert_indices (LIMdlModel* self,
                            int         group,
                            uint32_t*   indices,
                            int         count)
{
	int i;
	uint32_t* tmp;
	LIMdlFaces* group_;

	group_ = self->facegroups.array + group;

	/* Allocate space. */
	if (group_->indices.capacity < group_->indices.count + count)
	{
		if (group_->indices.capacity)
			i = group_->indices.capacity;
		else
			i = 64;
		while (i < group_->indices.count + count)
			i <<= 1;
		tmp = lisys_realloc (group_->indices.array, i * sizeof (uint32_t));
		if (tmp == NULL)
			return 0;
		group_->indices.array = tmp;
		group_->indices.capacity = i;
	}

	/* Insert indices. */
	memcpy (group_->indices.array + group_->indices.count, indices, count * sizeof (uint32_t));
	group_->indices.count += count;

	return 1;
}

/**
 * \brief Inserts a material to the model.
 *
 * \param self Model.
 * \param material Material.
 * \return Nonzero on success.
 */
int
limdl_model_insert_material (LIMdlModel*          self,
                             const LIMdlMaterial* material)
{
	int count;
	LIMdlMaterial* tmp;

	/* Resize buffer. */
	count = self->materials.count + 1;
	tmp = lisys_realloc (self->materials.array, count * sizeof (LIMdlMaterial));
	if (tmp == NULL)
		return 0;
	self->materials.array = tmp;
	tmp += self->materials.count;

	/* Copy material. */
	if (!limdl_material_init_copy (tmp, material))
		return 0;
	self->materials.count++;

	return 1;
}

/**
 * \brief Inserts a node to the model.
 *
 * \param self Model.
 * \param node Node.
 * \return Nonzero on success.
 */
int
limdl_model_insert_node (LIMdlModel*      self,
                         const LIMdlNode* node)
{
	LIMdlNode** tmp;

	tmp = realloc (self->nodes.array, (self->nodes.count + 1) * sizeof (LIMdlNode*));
	if (tmp == NULL)
		return 0;
	self->nodes.array = tmp;
	tmp += self->nodes.count;

	*tmp = limdl_node_copy (node);
	if (*tmp == NULL)
		return 0;
	self->nodes.count++;

	return 1;
}

/**
 * \brief Inserts a vertex to the model.
 *
 * \param self Model.
 * \param vertex Vertex.
 * \return Nonzero on success.
 */
int limdl_model_insert_vertex (
	LIMdlModel*         self,
	const LIMdlVertex*  vertex)
{
	int count;
	int capacity;
	void* tmp;

	/* Allocate space for the vertex. */
	if (self->vertices.capacity < self->vertices.count + 1)
	{
		if (self->vertices.capacity)
			capacity = self->vertices.capacity << 1;
		else
			capacity = 32;
		tmp = lisys_realloc (self->vertices.array, capacity * sizeof (LIMdlVertex));
		if (tmp == NULL)
			return 0;
		self->vertices.array = tmp;
		self->vertices.capacity = capacity;
	}
	count = self->vertices.count;

	/* Append vertex. */
	self->vertices.array[count] = *vertex;
	self->vertices.count++;

	return 1;
}

/**
 * \brief Inserts vertex weights to the model.
 *
 * \param self Model.
 * \param weights Vertex weights or NULL.
 * \param mapping Maps group indices of the weights array. NULL if no mapping is needed.
 * \return Nonzero on success.
 */
int limdl_model_insert_vertex_weights (
	LIMdlModel*         self,
	const LIMdlWeights* weights,
	const int*          mapping)
{
	int i;
	int group;
	int count;
	int capacity;
	LIMdlWeights* tmp;
	LIMdlWeights* dstwei;

	/* Don't create weights when none are needed. */
	if (!self->weights.count && (weights == NULL || !weights->count))
		return 1;
	lisys_assert (self->weights.count < self->vertices.count);
	count = self->weights.count;

	/* Allocate space for weights. */
	if (self->weights.capacity < self->vertices.count)
	{
		if (self->weights.capacity)
			capacity = self->weights.capacity;
		else
			capacity = 32;
		while (capacity < self->vertices.count) 
			capacity = capacity << 1;
		tmp = lisys_realloc (self->weights.array, capacity * sizeof (LIMdlWeights));
		if (tmp == NULL)
			return 0;
		self->weights.array = tmp;
		self->weights.capacity = capacity;
	}

	/* Initialize the new weights and any missing weights. */
	memset (self->weights.array + count, 0, (self->vertices.count - count) * sizeof (LIMdlWeights));
	dstwei = self->weights.array + self->vertices.count - 1;

	/* Copy weights. */
	if (weights != NULL && weights->count)
	{
		dstwei->weights = lisys_malloc (weights->count * sizeof (LIMdlWeight));
		if (dstwei->weights == NULL)
			return 0;
		memcpy (dstwei->weights, weights->weights, weights->count * sizeof (LIMdlWeight));
		dstwei->count = weights->count;
	}
	self->weights.count = self->vertices.count;

	/* Remap weight group indices. */
	if (mapping != NULL)
	{
		for (i = 0 ; i < dstwei->count ; i++)
		{
			group = mapping[dstwei->weights[i].group];
			lisys_assert (group < self->weightgroups.count);
			dstwei->weights[i].group = group;
		}
	}

	return 1;
}

/**
 * \brief Inserts a vertex to the model.
 *
 * \param self Model.
 * \param vertex Vertex.
 * \param weights Vertex weights or NULL.
 * \param mapping Maps group indices of the weights array. NULL if no mapping is needed.
 * \return Nonzero on success.
 */
int limdl_model_insert_vertex_weighted (
	LIMdlModel*         self,
	const LIMdlVertex*  vertex,
	const LIMdlWeights* weights,
	const int*          mapping)
{
	if (!limdl_model_insert_vertex (self, vertex))
		return 0;
	if (!limdl_model_insert_vertex_weights (self, weights, mapping))
	{
		self->vertices.count--;
		return 0;
	}

	return 1;
}

/**
 * \brief Inserts a weight group to the model.
 *
 * \param self Model.
 * \param name Group name.
 * \param bone Bone name.
 * \return Nonzero on success.
 */
int limdl_model_insert_weightgroup (
	LIMdlModel* self,
	const char* name,
	const char* bone)
{
	int count;
	LIMdlWeightGroup* tmp;

	/* Resize buffer. */
	count = self->weightgroups.count + 1;
	tmp = lisys_realloc (self->weightgroups.array, count * sizeof (LIMdlWeightGroup));
	if (tmp == NULL)
		return 0;
	self->weightgroups.array = tmp;
	tmp += self->weightgroups.count;

	/* Copy weight group. */
	tmp->name = listr_dup (name);
	tmp->bone = listr_dup (bone);
	if (tmp->name == NULL || tmp->bone == NULL)
	{
		lisys_free (tmp->name);
		lisys_free (tmp->bone);
		return 0;
	}
	self->weightgroups.count++;

	/* Map node. */
	tmp->node = limdl_model_find_node (self, tmp->bone);

	return 1;
}

int
limdl_model_merge (LIMdlModel* self,
                   LIMdlModel* model)
{
	int i;
	int j;
	int count;
	int group;
	int material;
	int vertex;
	int* vertices = NULL;
	int* wgroups = NULL;
	uint32_t index;
	uint32_t* indices;
	LIMdlFaces* dstfaces;
	LIMdlFaces* srcfaces;
	LIMdlWeights* weights;

	/* Map weight groups. */
	if (model->weightgroups.count)
	{
		wgroups = lisys_calloc (model->weightgroups.count, sizeof (int));
		if (wgroups == NULL)
			return 0;
		for (i = 0 ; i < model->weightgroups.count ; i++)
		{
			group = limdl_model_find_weightgroup (self,
				model->weightgroups.array[i].name,
				model->weightgroups.array[i].bone);
			if (group == -1)
			{
				group = self->weightgroups.count;
				if (!limdl_model_insert_weightgroup (self, 
				    model->weightgroups.array[i].name,
				    model->weightgroups.array[i].bone))
					goto error;
			}
			lisys_assert (group < self->weightgroups.count);
			wgroups[i] = group;
		}
	}

	/* Map vertices. */
	if (model->vertices.count)
	{
		vertices = lisys_calloc (model->vertices.count, sizeof (int));
		for (i = 0 ; i < model->vertices.count ; i++)
		{
			/* FIXME: Too slow with the lousy search algorithm. */
/*			vertex = limdl_model_find_vertex_weighted (self, model->vertices.array + i,
				model->weights.array + i, wgroups);
			if (vertex == -1)*/
			{
				vertex = self->vertices.count;
				if (model->weights.count)
					weights = model->weights.array + i;
				else
					weights = NULL;
				if (!limdl_model_insert_vertex_weighted (self, model->vertices.array + i, weights, wgroups))
					goto error;
			}
			vertices[i] = vertex;
		}
	}

	/* Merge each face group. */
	for (i = 0 ; i < model->facegroups.count ; i++)
	{
		srcfaces = model->facegroups.array + i;

		/* Find or create material. */
		material = limdl_model_find_material (self, model->materials.array + srcfaces->material);
		if (material == -1)
		{
			material = self->materials.count;
			if (!limdl_model_insert_material (self, model->materials.array + srcfaces->material))
				goto error;
		}

		/* Find or create face group. */
		group = limdl_model_find_facegroup (self, material);
		if (group == -1)
		{
			group = self->facegroups.count;
			if (!limdl_model_insert_facegroup (self, material))
				goto error;
		}

		/* Destination group for cloned faces. */
		dstfaces = self->facegroups.array + group;

		/* Allocate space for indices. */
		count = dstfaces->indices.count + srcfaces->indices.count;
		if (dstfaces->indices.capacity < count)
		{
			indices = lisys_realloc (dstfaces->indices.array, count * sizeof (uint32_t));
			if (indices == NULL)
				goto error;
			dstfaces->indices.array = indices;
			dstfaces->indices.capacity = count;
		}

		/* Insert indices. */
		for (j = 0 ; j < srcfaces->indices.count ; j++)
		{
			index = vertices[srcfaces->indices.array[j]];
			dstfaces->indices.array[dstfaces->indices.count] = index;
			dstfaces->indices.count++;
		}
	}

	lisys_free (wgroups);
	lisys_free (vertices);

	return 1;

error:
	lisys_free (wgroups);
	lisys_free (vertices);
	return 0;
}

int
limdl_model_write (const LIMdlModel* self,
                   LIArcWriter*      writer)
{
	return private_write (self, writer);
}

int
limdl_model_write_file (const LIMdlModel* self,
                        const char*       path)
{
	LIArcWriter* writer;

	/* Create a writer. */
	writer = liarc_writer_new_file (path);
	if (writer == NULL)
		return 0;

	/* Pack to the writer. */
	if (!limdl_model_write (self, writer))
	{
		liarc_writer_free (writer);
		return 0;
	}

	liarc_writer_free (writer);

	return 1;
}

int
limdl_model_get_index_count (const LIMdlModel* self)
{
	int i;
	int c;

	for (i = c = 0 ; i < self->facegroups.count ; i++)
		c += self->facegroups.array[i].indices.count;

	return c;
}

/*****************************************************************************/

static void
private_build (LIMdlModel* self)
{
	int i;
	LIMdlNode* node;
	LIMdlWeightGroup* group;

	/* Resolve node references. */
	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		group = self->weightgroups.array + i;
		group->node = limdl_model_find_node (self, group->bone);
	}

	/* Transform each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		limdl_node_rebuild (node, 1);
	}
}

static int
private_read (LIMdlModel*  self,
              LIArcReader* reader)
{
	int i;
	int j;
	int ret;
	char* id;
	uint32_t tmp;
	uint32_t size;
	uint32_t version;
	LIMdlFaces* group;

	/* Read magic. */
	if (!liarc_reader_check_text (reader, "lips/mdl", ""))
	{
		lisys_error_set (EINVAL, "wrong file format");
		return 0;
	}

	/* Read header chunk size. */
	if (!liarc_reader_get_uint32 (reader, &size))
		return 0;
	if (size != 8)
	{
		lisys_error_set (LISYS_ERROR_VERSION, "invalid header block");
		return 0;
	}

	/* Read version. */
	if (!liarc_reader_get_uint32 (reader, &version))
		return 0;
	if (version != LIMDL_FORMAT_VERSION)
	{
		lisys_error_set (LISYS_ERROR_VERSION, "model version mismatch");
		return 0;
	}

	/* Read flags. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->flags = tmp;

	/* Read chunks. */
	while (!liarc_reader_check_end (reader))
	{
		id = NULL;
		if (!liarc_reader_get_text (reader, "", &id) ||
		    !liarc_reader_get_uint32 (reader, &size))
		{
			lisys_error_append ("reading chunk failed");
			lisys_free (id);
			return 0;
		}
		if (size > reader->length - reader->pos)
		{
			lisys_error_append ("invalid chunk size for `%s'", id);
			lisys_free (id);
			return 0;
		}
		if (!strcmp (id, "bou"))
			ret = private_read_bounds (self, reader);
		else if (!strcmp (id, "mat"))
			ret = private_read_materials (self, reader);
		else if (!strcmp (id, "ver"))
			ret = private_read_vertices (self, reader);
		else if (!strcmp (id, "fac"))
			ret = private_read_faces (self, reader);
		else if (!strcmp (id, "wei"))
			ret = private_read_weights (self, reader);
		else if (!strcmp (id, "nod"))
			ret = private_read_nodes (self, reader);
		else if (!strcmp (id, "ani"))
			ret = private_read_animations (self, reader);
		else if (!strcmp (id, "hai"))
			ret = private_read_hairs (self, reader);
		else if (!strcmp (id, "par"))
			ret = private_read_particles (self, reader);
		else if (!strcmp (id, "sha"))
			ret = private_read_shapes (self, reader);
		else
		{
			if (!liarc_reader_skip_bytes (reader, size))
			{
				lisys_error_append ("failed to skip block `%s'", id);
				lisys_free (id);
				return 0;
			}
			ret = 1;
		}
		if (!ret)
		{
			lisys_error_append ("failed to read block `%s'", id);
			lisys_free (id);
			return 0;
		}
		lisys_free (id);
	}

	/* Sanity checks. */
	for (i = 0 ; i < self->facegroups.count ; i++)
	{
		group = self->facegroups.array + i;
		if (group->material >= self->materials.count)
		{
			lisys_error_set (EINVAL, "material index out of bounds");
			return 0;
		}
	}
	for (i = 0 ; i < self->weights.count ; i++)
	{
		for (j = 0 ; j < self->weights.array[i].count ; j++)
		{
			if (self->weights.array[i].weights[j].group >= self->weightgroups.count)
			{
				lisys_error_set (EINVAL, "weight group index out of bounds");
				return 0;
			}
		}
	}

	return 1;
}

static int
private_read_animations (LIMdlModel*  self,
                         LIArcReader* reader)
{
	int i;
	uint32_t tmp;
	LIMdlAnimation* animation;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->animations.count = tmp;

	/* Read animations. */
	if (self->animations.count)
	{
		self->animations.array = lisys_calloc (self->animations.count, sizeof (LIMdlAnimation));
		if (self->animations.array == NULL)
			return 0;
		for (i = 0 ; i < self->animations.count ; i++)
		{
			animation = self->animations.array + i;
			if (!limdl_animation_read (animation, reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_bounds (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	LIMatVector min;
	LIMatVector max;

	if (!liarc_reader_get_float (reader, &min.x) ||
	    !liarc_reader_get_float (reader, &min.y) ||
	    !liarc_reader_get_float (reader, &min.z) ||
	    !liarc_reader_get_float (reader, &max.x) ||
	    !liarc_reader_get_float (reader, &max.y) ||
	    !liarc_reader_get_float (reader, &max.z))
		return 0;
	limat_aabb_init_from_points (&self->bounds, &min, &max);

	return 1;
}

static int
private_read_faces (LIMdlModel*  self,
                    LIArcReader* reader)
{
	int i;
	uint32_t tmp;
	LIMdlFaces* group;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->facegroups.count = tmp;

	/* Read face groups. */
	if (self->facegroups.count)
	{
		self->facegroups.array = lisys_calloc (self->facegroups.count, sizeof (LIMdlFaces));
		if (self->facegroups.array == NULL)
			return 0;
		for (i = 0 ; i < self->facegroups.count ; i++)
		{
			group = self->facegroups.array + i;
			if (!limdl_faces_read (group, reader))
				return 0;
		}
	}

	return 1;
}

static int
private_read_hairs (LIMdlModel*  self,
                    LIArcReader* reader)
{
	int i;
	uint32_t count;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &count))
		return 0;
	self->hairs.count = count;

	/* Read hairs. */
	if (self->hairs.count)
	{
		self->hairs.array = lisys_calloc (self->hairs.count, sizeof (LIMdlHairs));
		if (self->hairs.array == NULL)
			return 0;
		for (i = 0 ; i < self->hairs.count ; i++)
		{
			if (!limdl_hairs_read (self->hairs.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int
private_read_materials (LIMdlModel*  self,
                        LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->materials.count = tmp;

	/* Read materials. */
	if (self->materials.count)
	{
		self->materials.array = lisys_calloc (self->materials.count, sizeof (LIMdlMaterial));
		if (self->materials.array == NULL)
			return 0;
		for (i = 0 ; i < self->materials.count ; i++)
		{
			if (!limdl_material_read (self->materials.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int
private_read_nodes (LIMdlModel*  self,
                    LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->nodes.count = tmp;

	/* Read nodes. */
	if (self->nodes.count)
	{
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (LIMdlNode*));
		if (self->nodes.array == NULL)
			return 0;
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_new (self);
			if (self->nodes.array[i] == NULL)
				return 0;
			if (!limdl_node_read (self->nodes.array[i], reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_particles (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp[1];

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, tmp + 0))
		return 0;

	/* Allocate particle systems. */
	if (tmp[0])
	{
		self->particlesystems.array = lisys_calloc (tmp[0], sizeof (LIMdlParticleSystem));
		if (self->particlesystems.array == NULL)
			return 0;
		self->particlesystems.count = tmp[0];
	}

	/* Read particle systems. */
	for (i = 0 ; i < self->particlesystems.count ; i++)
	{
		if (!limdl_particle_system_read (self->particlesystems.array + i, reader))
			return 0;
	}

	return 1;
}

static int
private_read_shapes (LIMdlModel*  self,
                     LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->shapes.count = tmp;

	/* Read shapes. */
	if (self->shapes.count)
	{
		self->shapes.array = lisys_calloc (self->shapes.count, sizeof (LIMdlShape));
		if (self->shapes.array == NULL)
			return 0;
		for (i = 0 ; i < self->shapes.count ; i++)
		{
			if (!limdl_shape_read (self->shapes.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int
private_read_vertices (LIMdlModel*  self,
                       LIArcReader* reader)
{
	int i;
	uint32_t tmp;
	LIMdlVertex* vertex;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->vertices.count = tmp;
	self->vertices.capacity = tmp;

	/* Read vertices. */
	if (tmp)
	{
		self->vertices.array = lisys_calloc (tmp, sizeof (LIMdlVertex));
		if (self->vertices.array == NULL)
			return 0;
		self->vertices.count = tmp;
		for (i = 0 ; i < self->vertices.count ; i++)
		{
			vertex = self->vertices.array + i;
			if (!liarc_reader_get_float (reader, vertex->texcoord + 0) ||
				!liarc_reader_get_float (reader, vertex->texcoord + 1) ||
				!liarc_reader_get_float (reader, &vertex->normal.x) ||
				!liarc_reader_get_float (reader, &vertex->normal.y) ||
				!liarc_reader_get_float (reader, &vertex->normal.z) ||
				!liarc_reader_get_float (reader, &vertex->coord.x) ||
				!liarc_reader_get_float (reader, &vertex->coord.y) ||
				!liarc_reader_get_float (reader, &vertex->coord.z))
				return 0;
		}
	}

	return 1;
}

static int
private_read_weights (LIMdlModel*  self,
                      LIArcReader* reader)
{
	int i;
	uint32_t tmp[2];
	LIMdlWeightGroup* group;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, tmp + 0))
		return 0;

	/* Read weight groups. */
	if (tmp[0])
	{
		self->weightgroups.array = lisys_calloc (tmp[0], sizeof (LIMdlWeightGroup));
		if (self->weightgroups.array == NULL)
			return 0;
		self->weightgroups.count = tmp[0];
		for (i = 0 ; i < self->weightgroups.count ; i++)
		{
			group = self->weightgroups.array + i;
			if (!liarc_reader_get_text (reader, "", &group->name) ||
				!liarc_reader_get_text (reader, "", &group->bone))
				return 0;
		}
	}

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, tmp + 1))
		return 0;
	if (tmp[1] != self->vertices.count)
	{
		lisys_error_set (EINVAL, "vertex and weight counts must be equal");
		return 0;
	}

	/* Read vertex weights. */
	if (tmp[1])
	{
		self->weights.array = lisys_calloc (tmp[1], sizeof (LIMdlWeights));
		if (self->weights.array == NULL)
			return 0;
		self->weights.count = tmp[1];
		self->weights.capacity = tmp[1];
		for (i = 0 ; i < self->weights.count ; i++)
		{
			if (!private_read_vertex_weights (self, self->weights.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int
private_read_vertex_weights (LIMdlModel*   self,
                             LIMdlWeights* weights,
                             LIArcReader*  reader)
{
	float weight;
	uint32_t i;
	uint32_t count;
	uint32_t group;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &count))
		return 0;

	/* Allocate weights. */
	weights->count = count;
	weights->weights = lisys_calloc (count, sizeof (LIMdlWeight));
	if (weights->weights == NULL)
		return 0;

	/* Read weights. */
	for (i = 0 ; i < count ; i++)
	{
		if (!liarc_reader_get_uint32 (reader, &group) ||
		    !liarc_reader_get_float (reader, &weight))
			return 0;
		if (group >= self->weightgroups.count)
		{
			lisys_error_set (EINVAL, "weight group index out of bounds");
			return 0;
		}
		weights->weights[i].group = group;
		weights->weights[i].weight = weight;
	}

	return 1;
}

static int private_write (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	if (!private_write_block (self, "lips/mdl", private_write_header, writer) ||
	    !private_write_block (self, "bou", private_write_bounds, writer) ||
	    !private_write_block (self, "mat", private_write_materials, writer) ||
	    !private_write_block (self, "ver", private_write_vertices, writer) ||
	    !private_write_block (self, "fac", private_write_faces, writer) ||
	    !private_write_block (self, "wei", private_write_weights, writer) ||
	    !private_write_block (self, "nod", private_write_nodes, writer) ||
	    !private_write_block (self, "ani", private_write_animations, writer) ||
	    !private_write_block (self, "hai", private_write_hairs, writer) ||
	    !private_write_block (self, "par", private_write_particles, writer) ||
	    !private_write_block (self, "sha", private_write_shapes, writer))
		return 0;
	return 1;
}

static int private_write_block (
	const LIMdlModel* self,
	const char*       name,
	LIMdlWriteFunc    func,
	LIArcWriter*      writer)
{
	int len;
	LIArcWriter* data;

	/* Write the block to a temporary buffer. */
	data = liarc_writer_new ();
	if (data == NULL)
		return 0;
	if (!func (self, data))
	{
		liarc_writer_free (data);
		return 0;
	}
	len = liarc_writer_get_length (data);
	if (!len)
		return 1;

	/* Write the temporary to the main writer as a block. */
	if (!liarc_writer_append_string (writer, name) ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, len) ||
	    !liarc_writer_append_raw (writer, liarc_writer_get_buffer (data), len))
	{
		liarc_writer_free (data);
		return 0;
	}
	liarc_writer_free (data);

	return 1;
}

static int private_write_animations (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	int j;
	LIMdlAnimation* animation;
	LIMatTransform* transform;

	/* Check if writing is needed. */
	if (!self->animations.count)
		return 1;

	/* Write animations. */
	if (!liarc_writer_append_uint32 (writer, self->animations.count))
		return 0;
	for (i = 0 ; i < self->animations.count ; i++)
	{
		animation = self->animations.array + i;
		if (!liarc_writer_append_string (writer, animation->name) ||
		    !liarc_writer_append_nul (writer) ||
		    !liarc_writer_append_uint32 (writer, animation->channels.count) ||
		    !liarc_writer_append_uint32 (writer, animation->length))
			return 0;
		for (j = 0 ; j < animation->channels.count ; j++)
		{
			if (!liarc_writer_append_string (writer, animation->channels.array[j]) ||
				!liarc_writer_append_nul (writer))
				return 0;
		}
		for (j = 0 ; j < animation->buffer.count ; j++)
		{
			transform = &animation->buffer.array[j].transform;
			if (!liarc_writer_append_float (writer, transform->position.x) ||
			    !liarc_writer_append_float (writer, transform->position.y) ||
			    !liarc_writer_append_float (writer, transform->position.z) ||
			    !liarc_writer_append_float (writer, transform->rotation.x) ||
			    !liarc_writer_append_float (writer, transform->rotation.y) ||
			    !liarc_writer_append_float (writer, transform->rotation.z) ||
			    !liarc_writer_append_float (writer, transform->rotation.w))
				return 0;
		}
	}

	return 1;
}

static int private_write_bounds (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	if (!liarc_writer_append_float (writer, self->bounds.min.x) ||
	    !liarc_writer_append_float (writer, self->bounds.min.y) ||
	    !liarc_writer_append_float (writer, self->bounds.min.z) ||
	    !liarc_writer_append_float (writer, self->bounds.max.x) ||
	    !liarc_writer_append_float (writer, self->bounds.max.y) ||
	    !liarc_writer_append_float (writer, self->bounds.max.z))
		return 0;
	return 1;
}

static int private_write_faces (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;

	/* Check if writing is needed. */
	if (!self->facegroups.count)
		return 1;

	if (!liarc_writer_append_uint32 (writer, self->facegroups.count))
		return 0;
	for (i = 0 ; i < self->facegroups.count ; i++)
	{
		if (!limdl_faces_write (self->facegroups.array + i, writer))
			return 0;
	}

	return 1;
}

static int private_write_hairs (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlHairs* hairs;

	/* Check if writing is needed. */
	if (!self->hairs.count)
		return 1;

	/* Write animations. */
	if (!liarc_writer_append_uint32 (writer, self->hairs.count))
		return 0;
	for (i = 0 ; i < self->hairs.count ; i++)
	{
		hairs = self->hairs.array + i;
		if (!limdl_hairs_write (hairs, writer))
			return 0;
	}

	return 1;
}

static int private_write_header (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	if (!liarc_writer_append_uint32 (writer, LIMDL_FORMAT_VERSION) ||
	    !liarc_writer_append_uint32 (writer, self->flags))
		return 0;
	return 1;
}

static int private_write_materials (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlMaterial* material;

	/* Check if writing is needed. */
	if (!self->materials.count)
		return 1;

	if (!liarc_writer_append_uint32 (writer, self->materials.count))
		return 0;
	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.array + i;
		if (!limdl_material_write (material, writer))
			return 0;
	}

	return 1;
}

static int private_write_nodes (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlNode* node;

	/* Check if writing is needed. */
	if (!self->nodes.count)
		return 1;

	/* Write nodes. */
	if (!liarc_writer_append_uint32 (writer, self->nodes.count))
		return 0;
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		if (!limdl_node_write (node, writer))
			return 0;
	}

	return 1;
}

static int private_write_particles (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;

	/* Check if writing is needed. */
	if (!self->particlesystems.count)
		return 1;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->particlesystems.count))
		return 0;

	/* Write particle systems. */
	for (i = 0 ; i < self->particlesystems.count ; i++)
	{
		if (!limdl_particle_system_write (self->particlesystems.array + i, writer))
			return 0;
	}

	return 1;
}

static int private_write_shapes (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlShape* shape;

	/* Check if writing is needed. */
	if (!self->shapes.count)
		return 1;

	/* Write nodes. */
	if (!liarc_writer_append_uint32 (writer, self->shapes.count))
		return 0;
	for (i = 0 ; i < self->shapes.count ; i++)
	{
		shape = self->shapes.array + i;
		if (!limdl_shape_write (shape, writer))
			return 0;
	}

	return 1;
}

static int private_write_vertices (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlVertex* vertex;

	/* Check if writing is needed. */
	if (!self->vertices.count)
		return 1;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->vertices.count))
		return 0;

	/* Write vertices. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		vertex = self->vertices.array + i;
		if (!liarc_writer_append_float (writer, vertex->texcoord[0]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[1]) ||
		    !liarc_writer_append_float (writer, vertex->normal.x) ||
		    !liarc_writer_append_float (writer, vertex->normal.y) ||
		    !liarc_writer_append_float (writer, vertex->normal.z) ||
		    !liarc_writer_append_float (writer, vertex->coord.x) ||
		    !liarc_writer_append_float (writer, vertex->coord.y) ||
		    !liarc_writer_append_float (writer, vertex->coord.z))
			return 0;
	}

	return 1;
}

static int private_write_weights (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	int j;
	LIMdlWeightGroup* group;
	LIMdlWeights* weights;

	/* Check if writing is needed. */
	if (!self->weightgroups.count)
		return 1;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->weightgroups.count))
		return 0;

	/* Write weight groups. */
	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		group = self->weightgroups.array + i;
		liarc_writer_append_string (writer, group->name);
		liarc_writer_append_nul (writer);
		liarc_writer_append_string (writer, group->bone);
		liarc_writer_append_nul (writer);
	}

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->weights.count))
		return 0;

	/* Write vertex weights. */
	for (i = 0 ; i < self->weights.count ; i++)
	{
		weights = self->weights.array + i;
		if (!liarc_writer_append_uint32 (writer, weights->count))
			return 0;
		for (j = 0 ; j < weights->count ; j++)
		{
			if (!liarc_writer_append_uint32 (writer, weights->weights[j].group) ||
			    !liarc_writer_append_float (writer, weights->weights[j].weight))
				return 0;
		}
	}

	return !writer->error;
}

/** @} */
/** @} */
