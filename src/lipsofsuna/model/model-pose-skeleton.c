/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIMdlPoseSkeleton PoseSkeleton
 * @{
 */

#include "lipsofsuna/system.h"
#include "model.h"
#include "model-pose.h"
#include "model-pose-skeleton.h"

static int private_copy_node (
	LIMdlPoseSkeleton* self,
	LIMdlNode*         node,
	LIMdlNode*         parent,
	LIMdlNode***       root_array,
	int*               root_count);

static void private_copy_nodes (
	LIMdlPoseSkeleton* self,
	LIMdlModel**       models,
	int                models_num);

static LIMdlNode* private_find_node (
	const LIMdlPoseSkeleton* self,
	const char*              name);

static void private_transform_node (
	LIMdlPoseSkeleton* self,
	LIMdlPose*         pose,
	LIMdlNode*         node);

/*****************************************************************************/

LIMdlPoseSkeleton* limdl_pose_skeleton_new (
	LIMdlModel** models,
	int          count)
{
	LIMdlPoseSkeleton* self;

	lisys_assert (models != NULL || !count);

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlPoseSkeleton));
	if (self == NULL)
		return NULL;

	/* Copy nodes. */
	if (count)
		private_copy_nodes (self, models, count);

	return self;
}

void limdl_pose_skeleton_free (
	LIMdlPoseSkeleton* self)
{
	int i;

	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		lisys_free (self->nodes.array);
	}
	lisys_free (self);
}

/**
 * \brief Finds a node by name.
 * \param self Pose skeleton.
 * \param name Name of the node to find.
 * \return Node or NULL.
 */
LIMdlNode* limdl_pose_skeleton_find_node (
	const LIMdlPoseSkeleton* self,
	const char*              name)
{
	return private_find_node (self, name);
}

/**
 * \brief Rebuilds the skeleton from multiple models.
 *
 * Rebuilds the node hierarchy of the skeleton from the array of models.
 * The array should contain all models whose bones are needed by the
 * pose buffer, including the model of the pose buffer itself. In the case
 * of duplicate bones, the bone of the first model in the list is used.
 *
 * \param self Pose skeleton.
 * \param models Array of models.
 * \param count Number of models in the array.
 */
void limdl_pose_skeleton_rebuild (
	LIMdlPoseSkeleton* self,
	LIMdlModel**       models,
	int                count)
{
	private_copy_nodes (self, models, count);
}

void limdl_pose_skeleton_update (
	LIMdlPoseSkeleton* self,
	LIMdlPose*         pose)
{
	int i;

	/* Transform each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
		private_transform_node (self, pose, self->nodes.array[i]);
}

/*****************************************************************************/

static int private_copy_node (
	LIMdlPoseSkeleton* self,
	LIMdlNode*         node,
	LIMdlNode*         parent,
	LIMdlNode***       root_array,
	int*               root_count)
{
	int i;
	LIMdlNode* copy;
	LIMdlNode** tmp;

	/* Create a new node if there was no existing one. */
	/* If no existing node with the same name existed, we need to create a new
	   node. The new node is added to the parent node, if any, or to the list
	   of root nodes. */
	copy = private_find_node (self, node->name);
	if (copy == NULL)
	{
		/* Copy the node. */
		copy = limdl_node_copy (node, 0);
		if (copy == NULL)
			return 0;

		/* Store the copy. */
		if (parent != NULL)
		{
			copy->parent = parent;
			if (!limdl_node_add_child (parent, copy))
			{
				limdl_node_free (copy);
				return 0;
			}
		}
		else
		{
			tmp = lisys_realloc (*root_array, ((*root_count) + 1) * sizeof (LIMdlNode*));
			if (tmp == NULL)
			{
				limdl_node_free (copy);
				return 0;
			}
			*root_array = tmp;
			(*root_array)[*root_count] = copy;
			*root_count = (*root_count) + 1;
		}
	}

	/* Copy the child nodes recursively. */
	for (i = 0 ; i < node->nodes.count ; i++)
	{
		if (!private_copy_node (self, node->nodes.array[i], copy, root_array, root_count))
			return 0;
	}

	return 1;
}

static void private_copy_nodes (
	LIMdlPoseSkeleton* self,
	LIMdlModel**       models,
	int                models_num)
{
	int i;
	int j;

	/* Free the old nodes. */
	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		lisys_free (self->nodes.array);
		self->nodes.array = NULL;
		self->nodes.count = 0;
	}

	/* Copy nodes from the models. */
	/* This does skeleton merging by adding new nodes under an existing node
	   if the name of the parent exists in the skeleton. If no parent exists,
	   the node is added to the root of the skeleton. */
	for (j = 0 ; j < models_num ; j++)
	{
		for (i = 0 ; i < models[j]->nodes.count ; i++)
			private_copy_node (self, models[j]->nodes.array[i], NULL, &self->nodes.array, &self->nodes.count);
	}
}

static LIMdlNode* private_find_node (
	const LIMdlPoseSkeleton* self,
	const char*              name)
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

static void private_transform_node (
	LIMdlPoseSkeleton* self,
	LIMdlPose*         pose,
	LIMdlNode*         node)
{
	int i;
	float scale;
	LIMatTransform transform;

	/* Calculate the transformation. */
	limdl_pose_calculate_node_tranformation (pose, node->name, &transform, &scale);

	/* Update the node transformation. */
	limdl_node_set_local_transform (node, scale, &transform);
	limdl_node_rebuild_pose (node, 0);

	/* Update the child transformations recursively. */
	for (i = 0 ; i < node->nodes.count ; i++)
		private_transform_node (self, pose, node->nodes.array[i]);
}

/** @} */
/** @} */
