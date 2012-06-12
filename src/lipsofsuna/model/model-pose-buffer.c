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

LIMdlPoseBuffer* limdl_pose_buffer_new (
	LIMdlModel* model)
{
	LIMdlPoseBuffer* self;

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

	/* Set the rest pose transformation. */
	limdl_pose_buffer_reset (self);

	return self;
}

void limdl_pose_buffer_free (
	LIMdlPoseBuffer* self)
{
	lisys_free (self->bones.array);
	lisys_free (self);
}

void limdl_pose_buffer_reset (
	LIMdlPoseBuffer* self)
{
	int i;
	float s;
	LIMdlNode* node;
	LIMdlPoseBufferBone* bone;
	LIMdlWeightGroup* group;

	for (i = 0 ; i < self->model->weight_groups.count ; i++)
	{
		bone = self->bones.array + i + 1;
		group = self->model->weight_groups.array + i;
		node = limdl_model_find_node (self->model, group->bone);
		if (node != NULL)
		{
			s = node->pose_transform.global_scale;
			bone->scale = limat_vector_init (s, s, s);
			bone->transform = node->pose_transform.global;
		}
		else
		{
			bone->scale = limat_vector_init (1.0f, 1.0f, 1.0f);
			bone->transform = limat_transform_identity ();
		}
	}
}

void limdl_pose_buffer_update (
	LIMdlPoseBuffer*   self,
	LIMdlPoseSkeleton* skeleton)
{
	int i;
	float s;
	LIMdlNode* node;
	LIMdlPoseBufferBone* bone;
	LIMdlWeightGroup* group;

	lisys_assert (self->bones.count == self->model->weight_groups.count + 1);

	/* Update the pose buffer. */
	for (i = 0 ; i < self->bones.count - 1 ; i++)
	{
		bone = self->bones.array + i + 1;
		group = self->model->weight_groups.array + i;
		node = limdl_pose_skeleton_find_node (skeleton, group->bone);
		if (node == NULL)
			node = limdl_model_find_node (self->model, group->bone);

		if (node != NULL)
		{
			s = node->pose_transform.global_scale;
			bone->scale = limat_vector_init (s, s, s);
			bone->transform = node->pose_transform.global;
		}
	}
}

/** @} */
/** @} */
