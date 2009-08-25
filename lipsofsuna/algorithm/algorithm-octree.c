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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup lialgOctree Octree
 * @{
 */

#include <system/lips-system.h>
#include "algorithm-octree.h"

static void
private_free_node (lialgOctree* self,
                   lialgOcnode* node,
                   int          depth);

static int
private_read_node (lialgOctree*    self,
                   lialgOcnode**   node,
                   int             depth,
                   limatVector*    offset,
                   liarcReader*    reader,
                   lialgOctreeRead callback,
                   void*           data);

static int
private_write_node (lialgOctree*     self,
                    lialgOcnode*     node,
                    int              depth,
                    limatVector*     offset,
                    liarcWriter*     writer,
                    lialgOctreeWrite callback,
                    void*            data);

/*****************************************************************************/

/**
 * \brief Creates a new octree.
 *
 * The number of elements along each axis will be 2^depth.
 *
 * \param depth The depth of the tree.
 * \return New octree or NULL.
 */
lialgOctree*
lialg_octree_new (int depth)
{
	lialgOctree* self;

	self = lisys_calloc (1, sizeof (lialgOctree));
	if (self == NULL)
		return NULL;
	self->depth = depth;
	return self;
}

lialgOctree*
lialg_octree_new_from_data (liarcReader*    reader,
                            lialgOctreeRead callback,
                            void*           data)
{
	uint8_t depth;
	lialgOctree* self;
	limatVector offset = { 0.0f, 0.0f, 0.0f };

	/* Read in header. */
	if (!liarc_reader_get_uint8 (reader, &depth))
	{
		lisys_error_set (EINVAL, NULL);
		return NULL;
	}

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lialgOctree));
	if (self == NULL)
		return NULL;
	self->depth = depth;

	/* Load nodes. */
	if (!private_read_node (self, &self->root, 0, &offset, reader, callback, data))
	{
		lialg_octree_free (self);
		return NULL;
	}

	return self;
}

lialgOctree*
lialg_octree_new_from_file (const char*     path,
                            lialgOctreeRead callback,
                            void*           data)
{
	liarcReader* reader;
	lialgOctree* self;

	reader = liarc_reader_new_from_file (path);
	if (reader == NULL)
		return NULL;
	self = lialg_octree_new_from_data (reader, callback, data);
	liarc_reader_free (reader);
	return self;
}

/**
 * \brief Frees the octree.
 *
 * Frees the octree structure and the branch nodes but not any
 * of the potential leaves stored to it.
 *
 * \param self Octree.
 */
void
lialg_octree_free (lialgOctree* self)
{
	if (self->root != NULL)
		private_free_node (self, self->root, 0);
	lisys_free (self);
}

int
lialg_octree_write (lialgOctree*     self,
                    liarcWriter*     writer,
                    lialgOctreeWrite callback,
                    void*            data)
{
	limatVector offset = { 0.0f, 0.0f, 0.0f };

	liarc_writer_append_uint8 (writer, self->depth);
	if (writer->error)
		return 0;
	return private_write_node (self, self->root, 0, &offset, writer, callback, data);
}

void*
lialg_octree_get_data (lialgOctree*    self,
                       const limatVector* point)
{
	int i;
	int x;
	int y;
	int z;
	limatVector off = *point;
	lialgOcnode* node = self->root;

	/* Iterate branches. */
	for (i = 0 ; i < self->depth && node != NULL ; i++)
	{
		x = (off.x >= 0.5f);
		y = (off.y >= 0.5f);
		z = (off.z >= 0.5f);
		node = node->children[x][y][z];
		off.x = 2.0f * (off.x - 0.5f * x);
		off.y = 2.0f * (off.y - 0.5f * y);
		off.z = 2.0f * (off.z - 0.5f * z);
	}

	return node;
}

/* FIXME: Setting to NULL doesn't clean nodes. */
int
lialg_octree_set_data (lialgOctree*    self,
                       const limatVector* point,
                       void*           data)
{
	int i;
	int x;
	int y;
	int z;
	limatVector off = *point;
	lialgOcnode** node = &self->root;

	/* Create branches. */
	for (i = 0 ; i < self->depth ; i++)
	{
		x = (off.x >= 0.5f);
		y = (off.y >= 0.5f);
		z = (off.z >= 0.5f);
		if (*node == NULL)
		{
			*node = lisys_calloc (1, sizeof (lialgOcnode));
			if (*node == NULL)
				return 0;
		}
		node = &((*node)->children[x][y][z]);
		off.x = 2.0f * (off.x - 0.5f * x);
		off.y = 2.0f * (off.y - 0.5f * y);
		off.z = 2.0f * (off.z - 0.5f * z);
	}

	/* Create leaf. */
	*node = data;
	return 1;
}

