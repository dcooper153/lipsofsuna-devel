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

#ifndef __MODEL_NODE_H__
#define __MODEL_NODE_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>
#include <string/lips-string.h>
#include "model-bone.h"
#include "model-light.h"
#include "model-types.h"

struct _limdlNode
{
	int type;
	char* name;
	limdlModel* model;
	limdlNode* parent;
	struct
	{
		int count;
		limdlNode** array;
	} nodes;
	struct
	{
		limatTransform rest;
		limatTransform local;
		limatTransform global;
	} transform;
	union
	{
		limdlBone bone;
		limdlLight light;
	};
};

#ifdef __cplusplus
extern "C" {
#endif

limdlNode*
limdl_node_new (limdlModel* model);

limdlNode*
limdl_node_copy (const limdlNode* node);

void
limdl_node_free (limdlNode* self);

limdlNode*
limdl_node_find_node (const limdlNode* self,
                      const char*      name);

int
limdl_node_read (limdlNode*   self,
                 liarcReader* reader);

void
limdl_node_rebuild (limdlNode* self,
                    int        recursive);

int
limdl_node_write (const limdlNode* self,
                  liarcWriter*     writer);

limdlNode*
limdl_node_get_child (const limdlNode* self,
                      int              index);

int
limdl_node_get_child_count (const limdlNode* self);

int
limdl_node_get_child_total (const limdlNode* self);

const char*
limdl_node_get_name (const limdlNode* self);

void
limdl_node_get_pose_axes (const limdlNode* self,
                          limatVector*     x,
                          limatVector*     y,
                          limatVector*     z);

void
limdl_node_set_local_transform (limdlNode*            self,
                                const limatTransform* value);

void
limdl_node_get_rest_transform (const limdlNode* self,
                               limatTransform*  value);

void
limdl_node_get_world_transform (const limdlNode* self,
                                limatTransform*  value);

limdlNodeType
limdl_node_get_type (const limdlNode* self);

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#define LIMDL_ITER_STACK_MAX 16
#define LIMDL_FOREACH_NODE(iter, array) \
	for (limdl_node_iter_start (&iter, array) ; \
	     iter.depth >= 0 ; \
	     limdl_node_iter_next (&iter))

typedef struct _limdlNodeIterArray limdlNodeIterArray;
struct _limdlNodeIterArray
{
	int count;
	limdlNode** array;
};

typedef struct _limdlNodeIterStack limdlNodeIterStack;
struct _limdlNodeIterStack
{
	limdlNode* node;
	int index;
};

typedef struct _limdlNodeIter limdlNodeIter;
struct _limdlNodeIter
{
	const limdlNodeIterArray* array;
	limdlNode* value;
	int depth;
	int index;
	int root;
	limdlNodeIterStack* pointer;
	limdlNodeIterStack stack[LIMDL_ITER_STACK_MAX];
};

#ifdef __cplusplus
extern "C" {
#endif

static inline void
limdl_node_iter_start (limdlNodeIter* self,
                       const void*    array)
{
	self->root = 0;
	self->index = 0;
	self->array = (const limdlNodeIterArray*) array;
	self->pointer = self->stack;
	if (self->array->count)
	{
		self->value = self->array->array[0];
		self->pointer->index = 0;
		self->pointer->node = self->value;
		self->pointer++;
		self->depth = 0;
	}
	else
	{
		self->value = NULL;
		self->depth = -1;
	}
}

static inline int
limdl_node_iter_next (limdlNodeIter* self)
{
	/* Descend if found children. */
	if (self->pointer[-1].node->nodes.count && self->depth < LIMDL_ITER_STACK_MAX)
	{
		self->value = self->pointer[-1].node->nodes.array[self->pointer[-1].index];
		self->pointer->index = 0;
		self->pointer->node = self->value;
		self->pointer++;
		self->depth++;
		return 1;
	}

	/* Move to the next node. */
	/* Ascend and retry if siblings ended. */
	while (++self->pointer[-1].index >= self->pointer[-1].node->nodes.count)
	{
		if (self->depth == 0)
		{
			/* Next root node if old one ended. */
			if (++self->root < self->array->count)
			{
				self->value = self->array->array[self->root];
				self->pointer->index = 0;
				self->pointer->node = self->value;
				self->pointer++;
				self->depth = 0;
				return 1;
			}
			else
			{
				self->pointer = self->stack;
				self->value = NULL;
				self->depth = -1;
				return 0;
			}
		}
		else
		{
			/* Ascend one node. */
			self->pointer--;
			self->depth--;
		}
	}

	/* Return found node. */
	self->value = self->pointer[-1].node->nodes.array[self->pointer[-1].index];
	return 1;
}

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
