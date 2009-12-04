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
 * \addtogroup limdlNode Node
 * @{
 */

#include <system/lips-system.h>
#include "model-node.h"

static void
private_calculate_world_transform (limdlNode* self);

/*****************************************************************************/

limdlNode*
limdl_node_new (limdlModel* model)
{
	limdlNode* self;

	self = lisys_calloc (1, sizeof (limdlNode));
	if (self == NULL)
		return NULL;
	self->model = model;
	self->transform.rest = limat_transform_identity ();
	self->transform.local = limat_transform_identity ();
	self->transform.global = limat_transform_identity ();

	return self;
}

limdlNode*
limdl_node_copy (const limdlNode* node)
{
	int i;
	limdlNode* self;

	self = lisys_calloc (1, sizeof (limdlNode));
	if (self == NULL)
		return NULL;
	self->type = node->type;
	self->model = node->model;
	self->transform.rest = node->transform.rest;
	self->transform.local = node->transform.local;
	self->transform.global = node->transform.global;

	/* Copy name. */
	if (node->name != NULL)
	{
		self->name = listr_dup (node->name);
		if (self->name == NULL)
			goto error;
	}

	/* Copy type specific data. */
	switch (node->type)
	{
		case LIMDL_NODE_BONE:
			self->bone = node->bone;
			break;
		case LIMDL_NODE_EMPTY:
			break;
		case LIMDL_NODE_LIGHT:
			self->light = node->light;
			break;
		default:
			assert (0);
			break;
	}

	/* Copy child nodes. */
	if (node->nodes.count)
	{
		self->nodes.count = node->nodes.count;
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (limdlNode*));
		if (self->nodes.array == NULL)
			goto error;
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_copy (node->nodes.array[i]);
			if (self->nodes.array[i] == NULL)
				goto error;
			self->nodes.array[i]->parent = self;
		}
	}

	return self;

error:
	limdl_node_free (self);
	return NULL;
}

void
limdl_node_free (limdlNode* self)
{
	int i;

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

	lisys_free (self->name);
	lisys_free (self);
}

limdlNode*
limdl_node_find_node (const limdlNode* self,
                      const char*      name)
{
	int i;
	limdlNode* tmp;

	/* Test self. */
	if (self->name != NULL && !strcmp (self->name, name))
		return (limdlNode*) self;

	/* Test children. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		if (self->nodes.array[i] != NULL)
		{
			tmp = limdl_node_find_node (self->nodes.array[i], name);
			if (tmp != NULL)
				return tmp;
		}
	}

	return NULL;
}

int
limdl_node_read (limdlNode*   self,
                 liarcReader* reader)
{
	int i;
	uint32_t count;
	uint32_t type;
	limatVector position;
	limatQuaternion rotation;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &type) ||
	    !liarc_reader_get_uint32 (reader, &count) ||
	    !liarc_reader_get_text (reader, "", &self->name) ||
	    !liarc_reader_get_float (reader, &position.x) ||
	    !liarc_reader_get_float (reader, &position.y) ||
	    !liarc_reader_get_float (reader, &position.z) ||
	    !liarc_reader_get_float (reader, &rotation.x) ||
	    !liarc_reader_get_float (reader, &rotation.y) ||
	    !liarc_reader_get_float (reader, &rotation.z) ||
	    !liarc_reader_get_float (reader, &rotation.w))
		return 0;
	self->type = type;
	self->nodes.count = count;
	self->transform.rest = limat_transform_init (position, rotation);
	self->transform.local = limat_transform_identity ();
	self->transform.global = limat_transform_init (position, rotation);

	/* Read type sepecific data. */
	switch (type)
	{
		case LIMDL_NODE_BONE:
			if (!limdl_bone_read (self, reader))
				return 0;
			break;
		case LIMDL_NODE_EMPTY:
			break;
		case LIMDL_NODE_LIGHT:
			if (!limdl_light_read (self, reader))
				return 0;
			break;
		default:
			lisys_error_set (EINVAL, "invalid node type %d", type);
			return 0;
	}

	/* Read child nodes. */
	if (self->nodes.count)
	{
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (limdlNode*));
		if (self->nodes.array == NULL)
			return 0;
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_new (self->model);
			if (self->nodes.array[i] == NULL)
				return 0;
			self->nodes.array[i]->parent = self;
			if (!limdl_node_read (self->nodes.array[i], reader))
				return 0;
		}
	}

	return 1;
}

/**
 * \brief Updates the world space transformation of the node and, optionally, its children.
 *
 * \param self Node.
 * \param recursive Nonzero if children should be rebuilt.
 */
void
limdl_node_rebuild (limdlNode* self,
                    int        recursive)
{
	int i;

	private_calculate_world_transform (self);
	if (recursive)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
			limdl_node_rebuild (self->nodes.array[i], 1);
	}
}

