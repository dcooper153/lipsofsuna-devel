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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlModel Model
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "model.h"

static void
private_build (limdlModel* self);

static void
private_free_animation (limdlModel*     self,
                        limdlAnimation* animation);

static int
private_read (limdlModel*  self,
              liarcReader* reader);

static int
private_read_animations (limdlModel*  self,
                         liarcReader* reader);

static int
private_read_faces (limdlModel*  self,
                    liarcReader* reader);

static int
private_read_hairs (limdlModel*  self,
                    liarcReader* reader);

static int
private_read_materials (limdlModel*  self,
                        liarcReader* reader);

static int
private_read_nodes (limdlModel*  self,
                    liarcReader* reader);

static int
private_read_shapes (limdlModel*  self,
                     liarcReader* reader);

static int
private_read_vertices (limdlModel*  self,
                       liarcReader* reader);

static int
private_read_weights (limdlModel*  self,
                      liarcReader* reader);

static int
private_read_animation (limdlModel*     self,
                        limdlAnimation* animation,
                        liarcReader*    reader);

static int
private_read_vertex_weights (limdlModel*   self,
                             limdlWeights* weights,
                             liarcReader*  reader);

static int
private_write (const limdlModel* self,
               liarcWriter*      writer);

static int
private_write_animations (const limdlModel* self,
                          liarcWriter*      writer);

static int
private_write_faces (const limdlModel* self,
                     liarcWriter*      writer);

static int
private_write_hairs (const limdlModel* self,
                     liarcWriter*      writer);

static int
private_write_materials (const limdlModel* self,
                         liarcWriter*      writer);

static int
private_write_nodes (const limdlModel* self,
                     liarcWriter*      writer);

static int
private_write_shapes (const limdlModel* self,
                      liarcWriter*      writer);

static int
private_write_vertices (const limdlModel* self,
                        liarcWriter*      writer);

static int
private_write_weights (const limdlModel* self,
                       liarcWriter*      writer);

/*****************************************************************************/

