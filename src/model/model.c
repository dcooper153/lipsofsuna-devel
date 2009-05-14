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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string/lips-string.h>
#include "model.h"

static void
private_build (limdlModel* self);

static void
private_free_animation (limdlModel*     self,
                        limdlAnimation* animation);

static void
private_free_ipo (limdlModel* self,
                  limdlIpo*   ipo);

static int
private_read (limdlModel* self,
              liReader*   reader);

static int
private_read_armature (limdlModel* self,
                       liReader*   reader);

static int
private_read_hierarchy (limdlModel* self,
                        liReader*   reader);

static int
private_read_mesh (limdlModel* self,
                   liReader*   reader);

static int
private_read_animation (limdlModel*     self,
                        limdlAnimation* animation,
                        liReader*       reader);

static int
private_read_ipo (limdlModel* self,
                  limdlIpo*   ipo,
                  liReader*   reader);

static int
private_read_material (limdlModel*    self,
                       limdlMaterial* material,
                       liReader*      reader);

static int
private_read_weight (limdlModel*  self,
                     limdlWeight* weight,
                     liReader*    reader);

static int
private_read_weights (limdlModel*   self,
                      limdlWeights* weights,
                      liReader*     reader);

static int
private_read_weightgroup (limdlModel*       self,
                          limdlWeightGroup* group,
                          liReader*         reader);

static int
private_write (const limdlModel* self,
               liarcWriter*      writer);

static int
private_write_armature (const limdlModel* self,
                        liarcWriter*      writer);

static int
private_write_hierarchy (const limdlModel* self,
                         liarcWriter*      writer);

static int
private_write_mesh (const limdlModel* self,
                    liarcWriter*      writer);

static void
private_write_animation (const limdlModel* self,
                         limdlAnimation*   animation,
                         liarcWriter*      writer);

static void
private_write_ipo (const limdlModel* self,
                   limdlIpo*         ipo,
                   liarcWriter*      writer);

static void
private_write_material (const limdlModel* self,
                        limdlMaterial*    material,
                        liarcWriter*      writer);

static void
private_write_weight (const limdlModel* self,
                      limdlWeight*      weight,
                      liarcWriter*      writer);

static void
private_write_weightgroup (const limdlModel* self,
                           limdlWeightGroup* group,
                           liarcWriter*      writer);

static void
private_write_weights (const limdlModel* self,
                       limdlWeights*     weights,
                       liarcWriter*      writer);

/*****************************************************************************/

limdlModel*
limdl_model_new ()
{
	limdlModel* self;

	/* Allocate self. */
	self = calloc (1, sizeof (limdlModel));
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
limdl_model_new_from_data (liReader* reader)
{
	limdlModel* self;

	/* Allocate self. */
	self = calloc (1, sizeof (limdlModel));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
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
	liReader* reader;
	limdlModel* self;

	/* Allocate self. */
	self = calloc (1, sizeof (limdlModel));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lisys_error_append ("cannot load model `%s'", path);
		return NULL;
	}

	/* Open the file. */
	reader = li_reader_new_from_file (path);
	if (reader == NULL)
		goto error;

	/* Read from stream. */
	if (!private_read (self, reader))
		goto error;
	li_reader_free (reader);

	/* Construct the rest pose. */
	private_build (self);
	return self;

error:
	lisys_error_append ("cannot load model `%s'", path);
	limdl_model_free (self);
	if (reader != NULL)
		li_reader_free (reader);
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
	int j;
	limdlMaterial* material;

	/* Free material groups. */
	if (self->materials.materials != NULL)
	{
		for (i = 0 ; i < self->materials.count ; i++)
		{
			material = self->materials.materials + i;
			free (material->shader);
			if (material->textures.textures != NULL)
			{
				for (j = 0 ; j < material->textures.count ; j++)
					free (material->textures.textures[j].string);
				free (material->textures.textures);
			}
		}
		free (self->materials.materials);
	}

	/* Free weight groups. */
	if (self->weightgroups.weightgroups != NULL)
	{
		for (i = 0 ; i < self->weightgroups.count ; i++)
			free (self->weightgroups.weightgroups[i].name);
		free (self->weightgroups.weightgroups);
	}

	/* Free nodes. */
	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		free (self->nodes.array);
	}

	/* Free animations. */
	if (self->animation.animations != NULL)
	{
		for (i = 0 ; i < self->animation.count ; i++)
			private_free_animation (self, self->animation.animations + i);
		free (self->animation.animations);
	}

	/* Free weights. */
	if (self->vertex.weights != NULL)
	{
		for (i = 0 ; i < self->vertex.count ; i++)
			free (self->vertex.weights[i].weights);
		free (self->vertex.weights);
	}

	/* Free the rest. */
	free (self->vertex.vertices);
	free (self);
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
		m = self->materials.materials + i;
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
					if (limdl_texture_compare (m->textures.textures + j, material->textures.textures + j))
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