int
limdl_node_write (const limdlNode* self,
                  liarcWriter*     writer)
{
	int i;
	int count = 0;

	/* Count the real number of nodes. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		if (self->nodes.array[i] != NULL)
			count++;
	}

	/* Write header. */
	liarc_writer_append_uint32 (writer, self->type);
	liarc_writer_append_uint32 (writer, count);
	if (self->name != NULL)
		liarc_writer_append_string (writer, self->name);
	liarc_writer_append_nul (writer);
	liarc_writer_append_float (writer, self->transform.rest.position.x);
	liarc_writer_append_float (writer, self->transform.rest.position.y);
	liarc_writer_append_float (writer, self->transform.rest.position.z);
	liarc_writer_append_float (writer, self->transform.rest.rotation.x);
	liarc_writer_append_float (writer, self->transform.rest.rotation.y);
	liarc_writer_append_float (writer, self->transform.rest.rotation.z);
	liarc_writer_append_float (writer, self->transform.rest.rotation.w);

	/* Write type sepecific data. */
	switch (self->type)
	{
		case LIMDL_NODE_BONE:
			if (!limdl_bone_write (self, writer))
				return 0;
			break;
		case LIMDL_NODE_EMPTY:
			break;
		case LIMDL_NODE_LIGHT:
			if (!limdl_light_write (self, writer))
				return 0;
			break;
		default:
			assert (0 && "invalid node type");
			break;
	}

	/* Write child nodes. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		if (!limdl_node_write (self->nodes.array[i], writer))
			return 0;
	}

	return !writer->error;
}

limdlNode*
limdl_node_get_child (const limdlNode* self,
                      int              index)
{
	return self->nodes.array[index];
}

/**
 * \brief Gets the number of direct children.
 *
 * \param self Node.
 * \return Child count.
 */
int
limdl_node_get_child_count (const limdlNode* self)
{
	return self->nodes.count;
}

/**
 * \brief Gets the recursive child count of the node.
 *
 * \param self Node.
 * \return Recursive child count.
 */
int
limdl_node_get_child_total (const limdlNode* self)
{
	int i;
	const limdlNode* node;

	i = self->nodes.count;
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		i += limdl_node_get_child_total (node);
	}

	return i;
}

/**
 * \brief Gets the name of the node.
 *
 * \param self Node.
 * \return Name.
 */
const char*
limdl_node_get_name (const limdlNode* self)
{
	if (self->name != NULL)
		return self->name;
	return "";
}

/**
 * \brief Gets the axes of the node in global coordinates.
 *
 * \param self Node.
 * \param x Return location for the X axis.
 * \param y Return location for the Y axis.
 * \param z Return location for the Z axis.
 */
void
limdl_node_get_pose_axes (const limdlNode* self,
                          limatVector*     x,
                          limatVector*     y,
                          limatVector*     z)
{
	*x = limat_quaternion_transform (self->transform.global.rotation, limat_vector_init (1.0f, 0.0f, 0.0f));
	*y = limat_quaternion_transform (self->transform.global.rotation, limat_vector_init (0.0f, 1.0f, 0.0f));
	*z = limat_quaternion_transform (self->transform.global.rotation, limat_vector_init (0.0f, 0.0f, 1.0f));
}

void
limdl_node_get_rest_transform (const limdlNode* self,
                               limatTransform*  value)
{
	*value = self->transform.rest;
}

void
limdl_node_get_world_transform (const limdlNode* self,
                                limatTransform*  value)
{
	*value = self->transform.global;
}

/**
 * \brief Sets the pose transformation of the node.
 *
 * Call #limdl_node_rebuild to apply the transformation.
 *
 * \param self Node.
 * \param value Local transformation of the node.
 */
void
limdl_node_set_local_transform (limdlNode*            self,
                                const limatTransform* value)
{
	self->transform.local = *value;
}

limdlNodeType
limdl_node_get_type (const limdlNode* self)
{
	return self->type;
}

/*****************************************************************************/

static void
private_calculate_world_transform (limdlNode* self)
{
	limatTransform p;
	limatTransform t;

	if (self->parent != NULL)
	{
		p = self->parent->transform.global;
		if (self->parent->type == LIMDL_NODE_BONE)
			p.position = limat_transform_transform (p, self->parent->bone.length);
		t = limat_transform_multiply (p, self->transform.rest);
		t = limat_transform_multiply (t, self->transform.local);
		self->transform.global = t;
		if (self->type == LIMDL_NODE_BONE)
			self->bone.tail = limat_transform_transform (t, self->bone.length);
	}
	else
	{
		t = limat_transform_multiply (self->transform.rest, self->transform.local);
		self->transform.global = t;
		if (self->type == LIMDL_NODE_BONE)
			self->bone.tail = limat_transform_transform (t, self->bone.length);
	}
}

/** @} */
/** @} */
