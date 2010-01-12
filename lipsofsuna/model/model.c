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

static void
private_build (LIMdlModel* self);

static void
private_free_animation (LIMdlModel*     self,
                        LIMdlAnimation* animation);

static int
private_read (LIMdlModel*  self,
              LIArcReader* reader);

static int
private_read_animations (LIMdlModel*  self,
                         LIArcReader* reader);

static int
private_read_faces (LIMdlModel*  self,
                    LIArcReader* reader);

static int
private_read_hairs (LIMdlModel*  self,
                    LIArcReader* reader);

static int
private_read_materials (LIMdlModel*  self,
                        LIArcReader* reader);

static int
private_read_nodes (LIMdlModel*  self,
                    LIArcReader* reader);

static int
private_read_shapes (LIMdlModel*  self,
                     LIArcReader* reader);

static int
private_read_vertices (LIMdlModel*  self,
                       LIArcReader* reader);

static int
private_read_weights (LIMdlModel*  self,
                      LIArcReader* reader);

static int
private_read_animation (LIMdlModel*     self,
                        LIMdlAnimation* animation,
                        LIArcReader*    reader);

static int
private_read_vertex_weights (LIMdlModel*   self,
                             LIMdlWeights* weights,
                             LIArcReader*  reader);

static int
private_write (const LIMdlModel* self,
               LIArcWriter*      writer);

static int
private_write_animations (const LIMdlModel* self,
                          LIArcWriter*      writer);

static int
private_write_faces (const LIMdlModel* self,
                     LIArcWriter*      writer);

static int
private_write_hairs (const LIMdlModel* self,
                     LIArcWriter*      writer);

static int
private_write_materials (const LIMdlModel* self,
                         LIArcWriter*      writer);

static int
private_write_nodes (const LIMdlModel* self,
                     LIArcWriter*      writer);

static int
private_write_shapes (const LIMdlModel* self,
                      LIArcWriter*      writer);

static int
private_write_vertices (const LIMdlModel* self,
                        LIArcWriter*      writer);

static int
private_write_weights (const LIMdlModel* self,
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
			limdl_material_clear_textures (material);
			lisys_free (material->shader);
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
			private_free_animation (self, self->animations.array + i);
		lisys_free (self->animations.array);
	}

	/* Free particles. */
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
		{
			lisys_free (self->shapes.array[i].name);
			lisys_free (self->shapes.array[i].vertices.array);
		}
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

	self->bounds.min = limat_vector_init (-2.0E10, -2.0E10, -2.0E10);
	self->bounds.max = limat_vector_init (2.0E10, 2.0E10, 2.0E10);
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
int
limdl_model_find_vertex (LIMdlModel*        self,
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
			if (!limdl_model_insert_vertex (self, vertices + i, (weights != NULL)? weights + i : NULL))
				return 0;
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

	assert (material >= 0);
	assert (material < self->materials.count);

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
 * \param weights Vertex weights or NULL.
 * \return Nonzero on success.
 */
int
limdl_model_insert_vertex (LIMdlModel*         self,
                           const LIMdlVertex*  vertex,
                           const LIMdlWeights* weights)
{
	int i;
	int count;
	void* tmp;
	LIMdlWeights* w;

	/* Resize buffers. */
	if (self->vertices.capacity < self->vertices.count + 1)
	{
		if (self->vertices.capacity)
			i = self->vertices.capacity << 1;
		else
			i = 32;
		tmp = lisys_realloc (self->vertices.array, i * sizeof (LIMdlVertex));
		if (tmp == NULL)
			return 0;
		self->vertices.array = tmp;
		self->vertices.capacity = i;
	}
	if (self->weights.capacity < self->weights.count + 1)
	{
		if (self->weights.capacity)
			i = self->weights.capacity << 1;
		else
			i = 32;
		tmp = lisys_realloc (self->weights.array, i * sizeof (LIMdlWeights));
		if (tmp == NULL)
			return 0;
		self->weights.array = tmp;
		self->weights.capacity = i;
	}
	count = self->vertices.count;
	assert (self->vertices.count == self->weights.count);

	/* Append weights. */
	if (weights != NULL && weights->count)
	{
		w = self->weights.array + count;
		w->weights = lisys_malloc (weights->count * sizeof (LIMdlWeight));
		if (w->weights == NULL)
			return 0;
		memcpy (w->weights, weights->weights, weights->count * sizeof (LIMdlWeight));
		w->count = weights->count;
	}
	else
	{
		w = self->weights.array + count;
		memset (w, 0, sizeof (LIMdlWeights));
	}

	/* Append vertex. */
	self->vertices.array[count] = *vertex;
	self->vertices.count++;
	self->weights.count++;

	return 1;
}

int
limdl_model_write (const LIMdlModel* self,
                   LIArcWriter*      writer)
{
	liarc_writer_append_string (writer, "lips/mdl");
	liarc_writer_append_nul (writer);
	if (writer->error)
		return 0;
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

static void
private_free_animation (LIMdlModel*     self,
                        LIMdlAnimation* animation)
{
	int i;

	if (animation->channels.array != NULL)
	{
		for (i = 0 ; i < animation->channels.count ; i++)
			lisys_free (animation->channels.array[i]);
		lisys_free (animation->channels.array);
	}
	lisys_free (animation->buffer.array);
	lisys_free (animation->name);
}

static int
private_read (LIMdlModel*  self,
              LIArcReader* reader)
{
	int i;
	int j;
	uint32_t tmp;
	uint32_t version;
	LIMatVector min;
	LIMatVector max;
	LIMdlFaces* group;

	/* Read magic. */
	if (!liarc_reader_check_text (reader, "lips/mdl", ""))
	{
		lisys_error_set (EINVAL, "wrong file format");
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

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp) ||
	    !liarc_reader_get_float (reader, &min.x) ||
	    !liarc_reader_get_float (reader, &min.y) ||
	    !liarc_reader_get_float (reader, &min.z) ||
	    !liarc_reader_get_float (reader, &max.x) ||
	    !liarc_reader_get_float (reader, &max.y) ||
	    !liarc_reader_get_float (reader, &max.z))
		return 0;
	self->flags = tmp;
	limat_aabb_init_from_points (&self->bounds, &min, &max);

	/* Read chunks. */
	if (!private_read_materials (self, reader) ||
	    !private_read_vertices (self, reader) ||
	    !private_read_faces (self, reader) ||
	    !private_read_weights (self, reader) ||
	    !private_read_nodes (self, reader) ||
	    !private_read_animations (self, reader) ||
	    !private_read_hairs (self, reader) ||
	    !private_read_shapes (self, reader))
		return 0;

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
	if (!liarc_reader_check_text (reader, "ani", ""))
	{
		lisys_error_set (EINVAL, "invalid animation block header");
		return 0;
	}
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
			if (!private_read_animation (self, animation, reader))
				return 0;
		}
	}

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
	if (!liarc_reader_check_text (reader, "fac", ""))
	{
		lisys_error_set (EINVAL, "invalid face block header");
		return 0;
	}
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
	if (!liarc_reader_check_text (reader, "hai", ""))
	{
		lisys_error_set (EINVAL, "invalid hair block header");
		return 0;
	}
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
	if (!liarc_reader_check_text (reader, "mat", ""))
	{
		lisys_error_set (EINVAL, "invalid material block header");
		return 0;
	}
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
	if (!liarc_reader_check_text (reader, "nod", ""))
	{
		lisys_error_set (EINVAL, "invalid node block header");
		return 0;
	}
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