/**
 * \brief Inserts a material to the model.
 *
 * \param self Model.
 * \param material Material.
 * \return Nonzero on success.
 */
int
limdl_model_insert_material (limdlModel*          self,
                             const limdlMaterial* material)
{
	int i;
	int count;
	limdlMaterial* tmp;

	assert (material->shader != NULL);

	/* Resize buffer. */
	count = self->materials.count + 1;
	tmp = realloc (self->materials.materials, count * sizeof (limdlMaterial));
	if (tmp == NULL)
		return 0;
	self->materials.materials = tmp;
	tmp += self->materials.count;

	/* Clear range. */
	tmp->start = count > 1? (tmp - 1)->end : 0;
	tmp->end = tmp->start;

	/* Copy attributes. */
	tmp->shader = strdup (material->shader);
	if (tmp->shader == NULL)
		return 0;
	tmp->flags = material->flags;
	tmp->shininess = material->shininess;
	tmp->diffuse[0] = material->diffuse[0];
	tmp->diffuse[1] = material->diffuse[1];
	tmp->diffuse[2] = material->diffuse[2];
	tmp->diffuse[3] = material->diffuse[3];
	tmp->specular[0] = material->specular[0];
	tmp->specular[1] = material->specular[1];
	tmp->specular[2] = material->specular[2];
	tmp->specular[3] = material->specular[3];

	/* Copy textures. */
	tmp->textures.count = material->textures.count;
	if (tmp->textures.count)
		tmp->textures.textures = calloc (tmp->textures.count, sizeof (limdlTexture));
	else
		tmp->textures.textures = NULL;
	for (i = 0 ; i < tmp->textures.count ; i++)
	{
		tmp->textures.textures[i] = material->textures.textures[i];
		tmp->textures.textures[i].string = strdup (material->textures.textures[i].string);
		if (tmp->textures.textures[i].string == NULL)
		{
			while (i)
				free (tmp->textures.textures[i--].string);
			free (tmp->textures.textures);
			free (tmp->shader);
			return 0;
		}
	}

	self->materials.count++;
	return 1;
}

/**
 * \brief Inserts a node to the model.
 *
 * The ownership of the node is given to the model on success.
 *
 * \param self Model.
 * \param node Node.
 * \return Nonzero on success.
 */
int
limdl_model_insert_node (limdlModel* self,
                         limdlNode*  node)
{
	if (!lialg_array_append (&self->nodes, &node))
		return 0;
	node->parent = NULL;
	return 1;
}

/**
 * \brief Inserts a face to the model.
 *
 * Inserts vertices and their weights to the material group requested.
 *
 * \param self Model.
 * \param material Material index.
 * \param vertices Array of three vertices.
 * \param weights Array of three weights or NULL.
 * \return Nonzero on success.
 */
