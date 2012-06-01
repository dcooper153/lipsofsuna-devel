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
 * \addtogroup LIMdlPoseBuffer PoseBuffer
 * @{
 */

#include "lipsofsuna/system.h"
#include "model.h"
#include "model-pose.h"
#include "model-pose-buffer.h"

static void private_transform_node (
	LIMdlPoseBuffer* self,
	LIMdlPose*       pose,
	LIMdlNode*       node);

/*****************************************************************************/

LIMdlPoseBuffer* limdl_pose_buffer_new (
	LIMdlModel* model)
{
	int i;
	LIMdlPoseBuffer* self;
	LIMdlPoseGroup* pose_group;
	LIMdlWeightGroup* weight_group;

	lisys_assert (model != NULL);

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlPoseBuffer));
	if (self == NULL)
		return NULL;
	self->model = model;

	/* Allocate the bone transformations. */
	self->bones.count = model->weight_groups.count + 1;
	self->bones.array = lisys_calloc (self->bones.count, sizeof (LIMdlPoseBufferBone));
	if (self->bones.array == NULL)
	{
		limdl_pose_buffer_free (self);
		return NULL;
	}
	self->bones.array[0].transform = limat_transform_identity ();
	self->bones.array[0].scale = limat_vector_init (1.0f, 1.0f, 1.0f);

	/* Copy nodes. */
	if (model != NULL && model->nodes.count)
	{
		self->nodes.count = model->nodes.count;
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (LIMdlNode*));
		if (self->nodes.array == NULL)
		{
			limdl_pose_buffer_free (self);
			return NULL;
		}
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_copy (model->nodes.array[i]);
			if (self->nodes.array[i] == NULL)
			{
				limdl_pose_buffer_free (self);
				return NULL;
			}
		}
	}

	/* Precalculate weight group information. */
	if (model != NULL && model->weight_groups.count)
	{
		self->groups.count = model->weight_groups.count;
		self->groups.array = lisys_calloc (self->groups.count, sizeof (LIMdlPoseGroup));
		if (self->groups.array == NULL)
		{
			limdl_pose_buffer_free (self);
			return NULL;
		}
		for (i = 0 ; i < self->groups.count ; i++)
		{
			weight_group = model->weight_groups.array + i;
			pose_group = self->groups.array + i;
			pose_group->weight_group = weight_group;
			pose_group->node = limdl_pose_buffer_find_node (self, weight_group->bone);
			pose_group->rotation = limat_quaternion_identity ();
			if (pose_group->node != NULL)
			{
				pose_group->head_rest = pose_group->node->rest_transform.global.position;
				pose_group->enabled = 1;
			}
		}
	}

	return self;
}

void limdl_pose_buffer_free (
	LIMdlPoseBuffer* self)
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
	lisys_free (self->groups.array);
	lisys_free (self->bones.array);
	lisys_free (self);
}

/**
 * \brief Finds a node by name.
 * \param self Pose buffer.
 * \param name Name of the node to find.
 * \return Node or NULL.
 */
LIMdlNode* limdl_pose_buffer_find_node (
	const LIMdlPoseBuffer* self,
	const char*            name)
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

void limdl_pose_buffer_update (
	LIMdlPoseBuffer* self,
	LIMdlPose*       pose)
{
	int i;
	float s;
	LIMatQuaternion quat0;
	LIMatQuaternion quat1;
	LIMdlNode* node;
	LIMdlPoseBufferBone* bone;
	LIMdlPoseGroup* group1;
	LIMdlNode* node0;
	LIMdlWeightGroup* group;

	lisys_assert (self->bones.count == self->model->weight_groups.count + 1);

	/* Transform each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node0 = self->nodes.array[i];
		private_transform_node (self, pose, node0);
	}

	/* Update node transformations. */
	for (i = 0 ; i < self->groups.count ; i++)
	{
		group1 = self->groups.array + i;
		if (group1->enabled)
		{
			quat0 = group1->node->rest_transform.global.rotation;
			quat1 = group1->node->pose_transform.global.rotation;
			quat0 = limat_quaternion_conjugate (quat0);
			group1->rotation = limat_quaternion_multiply (quat1, quat0);
			group1->head_pose = group1->node->pose_transform.global.position;
			group1->scale_pose = group1->node->pose_transform.global_scale;
		}
		else
		{
			group1->rotation = limat_quaternion_identity ();
			group1->head_pose = limat_vector_init (0.0f, 0.0f, 0.0f);
			group1->scale_pose = 1.0f;
		}
	}

	/* Update the pose buffer. */
	for (i = 0 ; i < self->model->weight_groups.count ; i++)
	{
		bone = self->bones.array + i + 1;
		group = self->model->weight_groups.array + i;
		if (group->node != NULL)
		{
			node = limdl_pose_buffer_find_node (self, group->node->name);
			if (node != NULL)
			{
				s = node->pose_transform.global_scale;
				bone->scale = limat_vector_init(s, s, s);
				bone->transform = node->pose_transform.global;
			}
		}
	}
}

/*****************************************************************************/

static void private_transform_node (
	LIMdlPoseBuffer* self,
	LIMdlPose*       pose,
	LIMdlNode*       node)
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