static int
private_read_shapes (LIMdlModel*  self,
                     LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_check_text (reader, "sha", ""))
	{
		lisys_error_set (EINVAL, "invalid shape block header");
		return 0;
	}
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
	if (!liarc_reader_check_text (reader, "ver", ""))
	{
		lisys_error_set (EINVAL, "invalid vertex block header");
		return 0;
	}
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
				!liarc_reader_get_float (reader, vertex->texcoord + 2) ||
				!liarc_reader_get_float (reader, vertex->texcoord + 3) ||
				!liarc_reader_get_float (reader, vertex->texcoord + 4) ||
				!liarc_reader_get_float (reader, vertex->texcoord + 5) ||
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
	if (!liarc_reader_check_text (reader, "wei", ""))
	{
		lisys_error_set (EINVAL, "invalid weight block header");
		return 0;
	}
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
private_read_animation (LIMdlModel*     self,
                        LIMdlAnimation* animation,
                        LIArcReader*    reader)
{
	int i;
	uint32_t count0;
	uint32_t count1;
	LIMatTransform* transform;

	/* Read the header. */
	if (!liarc_reader_get_text (reader, "", &animation->name) ||
	    !liarc_reader_get_uint32 (reader, &count0) ||
	    !liarc_reader_get_uint32 (reader, &count1))
		return 0;

	/* Allocate channels. */
	animation->channels.count = count0;
	if (count0)
	{
		animation->channels.array = lisys_calloc (count0, sizeof (char*));
		if (animation->channels.array == NULL)
			return 0;
	}

	/* Read channels. */
	for (i = 0 ; i < animation->channels.count ; i++)
	{
		if (!liarc_reader_get_text (reader, "", animation->channels.array + i))
			return 0;
	}

	/* Allocate frames. */
	animation->length = count1;
	animation->buffer.count = count0 * count1;
	if (animation->buffer.count)
	{
		animation->buffer.array = lisys_calloc (animation->buffer.count, sizeof (LIMdlFrame));
		if (animation->buffer.array == NULL)
			return 0;
	}

	/* Read frames. */
	for (i = 0 ; i < animation->buffer.count ; i++)
	{
		transform = &animation->buffer.array[i].transform;
		if (!liarc_reader_get_float (reader, &transform->position.x) ||
			!liarc_reader_get_float (reader, &transform->position.y) ||
			!liarc_reader_get_float (reader, &transform->position.z) ||
			!liarc_reader_get_float (reader, &transform->rotation.x) ||
			!liarc_reader_get_float (reader, &transform->rotation.y) ||
			!liarc_reader_get_float (reader, &transform->rotation.z) ||
			!liarc_reader_get_float (reader, &transform->rotation.w))
			return 0;
	}

	return 1;
}