int
limdl_model_insert_triangle (limdlModel*         self,
                             int                 material,
                             const limdlVertex*  vertices,
                             const limdlWeights* weights)
{
	int i;
	int count;
	int offset;
	limdlVertex* vertex;
	limdlWeights* weight;
	limdlWeights weightbuf[3] = { { 0, NULL }, { 0, NULL }, { 0, NULL } };

	assert (material >= 0);
	assert (material < self->materials.count);

	/* Duplicate weights. */
	if (weights != NULL)
	{
		for (i = 0 ; i < 3 ; i++)
		{
			if (!weights[0].count)
				continue;
			weightbuf[i].weights = malloc (weights[i].count * sizeof (limdlWeight));
			if (weightbuf[i].weights == NULL)
				goto error;
			memcpy (weightbuf[i].weights, weights[i].weights, weights[i].count * sizeof (limdlWeight));
			weightbuf[i].count = weights[i].count;
		}
	}

	/* Resize vertex buffer. */
	count = self->vertex.count + 3;
	vertex = realloc (self->vertex.vertices, count * sizeof (limdlVertex));
	if (vertex == NULL)
		goto error;
	self->vertex.vertices = vertex;

	/* Resize weight buffer. */
	weight = realloc (self->vertex.weights, count * sizeof (limdlWeights));
	if (weight == NULL)
		goto error;
	self->vertex.weights = weight;

	/* Shift vertices. */
	offset = 3 * self->materials.materials[material].end;
	for (i = count - 1 ; i >= offset + 3 ; i--)
	{
		memcpy (vertex + i, vertex + i - 3, sizeof (limdlVertex));
		memcpy (weight + i, weight + i - 3, sizeof (limdlWeights));
	}

	/* Insert vertices. */
	memcpy (vertex + offset, vertices, 3 * sizeof (limdlVertex));
	memcpy (weight + offset, weightbuf, 3 * sizeof (limdlWeights));
	self->vertex.count += 3;

	/* Update material groups. */
	self->materials.materials[material].end++;
	for (i = material + 1 ; i < self->materials.count ; i++)
	{
		self->materials.materials[i].start++;
		self->materials.materials[i].end++;
	}

	return 1;

error:
	free (weightbuf[0].weights);
	free (weightbuf[1].weights);
	free (weightbuf[2].weights);
	return 0;
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
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	/* Pack to the writer. */
	if (!limdl_model_write (self, writer))
	{
		lisys_error_set (ENOMEM, NULL);
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

	for (i = 0 ; i < self->animation.count ; i++)
	{
		if (!strcmp (self->animation.animations[i].name, name))
			return self->animation.animations + i;
	}
	return NULL;
}

/**
 * \brief Finds a vertex that matches the passed vertex exactly.
 *
 * \param self Model.
 * \param vertex Vertex to match against.
 * \return A vertex index or -1.
 */
int
limdl_model_get_index (const limdlModel*  self,
                       const limdlVertex* vertex)
{
	uint32_t i;

	for (i = 0 ; i < self->vertex.count ; i++)
	{
		if (!limdl_vertex_compare (vertex, self->vertex.vertices + i))
			return i;
	}

	return -1;
}

/*****************************************************************************/

static void
private_build (limdlModel* self)
{
	int i;
	limatTransform rest;
	limdlNode* node;
	limdlWeightGroup* group;

	/* Resolve node references. */
	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		group = self->weightgroups.weightgroups + i;
		group->node = limdl_model_find_node (self, group->bone);
	}

	/* Transform each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		rest = limat_transform_identity ();
		limdl_node_transform (node, &rest, &rest);
	}
}

static void
private_free_animation (limdlModel*     self,
                        limdlAnimation* animation)
{
	int i;

	if (animation->ipos.array != NULL)
	{
		for (i = 0 ; i < animation->ipos.count ; i++)
			private_free_ipo (self, animation->ipos.array + i);
		free (animation->ipos.array);
	}
	free (animation->name);
}

static void
private_free_ipo (limdlModel* self,
                  limdlIpo*   ipo)
{
	int i;

	for (i = 0 ; i < LIMDL_IPO_CHANNEL_NUM ; i++)
		free (ipo->channels[i].nodes);
	free (ipo->name);
}

static int
private_read (limdlModel* self,
              liReader*   reader)
{
	uint32_t version;

	/* Read magic. */
	if (!li_reader_check_text (reader, "lips/mdl", ""))
	{
		lisys_error_set (EINVAL, "wrong file format");
		goto error;
	}

	/* Read version. */
	if (!li_reader_get_uint32 (reader, &version))
		goto error;
	if (version != LIMDL_FORMAT_VERSION)
	{
		lisys_error_set (LI_ERROR_VERSION, "model version mismatch");
		goto error;
	}

	/* Read chunks. */
	if (!private_read_mesh (self, reader) ||
	    !private_read_hierarchy (self, reader) ||
	    !private_read_armature (self, reader))
		goto error;

	return 1;

error:
	return 0;
}