limdlModel*
limdl_model_new ()
{
	limdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (limdlModel));
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
limdlModel*
limdl_model_new_from_data (liarcReader* reader)
{
	limdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (limdlModel));
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
limdlModel*
limdl_model_new_from_file (const char* path)
{
	liarcReader* reader;
	limdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (limdlModel));
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
limdl_model_free (limdlModel* self)
{
	int i;
	limdlMaterial* material;

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
limdl_model_calculate_bounds (limdlModel* self)
{
	int j;
	limatVector v;

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
limdl_model_find_material (const limdlModel*    self,
                           const limdlMaterial* material)
{
	int i;
	int j;
	const limdlMaterial* m;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		m = self->materials.array + i;
		if (m->flags == material->flags &&
		    m->shininess == material->shininess &&
		    m->diffuse[0] == material->diffuse[0] &&
		    m->diffuse[1] == material->diffuse[1] &&
		    m->diffuse[2] == material->diffuse[2] &&
		    m->diffuse[3] == material->diffuse[3] &&
		    m->specular[0] == material->specular[0] &&
		    m->specular[1] == material->specular[1] &&
		    m->specular[2] == material->specular[2] &&
		    m->specular[3] == material->specular[3] &&
		    m->textures.count == material->textures.count)
		{
			if (!strcmp (m->shader, material->shader))
			{
				for (j = 0 ; j < m->textures.count ; j++)
				{
					if (limdl_texture_compare (m->textures.array + j, material->textures.array + j))
						break;
				}
				if (j == m->textures.count)
					return i;
			}
		}
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
limdlNode*
limdl_model_find_node (const limdlModel* self,
                       const char*       name)
{
	int i;
	limdlNode* node;

	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		node = limdl_node_find_node (node, name);
		if (node != NULL)
			return node;
	}

	return NULL;
}

int
limdl_model_write (const limdlModel* self,
                   liarcWriter*      writer)
{
	liarc_writer_append_string (writer, "lips/mdl");
	liarc_writer_append_nul (writer);
	if (writer->error)
		return 0;
	return private_write (self, writer);
}

int
limdl_model_write_file (const limdlModel* self,
                        const char*       path)
{
	liarcWriter* writer;

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

/**
 * \brief Gets an animation by name.
 *
 * \param self A model.
 * \param name The name of the animation.
 * \return An animation or NULL.
 */
limdlAnimation*
limdl_model_get_animation (limdlModel* self,
                           const char* name)
{
	uint32_t i;

	for (i = 0 ; i < self->animations.count ; i++)
	{
		if (!strcmp (self->animations.array[i].name, name))
			return self->animations.array + i;
	}
	return NULL;
}

int
limdl_model_get_index_count (const limdlModel* self)
{
	int i;
	int c;

	for (i = c = 0 ; i < self->facegroups.count ; i++)
		c += self->facegroups.array[i].indices.count;

	return c;
}

/*****************************************************************************/

static void
private_build (limdlModel* self)
{
	int i;
	limdlNode* node;
	limdlWeightGroup* group;

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
private_free_animation (limdlModel*     self,
                        limdlAnimation* animation)
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
private_read (limdlModel*  self,
              liarcReader* reader)
{
	int i;
	int j;
	uint32_t tmp;
	uint32_t version;
	limatVector min;
	limatVector max;
	limdlFaces* group;

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
		lisys_error_set (LI_ERROR_VERSION, "model version mismatch");
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
private_read_animations (limdlModel*  self,
                         liarcReader* reader)
{
	int i;
	uint32_t tmp;
	limdlAnimation* animation;

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
		self->animations.array = lisys_calloc (self->animations.count, sizeof (limdlAnimation));
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
private_read_faces (limdlModel*  self,
                    liarcReader* reader)
{
	int i;
	uint32_t tmp;
	limdlFaces* group;

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
		self->facegroups.array = lisys_calloc (self->facegroups.count, sizeof (limdlFaces));
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
private_read_hairs (limdlModel*  self,
                    liarcReader* reader)
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
		self->hairs.array = lisys_calloc (self->hairs.count, sizeof (limdlHairs));
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
private_read_materials (limdlModel*  self,
                        liarcReader* reader)
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
		self->materials.array = lisys_calloc (self->materials.count, sizeof (limdlMaterial));
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
private_read_nodes (limdlModel*  self,
                    liarcReader* reader)
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
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (limdlNode*));
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
private_read_shapes (limdlModel*  self,
                     liarcReader* reader)
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
		self->shapes.array = lisys_calloc (self->shapes.count, sizeof (limdlShape));
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
private_read_vertices (limdlModel*  self,
                       liarcReader* reader)
{
	int i;
	uint32_t tmp;
	limdlVertex* vertex;

	/* Read header. */
	if (!liarc_reader_check_text (reader, "ver", ""))
	{
		lisys_error_set (EINVAL, "invalid vertex block header");
		return 0;
	}
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->vertices.count = tmp;

	/* Read vertices. */
	if (tmp)
	{
		self->vertices.array = lisys_calloc (tmp, sizeof (limdlVertex));
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
private_read_weights (limdlModel*  self,
                      liarcReader* reader)
{
	int i;
	uint32_t tmp[2];
	limdlWeightGroup* group;

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
		self->weightgroups.array = lisys_calloc (tmp[0], sizeof (limdlWeightGroup));
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
		self->weights.array = lisys_calloc (tmp[1], sizeof (limdlWeights));
		if (self->weights.array == NULL)
			return 0;
		self->weights.count = tmp[1];
		for (i = 0 ; i < self->weights.count ; i++)
		{
			if (!private_read_vertex_weights (self, self->weights.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int
private_read_animation (limdlModel*     self,
                        limdlAnimation* animation,
                        liarcReader*    reader)
{
	int i;
	uint32_t count0;
	uint32_t count1;
	limatTransform* transform;

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
		animation->buffer.array = lisys_calloc (animation->buffer.count, sizeof (limdlFrame));
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
private_read_vertex_weights (limdlModel*   self,
                             limdlWeights* weights,
                             liarcReader*  reader)
{
	uint32_t i;
	uint32_t count;
	uint32_t group;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &count))
		return 0;

	/* Allocate weights. */
	weights->count = count;
	weights->weights = lisys_calloc (count, sizeof (limdlWeight));
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
private_write (const limdlModel* self,
               liarcWriter*      writer)
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
private_write_animations (const limdlModel* self,
                          liarcWriter*      writer)
{
	int i;
	int j;
	limdlAnimation* animation;
	limatTransform* transform;

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
private_write_faces (const limdlModel* self,
                     liarcWriter*      writer)
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
private_write_hairs (const limdlModel* self,
                     liarcWriter*      writer)
{
	int i;
	limdlHairs* hairs;

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
private_write_materials (const limdlModel* self,
                         liarcWriter*      writer)
{
	int i;
	limdlMaterial* material;

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
private_write_nodes (const limdlModel* self,
                     liarcWriter*      writer)
{
	int i;
	limdlNode* node;

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
private_write_shapes (const limdlModel* self,
                      liarcWriter*      writer)
{
	int i;
	limdlShape* shape;

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
private_write_vertices (const limdlModel* self,
                        liarcWriter*      writer)
{
	int i;
	limdlVertex* vertex;

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
private_write_weights (const limdlModel* self,
                       liarcWriter*      writer)
{
	int i;
	int j;
	limdlWeightGroup* group;
	limdlWeights* weights;

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