static int
private_read_vertex_weights (LIMdlModel*   self,
                             LIMdlWeights* weights,
                             LIArcReader*  reader)
{
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
		    !liarc_reader_get_float (reader, &weights->weights[i].weight))
			return 0;
		weights->weights[i].group = group;
	}

	return 1;
}

static int
private_write (const LIMdlModel* self,
               LIArcWriter*      writer)
{
	if (!liarc_writer_append_uint32 (writer, LIMDL_FORMAT_VERSION) ||
	    !liarc_writer_append_uint32 (writer, self->flags) ||
	    !liarc_writer_append_float (writer, self->bounds.min.x) ||
	    !liarc_writer_append_float (writer, self->bounds.min.y) ||
	    !liarc_writer_append_float (writer, self->bounds.min.z) ||
	    !liarc_writer_append_float (writer, self->bounds.max.x) ||
	    !liarc_writer_append_float (writer, self->bounds.max.y) ||
	    !liarc_writer_append_float (writer, self->bounds.max.z))
		return 0;
	if (!private_write_materials (self, writer) ||
	    !private_write_vertices (self, writer) ||
	    !private_write_faces (self, writer) ||
	    !private_write_weights (self, writer) ||
	    !private_write_nodes (self, writer) ||
	    !private_write_animations (self, writer) ||
	    !private_write_hairs (self, writer) ||
	    !private_write_shapes (self, writer))
		return 0;
	return 1;
}

static int
private_write_animations (const LIMdlModel* self,
                          LIArcWriter*      writer)
{
	int i;
	int j;
	LIMdlAnimation* animation;
	LIMatTransform* transform;

	/* Write animations. */
	if (!liarc_writer_append_string (writer, "ani") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->animations.count))
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

static int
private_write_faces (const LIMdlModel* self,
                     LIArcWriter*      writer)
{
	int i;

	if (!liarc_writer_append_string (writer, "fac") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->facegroups.count))
		return 0;
	for (i = 0 ; i < self->facegroups.count ; i++)
	{
		if (!limdl_faces_write (self->facegroups.array + i, writer))
			return 0;
	}

	return 1;
}

static int
private_write_hairs (const LIMdlModel* self,
                     LIArcWriter*      writer)
{
	int i;
	LIMdlHairs* hairs;

	/* Write animations. */
	if (!liarc_writer_append_string (writer, "hai") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->hairs.count))
		return 0;
	for (i = 0 ; i < self->hairs.count ; i++)
	{
		hairs = self->hairs.array + i;
		if (!limdl_hairs_write (hairs, writer))
			return 0;
	}

	return 1;
}

static int
private_write_materials (const LIMdlModel* self,
                         LIArcWriter*      writer)
{
	int i;
	LIMdlMaterial* material;

	if (!liarc_writer_append_string (writer, "mat") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->materials.count))
		return 0;
	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.array + i;
		if (!limdl_material_write (material, writer))
			return 0;
	}

	return 1;
}

static int
private_write_nodes (const LIMdlModel* self,
                     LIArcWriter*      writer)
{
	int i;
	LIMdlNode* node;

	/* Write nodes. */
	if (!liarc_writer_append_string (writer, "nod") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->nodes.count))
		return 0;
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		if (!limdl_node_write (node, writer))
			return 0;
	}

	return 1;
}

static int
private_write_shapes (const LIMdlModel* self,
                      LIArcWriter*      writer)
{
	int i;
	LIMdlShape* shape;

	/* Write nodes. */
	if (!liarc_writer_append_string (writer, "sha") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->shapes.count))
		return 0;
	for (i = 0 ; i < self->shapes.count ; i++)
	{
		shape = self->shapes.array + i;
		if (!limdl_shape_write (shape, writer))
			return 0;
	}

	return 1;
}

static int
private_write_vertices (const LIMdlModel* self,
                        LIArcWriter*      writer)
{
	int i;
	LIMdlVertex* vertex;

	/* Write header. */
	if (!liarc_writer_append_string (writer, "ver") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->vertices.count))
		return 0;

	/* Write vertices. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		vertex = self->vertices.array + i;
		if (!liarc_writer_append_float (writer, vertex->texcoord[0]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[1]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[2]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[3]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[4]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[5]) ||
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

static int
private_write_weights (const LIMdlModel* self,
                       LIArcWriter*      writer)
{
	int i;
	int j;
	LIMdlWeightGroup* group;
	LIMdlWeights* weights;

	/* Write header. */
	if (!liarc_writer_append_string (writer, "wei") ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, self->weightgroups.count))
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