/**
 * \brief Gets the number of elements along any axis.
 *
 * The total number of elements in the whole tree is size^3.
 *
 * \param self Octree.
 * \return Number of elements.
 */
int
lialg_octree_get_size (const lialgOctree* self)
{
	return 1 << self->depth;
}

/*****************************************************************************/

static void
private_free_node (lialgOctree* self,
                   lialgOcnode* node,
                   int          depth)
{
	if (depth++ == self->depth)
		return;
	if (node->children[0][0][0] != NULL) private_free_node (self, node->children[0][0][0], depth);
	if (node->children[0][0][1] != NULL) private_free_node (self, node->children[0][0][1], depth);
	if (node->children[0][1][0] != NULL) private_free_node (self, node->children[0][1][0], depth);
	if (node->children[0][1][1] != NULL) private_free_node (self, node->children[0][1][1], depth);
	if (node->children[1][0][0] != NULL) private_free_node (self, node->children[1][0][0], depth);
	if (node->children[1][0][1] != NULL) private_free_node (self, node->children[1][0][1], depth);
	if (node->children[1][1][0] != NULL) private_free_node (self, node->children[1][1][0], depth);
	if (node->children[1][1][1] != NULL) private_free_node (self, node->children[1][1][1], depth);
}

static int 
private_read_node (lialgOctree*    self,
                   lialgOcnode**   node,
                   int             depth,
                   limatVector*    offset,
                   liarcReader*    reader,
                   lialgOctreeRead callback,
                   void*           data)
{
	int x;
	int y;
	int z;
	uint8_t bit = 1;
	uint8_t mask;
	limatVector suboff;

	/* Read leaves. */
	if (depth++ == self->depth)
	{
		if (!callback (data, (void**) node, offset, reader))
			return 0;
		return 1;
	}

	/* Read branch header. */
	*node = lisys_calloc (1, sizeof (lialgOcnode));
	if (*node == NULL)
		return 0;
	if (!liarc_reader_get_uint8 (reader, &mask) || !mask)
	{
		lisys_error_set (EINVAL, NULL);
		return 0;
	}

	/* Read branches recursively. */
	for (z = 0 ; z < 2 ; z++)
	for (y = 0 ; y < 2 ; y++)
	for (x = 0 ; x < 2 ; x++, bit <<= 1)
	{
		if (!(mask & bit))
			continue;
		suboff = limat_vector_init (x, y, z);
		suboff = limat_vector_multiply (suboff, 0.5f / depth);
		suboff = limat_vector_add (suboff, *offset);
		if (!private_read_node (self, &((*node)->children[x][y][z]), depth, &suboff, reader, callback, data))
			return 0;
	}

	return 1;
}

static int
private_write_node (lialgOctree*     self,
                    lialgOcnode*     node,
                    int              depth,
                    limatVector*     offset,
                    liarcWriter*     writer,
                    lialgOctreeWrite callback,
                    void*            data)
{
	int x;
	int y;
	int z;
	uint8_t mask;
	limatVector suboff;

	/* Write leaf data. */
	if (depth++ == self->depth)
	{
		if (!callback (data, node, offset, writer))
			return 0;
		return 1;
	}

	/* Write branch mask. */
	mask = 0;
	if (node->children[0][0][0] != NULL) mask |= 0x01;
	if (node->children[1][0][0] != NULL) mask |= 0x02;
	if (node->children[0][1][0] != NULL) mask |= 0x04;
	if (node->children[1][1][0] != NULL) mask |= 0x08;
	if (node->children[0][0][1] != NULL) mask |= 0x10;
	if (node->children[1][0][1] != NULL) mask |= 0x20;
	if (node->children[0][1][1] != NULL) mask |= 0x40;
	if (node->children[1][1][1] != NULL) mask |= 0x80;
	liarc_writer_append_uint8 (writer, mask);
	if (writer->error)
		return 0;

	/* Write branches recursively. */
	for (z = 0 ; z < 2 ; z++)
	for (y = 0 ; y < 2 ; y++)
	for (x = 0 ; x < 2 ; x++)
	{
		if (node->children[x][y][z] != NULL)
		{
			suboff = limat_vector_init (x, y, z);
			suboff = limat_vector_multiply (suboff, 0.5f / depth);
			suboff = limat_vector_add (suboff, *offset);
			if (!private_write_node (self, node->children[x][y][z], depth, &suboff, writer, callback, data))
				return 0;
		}
	}

	return 1;
}

/** @} */
/** @} */