static int
private_read_armature (limdlModel* self,
                       liReader*   reader)
{
	int i;
	uint32_t tmp;
	limdlAnimation* animation;

	if (!li_reader_get_uint32 (reader, &tmp))
		goto error;
	self->animation.count = tmp;

	/* Read animations. */
	if (self->animation.count)
	{
		self->animation.animations = calloc (self->animation.count, sizeof (limdlAnimation));
		if (self->animation.animations == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (i = 0 ; i < self->animation.count ; i++)
		{
			animation = self->animation.animations + i;
			if (!private_read_animation (self, animation, reader))
				goto error;
		}
	}

	return 1;

error:
	return 0;
}

static int
private_read_hierarchy (limdlModel* self,
                        liReader*   reader)
{
	int i;
	uint32_t tmp;

	if (!li_reader_get_uint32 (reader, &tmp))
		goto error;
	self->nodes.count = tmp;

	/* Read nodes. */
	if (self->nodes.count)
	{
		self->nodes.array = calloc (self->nodes.count, sizeof (limdlNode*));
		if (self->nodes.array == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_new (self);
			if (self->nodes.array[i] == NULL)
			{
				lisys_error_set (ENOMEM, NULL);
				goto error;
			}
			if (!limdl_node_read (self->nodes.array[i], reader))
				goto error;
		}
	}

	return 1;

error:
	return 0;
}

static int
private_read_mesh (limdlModel* self,
                   liReader*   reader)
{
	int i;
	uint32_t tmp[4];
	limatVector min;
	limatVector max;
	limdlVertex* vertex;

	if (!li_reader_get_uint32 (reader, tmp + 0) ||
	    !li_reader_get_uint32 (reader, tmp + 1) ||
	    !li_reader_get_uint32 (reader, tmp + 2) ||
	    !li_reader_get_uint32 (reader, tmp + 3))
		goto error;
	self->flags = tmp[0];
	self->vertex.count = tmp[1];
	self->materials.count = tmp[2];
	self->weightgroups.count = tmp[3];

	/* Read the bounding box. */
	if (!li_reader_get_float (reader, &min.x) ||
	    !li_reader_get_float (reader, &min.y) ||
	    !li_reader_get_float (reader, &min.z) ||
	    !li_reader_get_float (reader, &max.x) ||
	    !li_reader_get_float (reader, &max.y) ||
	    !li_reader_get_float (reader, &max.z))
		goto error;
	limat_aabb_init_from_points (&self->bounds, &min, &max);

	/* Read vertices. */
	if (self->vertex.count)
	{
		self->vertex.vertices = calloc (self->vertex.count, sizeof (limdlVertex));
		if (self->vertex.vertices == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (i = 0 ; i < self->vertex.count ; i++)
		{
			vertex = self->vertex.vertices + i;
			if (!li_reader_get_float (reader, vertex->texcoord + 0) ||
				!li_reader_get_float (reader, vertex->texcoord + 1) ||
				!li_reader_get_float (reader, vertex->texcoord + 2) ||
				!li_reader_get_float (reader, vertex->texcoord + 3) ||
				!li_reader_get_float (reader, vertex->texcoord + 4) ||
				!li_reader_get_float (reader, vertex->texcoord + 5) ||
				!li_reader_get_float (reader, &vertex->normal.x) ||
				!li_reader_get_float (reader, &vertex->normal.y) ||
				!li_reader_get_float (reader, &vertex->normal.z) ||
				!li_reader_get_float (reader, &vertex->coord.x) ||
				!li_reader_get_float (reader, &vertex->coord.y) ||
				!li_reader_get_float (reader, &vertex->coord.z))
				goto error;
		}
	}

	/* Read materials. */
	if (self->materials.count)
	{
		self->materials.materials = calloc (self->materials.count, sizeof (limdlMaterial));
		if (self->materials.materials == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (i = 0 ; i < self->materials.count ; i++)
		{
			if (!private_read_material (self, self->materials.materials + i, reader))
				goto error;
		}
	}

	/* Read weight groups. */
	if (self->weightgroups.count)
	{
		self->weightgroups.weightgroups = calloc (self->weightgroups.count, sizeof (limdlWeightGroup));
		if (self->weightgroups.weightgroups == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (i = 0 ; i < self->weightgroups.count ; i++)
		{
			if (!private_read_weightgroup (self, self->weightgroups.weightgroups + i, reader))
				goto error;
		}
	}

	/* Read vertex weights. */
	if (self->vertex.count)
	{
		self->vertex.weights = calloc (self->vertex.count, sizeof (limdlWeights));
		if (self->vertex.weights == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			goto error;
		}
		for (i = 0 ; i < self->vertex.count ; i++)
		{
			if (!private_read_weights (self, self->vertex.weights + i, reader))
				goto error;
		}
	}

	return 1;

error:
	return 0;
}

static int
private_read_animation (limdlModel*     self,
                        limdlAnimation* animation,
                        liReader*       reader)
{
	int i;
	uint32_t count;

	/* Read the header. */
	if (!li_reader_get_text (reader, "", &animation->name) ||
	    !li_reader_get_uint32 (reader, &count) ||
	    !li_reader_get_float (reader, &animation->duration) ||
	    !li_reader_get_float (reader, &animation->blendin) ||
	    !li_reader_get_float (reader, &animation->blendout))
		return 0;

	/* Allocate curves. */
	animation->ipos.count = count;
	if (count)
	{
		animation->ipos.array = calloc (count, sizeof (limdlIpo));
		if (animation->ipos.array == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			return 0;
		}
	}

	/* Read curves. */
	for (i = 0 ; i < animation->ipos.count ; i++)
	{
		if (!private_read_ipo (self, animation->ipos.array + i, reader))
			return 0;
	}

	return 1;
}

static int
private_read_ipo (limdlModel* self,
                  limdlIpo*   ipo,
                  liReader*   reader)
{
	int i;
	int j;
	int channels;
	uint32_t tmp[2];

	/* Read the number of channels. */
	if (!li_reader_get_text (reader, "", &ipo->name) ||
	    !li_reader_get_uint32 (reader, tmp))
		return 0;
	channels = tmp[0];
	if (channels != LIMDL_IPO_CHANNEL_NUM)
	{
		lisys_error_set (EINVAL, "invalid ipo channel");
		return 0;
	}

	/* Read each channel. */
	for (i = 0 ; i < channels ; i++)
	{
		/* Read the header. */
		if (!li_reader_get_uint32 (reader, tmp + 0) ||
		    !li_reader_get_uint32 (reader, tmp + 1))
			return 0;
		ipo->channels[i].type = tmp[0];
		ipo->channels[i].length = tmp[1];

		if (ipo->channels[i].length)
		{
			/* Allocate memory. */
			ipo->channels[i].nodes = calloc (ipo->channels[i].length, sizeof (float));
			if (ipo->channels[i].nodes == NULL)
			{
				lisys_error_set (ENOMEM, NULL);
				return 0;
			}

			/* Read the data. */
			for (j = 0 ; j < ipo->channels[i].length ; j++)
			{
				if (!li_reader_get_float (reader, ipo->channels[i].nodes + j))
					return 0;
			}
		}
	}
	
	return 1;
}

static int
private_read_material (limdlModel*    self,
                       limdlMaterial* material,
                       liReader*      reader)
{
	int i;
	uint32_t tmp[4];

	/* Read header. */
	if (!li_reader_get_uint32 (reader, tmp + 0) ||
		!li_reader_get_uint32 (reader, tmp + 1) ||
		!li_reader_get_uint32 (reader, tmp + 2) ||
		!li_reader_get_float (reader, &material->emission) ||
		!li_reader_get_float (reader, &material->shininess) ||
		!li_reader_get_float (reader, material->diffuse + 0) ||
		!li_reader_get_float (reader, material->diffuse + 1) ||
		!li_reader_get_float (reader, material->diffuse + 2) ||
		!li_reader_get_float (reader, material->diffuse + 3) ||
		!li_reader_get_float (reader, material->specular + 0) ||
		!li_reader_get_float (reader, material->specular + 1) ||
		!li_reader_get_float (reader, material->specular + 2) ||
		!li_reader_get_float (reader, material->specular + 3) ||
		!li_reader_get_uint32 (reader, tmp + 3))
		return 0;
	material->start = tmp[0];
	material->end = tmp[1];
	material->flags = tmp[2];
	material->textures.count = tmp[3];

	/* Sanity checks. */
	if (material->start == material->end)
	{
		lisys_error_set (EINVAL, "unused material");
		return 0;
	}
	if (material->start > material->end)
	{
		lisys_error_set (EINVAL, "invalid material range");
		return 0;
	}
	if (material->start >= self->vertex.count / 3 ||
		material->end > self->vertex.count / 3)
	{
		lisys_error_set (EINVAL, "material range out of bounds");
		return 0;
	}

	/* Read shader. */
	if (!li_reader_get_text (reader, "", &material->shader))
		return 0;

	/* Read textures. */
	if (material->textures.count > 0)
	{
		material->textures.textures = calloc (material->textures.count, sizeof (limdlTexture));
		if (material->textures.textures == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			return 0;
		}
		for (i = 0 ; i < material->textures.count ; i++)
		{
			if (!li_reader_get_uint32 (reader, tmp + 0) ||
			    !li_reader_get_uint32 (reader, tmp + 1) ||
			    !li_reader_get_uint32 (reader, tmp + 2) ||
			    !li_reader_get_uint32 (reader, tmp + 3) ||
			    !li_reader_get_text (reader, "", &material->textures.textures[i].string))
				return 0;
			material->textures.textures[i].type = tmp[0];
			material->textures.textures[i].flags = tmp[1];
			material->textures.textures[i].width = tmp[2];
			material->textures.textures[i].height = tmp[3];
		}
	}

	return 1;
}

static int
private_read_weight (limdlModel*  self,
                     limdlWeight* weight,
                     liReader*    reader)
{
	uint32_t group;

	if (!li_reader_get_uint32 (reader, &group) ||
	    !li_reader_get_float (reader, &weight->weight))
		return 0;
	if (group >= self->weightgroups.count)
	{
		lisys_error_set (EINVAL, "reference to invalid weight group %d", group);
		return 0;
	}
	weight->group = group;

	return 1;
}

static int
private_read_weightgroup (limdlModel*       self,
                          limdlWeightGroup* group,
                          liReader*         reader)
{
	if (!li_reader_get_text (reader, "", &group->name) ||
	    !li_reader_get_text (reader, "", &group->bone))
		return 0;

	return 1;
}

static int
private_read_weights (limdlModel*   self,
                      limdlWeights* weights,
                      liReader*     reader)
{
	uint32_t i;
	uint32_t count;

	/* Read header. */
	if (!li_reader_get_uint32 (reader, &count))
		return 0;

	/* Allocate weights. */
	weights->count = count;
	weights->weights = calloc (count, sizeof (limdlWeight));
	if (weights->weights == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	/* Read weights. */
	for (i = 0 ; i < count ; i++)
	{
		if (!private_read_weight (self, weights->weights + i, reader))
			return 0;
	}

	return 1;
}

static int
private_write (const limdlModel* self,
               liarcWriter*      writer)
{
	liarc_writer_append_uint32 (writer, LIMDL_FORMAT_VERSION);
	if (writer->error)
		return 0;
	if (!private_write_mesh (self, writer) ||
	    !private_write_hierarchy (self, writer) ||
	    !private_write_armature (self, writer))
		return 0;
	return 1;
}

static int
private_write_armature (const limdlModel* self,
                        liarcWriter*      writer)
{
	int i;
	limdlAnimation* animation;

	/* Write animations. */
	liarc_writer_append_uint32 (writer, self->animation.count);
	for (i = 0 ; i < self->animation.count ; i++)
	{
		animation = self->animation.animations + i;
		private_write_animation (self, animation, writer);
	}

	return !writer->error;
}

static int
private_write_hierarchy (const limdlModel* self,
                         liarcWriter*      writer)
{
	int i;
	limdlNode* node;

	/* Write nodes. */
	liarc_writer_append_uint32 (writer, self->nodes.count);
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		limdl_node_write (node, writer);
	}

	return !writer->error;
}

static int
private_write_mesh (const limdlModel* self,
                    liarcWriter*      writer)
{
	int i;
	limdlMaterial* material;
	limdlVertex* vertex;

	liarc_writer_append_uint32 (writer, self->flags);
	liarc_writer_append_uint32 (writer, self->vertex.count);
	liarc_writer_append_uint32 (writer, self->materials.count);
	liarc_writer_append_uint32 (writer, self->weightgroups.count);
	liarc_writer_append_float (writer, self->bounds.min.x);
	liarc_writer_append_float (writer, self->bounds.min.y);
	liarc_writer_append_float (writer, self->bounds.min.z);
	liarc_writer_append_float (writer, self->bounds.max.x);
	liarc_writer_append_float (writer, self->bounds.max.y);
	liarc_writer_append_float (writer, self->bounds.max.z);

	/* Write vertices. */
	for (i = 0 ; i < self->vertex.count ; i++)
	{
		vertex = self->vertex.vertices + i;
		liarc_writer_append_float (writer, vertex->texcoord[0]);
		liarc_writer_append_float (writer, vertex->texcoord[1]);
		liarc_writer_append_float (writer, vertex->texcoord[2]);
		liarc_writer_append_float (writer, vertex->texcoord[3]);
		liarc_writer_append_float (writer, vertex->texcoord[4]);
		liarc_writer_append_float (writer, vertex->texcoord[5]);
		liarc_writer_append_float (writer, vertex->normal.x);
		liarc_writer_append_float (writer, vertex->normal.y);
		liarc_writer_append_float (writer, vertex->normal.z);
		liarc_writer_append_float (writer, vertex->coord.x);
		liarc_writer_append_float (writer, vertex->coord.y);
		liarc_writer_append_float (writer, vertex->coord.z);
	}

	/* Write materials. */
	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.materials + i;
		private_write_material (self, material, writer);
	}

	/* Write weight groups. */
	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		private_write_weightgroup (self, self->weightgroups.weightgroups + i, writer);
	}

	/* Write vertex weights. */
	for (i = 0 ; i < self->vertex.count ; i++)
	{
		private_write_weights (self, self->vertex.weights + i, writer);
	}

	return !writer->error;
}

static void
private_write_animation (const limdlModel* self,
                         limdlAnimation*   animation,
                         liarcWriter*      writer)
{
	int i;
	limdlIpo* ipo;

	liarc_writer_append_string (writer, animation->name);
	liarc_writer_append_nul (writer);
	liarc_writer_append_uint32 (writer, animation->ipos.count);
	liarc_writer_append_float (writer, animation->duration);
	liarc_writer_append_float (writer, animation->blendin);
	liarc_writer_append_float (writer, animation->blendout);
	for (i = 0 ; i < animation->ipos.count ; i++)
	{
		ipo = animation->ipos.array + i;
		private_write_ipo (self, ipo, writer);
	}
}

static void
private_write_ipo (const limdlModel* self,
                   limdlIpo*         ipo,
                   liarcWriter*      writer)
{
	uint32_t i;
	uint32_t j;

	liarc_writer_append_string (writer, ipo->name);
	liarc_writer_append_nul (writer);
	liarc_writer_append_uint32 (writer, LIMDL_IPO_CHANNEL_NUM);
	for (i = 0 ; i < LIMDL_IPO_CHANNEL_NUM ; i++)
	{
		liarc_writer_append_uint32 (writer, ipo->channels[i].type);
		liarc_writer_append_uint32 (writer, ipo->channels[i].length);
		for (j = 0 ; j < ipo->channels[i].length ; j++)
		{
			liarc_writer_append_float (writer, ipo->channels[i].nodes[j]);
		}
	}
}

static void
private_write_material (const limdlModel* self,
                        limdlMaterial*    material,
                        liarcWriter*      writer)
{
	int i;
	limdlTexture* texture;

	liarc_writer_append_uint32 (writer, material->start);
	liarc_writer_append_uint32 (writer, material->end);
	liarc_writer_append_uint32 (writer, material->flags);
	liarc_writer_append_float (writer, material->emission);
	liarc_writer_append_float (writer, material->shininess);
	liarc_writer_append_float (writer, material->diffuse[0]);
	liarc_writer_append_float (writer, material->diffuse[1]);
	liarc_writer_append_float (writer, material->diffuse[2]);
	liarc_writer_append_float (writer, material->diffuse[3]);
	liarc_writer_append_float (writer, material->specular[0]);
	liarc_writer_append_float (writer, material->specular[1]);
	liarc_writer_append_float (writer, material->specular[2]);
	liarc_writer_append_float (writer, material->specular[3]);
	liarc_writer_append_uint32 (writer, material->textures.count);
	liarc_writer_append_string (writer, material->shader);
	liarc_writer_append_nul (writer);
	for (i = 0 ; i < material->textures.count ; i++)
	{
		texture = material->textures.textures + i;
		liarc_writer_append_uint32 (writer, texture->type);
		liarc_writer_append_uint32 (writer, texture->flags);
		liarc_writer_append_uint32 (writer, texture->width);
		liarc_writer_append_uint32 (writer, texture->height);
		liarc_writer_append_string (writer, texture->string);
		liarc_writer_append_nul (writer);
	}
}

static void
private_write_weight (const limdlModel* self,
                      limdlWeight*      weight,
                      liarcWriter*      writer)
{
	liarc_writer_append_uint32 (writer, weight->group);
	liarc_writer_append_float (writer, weight->weight);
}

static void
private_write_weightgroup (const limdlModel* self,
                           limdlWeightGroup* group,
                           liarcWriter*      writer)
{
	liarc_writer_append_string (writer, group->name);
	liarc_writer_append_nul (writer);
	liarc_writer_append_string (writer, group->bone);
	liarc_writer_append_nul (writer);
}

static void
private_write_weights (const limdlModel* self,
                       limdlWeights*     weights,
                       liarcWriter*      writer)
{
	int i;

	liarc_writer_append_uint32 (writer, weights->count);
	for (i = 0 ; i < weights->count ; i++)
		private_write_weight (self, weights->weights + i, writer);
}

/** @} */
/** @} */
