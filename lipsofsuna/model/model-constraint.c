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
 * \addtogroup limdlConstraint Constraint
 * @{
 */

#include <system/lips-system.h>
#include "model-constraint.h"

#define LIMDL_CONSTRAINT_DAMPING(i) 0.05f
#define LIMDL_CONSTRAINT_ITERATIONS 100

static void
private_solve_inverse_kinematics (limdlConstraint* self,
                                  limdlPose*       pose);

/*****************************************************************************/

limdlConstraint*
limdl_constraint_new_from_stream (liarcReader* reader)
{
	uint32_t tmp;
	uint32_t type;
	limdlConstraint* self;

	self = lisys_calloc (1, sizeof (limdlConstraint));
	if (self == NULL)
		return NULL;

	if (!liarc_reader_get_uint32 (reader, &type))
	{
		lisys_free (self);
		return NULL;
	}
	self->type = type;
	switch (type)
	{
		case LIMDL_CONSTRAINT_COPY_ROTATION:
			if (!liarc_reader_get_text (reader, "", &self->copy_rotation.node_name) ||
			    !liarc_reader_get_text (reader, "", &self->copy_rotation.target_name))
			{
				limdl_constraint_free (self);
				return NULL;
			}
			break;
		case LIMDL_CONSTRAINT_INVERSE_KINEMATICS:
			if (!liarc_reader_get_text (reader, "", &self->inverse_kinematics.node_name) ||
			    !liarc_reader_get_text (reader, "", &self->inverse_kinematics.target_name) ||
			    !liarc_reader_get_uint32 (reader, &tmp))
			{
				limdl_constraint_free (self);
				return NULL;
			}
			self->inverse_kinematics.chain_length = tmp;
			break;
		case LIMDL_CONSTRAINT_LIMIT_ROTATION:
			if (!liarc_reader_get_text (reader, "", &self->limit_rotation.node_name) ||
			    !liarc_reader_get_float (reader, self->limit_rotation.min + 0) ||
			    !liarc_reader_get_float (reader, self->limit_rotation.max + 0) ||
			    !liarc_reader_get_float (reader, self->limit_rotation.min + 1) ||
			    !liarc_reader_get_float (reader, self->limit_rotation.max + 1) ||
			    !liarc_reader_get_float (reader, self->limit_rotation.min + 2) ||
			    !liarc_reader_get_float (reader, self->limit_rotation.max + 2))
			{
				limdl_constraint_free (self);
				return NULL;
			}
			break;
		default:
			lisys_error_set (EINVAL, "invalid constraint type");
			lisys_free (self);
			return NULL;
	}

	return self;
}

void
limdl_constraint_free (limdlConstraint* self)
{
	switch (self->type)
	{
		case LIMDL_CONSTRAINT_COPY_ROTATION:
			lisys_free (self->copy_rotation.node_name);
			lisys_free (self->copy_rotation.target_name);
			break;
		case LIMDL_CONSTRAINT_INVERSE_KINEMATICS:
			lisys_free (self->inverse_kinematics.node_name);
			lisys_free (self->inverse_kinematics.target_name);
			break;
		case LIMDL_CONSTRAINT_LIMIT_ROTATION:
			lisys_free (self->limit_rotation.node_name);
			break;
	}
	lisys_free (self);
}

void
limdl_constraint_solve (limdlConstraint* self,
                        limdlPose*       pose)
{
	switch (self->type)
	{
		case LIMDL_CONSTRAINT_COPY_ROTATION:
			/* TODO */
			break;
		case LIMDL_CONSTRAINT_INVERSE_KINEMATICS:
			private_solve_inverse_kinematics (self, pose);
			break;
		case LIMDL_CONSTRAINT_LIMIT_ROTATION:
			/* TODO */
			break;
	}
}

int
limdl_constraint_write (const limdlConstraint* self,
                        liarcWriter*           writer)
{
	if (!liarc_writer_append_uint32 (writer, self->type))
		return 0;
	switch (self->type)
	{
		case LIMDL_CONSTRAINT_COPY_ROTATION:
			if (!liarc_writer_append_string (writer, self->copy_rotation.node_name) ||
			    !liarc_writer_append_nul (writer) ||
			    !liarc_writer_append_string (writer, self->copy_rotation.target_name) ||
			    !liarc_writer_append_nul (writer))
				return 0;
			break;
		case LIMDL_CONSTRAINT_INVERSE_KINEMATICS:
			if (!liarc_writer_append_string (writer, self->inverse_kinematics.node_name) ||
			    !liarc_writer_append_nul (writer) ||
			    !liarc_writer_append_string (writer, self->inverse_kinematics.target_name) ||
			    !liarc_writer_append_nul (writer) ||
			    !liarc_writer_append_uint32 (writer, self->inverse_kinematics.chain_length))
				return 0;
			break;
		case LIMDL_CONSTRAINT_LIMIT_ROTATION:
			if (!liarc_writer_append_string (writer, self->copy_rotation.node_name) ||
			    !liarc_writer_append_nul (writer) ||
			    !liarc_writer_append_float (writer, self->limit_rotation.min[0]) ||
			    !liarc_writer_append_float (writer, self->limit_rotation.max[0]) ||
			    !liarc_writer_append_float (writer, self->limit_rotation.min[1]) ||
			    !liarc_writer_append_float (writer, self->limit_rotation.max[1]) ||
			    !liarc_writer_append_float (writer, self->limit_rotation.min[2]) ||
			    !liarc_writer_append_float (writer, self->limit_rotation.max[2]))
				return 0;
			break;
	}

	return 1;
}

/*****************************************************************************/

/**
 * \brief Solves an inverse kinematics constraint.
 *
 * This function uses the cyclic coordinate descent algorithm to solve the IK
 * problem. It kind of works, though SDLS would likely be more reliable.
 * Depending on a huge math library such as Eigen just to calculate the SVD
 * doesn't sound compelling, however, nor does writing my own LGPL compatible
 * SVD algorithm right now.
 *
 * \param self Constraint.
 * \param pose Pose.
 */
static void
private_solve_inverse_kinematics (limdlConstraint* self,
                                  limdlPose*       pose)
{
	int i;
	int j;
	int k;
	int joints;
	float rot;
	limatQuaternion quat;
	limatTransform transform0;
	limatTransform transform1;
	limatVector eff;
	limatVector tar;
	limatVector axis;
	limatVector diff;
	limdlNode* node;
	limdlNode* effector;
	limdlNode* target;
	limdlNode** nodes;

	/* Get effector and target. */
	effector = limdl_pose_find_node (pose, self->inverse_kinematics.node_name);
	target = limdl_pose_find_node (pose, self->inverse_kinematics.target_name);
	if (effector == NULL || target == NULL)
		return;

	/* Get node chain. */
	for (i = 0, node = effector ; i < self->inverse_kinematics.chain_length ; i++, node = node->parent)
	{
		if (node->parent == NULL)
			break;
	}
	joints = i;
	nodes = calloc (joints, sizeof (limdlNode**));
	for (i = 0, node = effector ; i < joints ; i++, node = node->parent)
	{
		nodes[i] = node;
	}

	/* Calculate new rotations. */
	for (j = 0 ; j < LIMDL_CONSTRAINT_ITERATIONS ; j++)
	{
		for (k = 0 ; k < joints ; k++)
		{
			tar = target->transform.global.position;
			if (effector->type == LIMDL_NODE_BONE)
				eff = effector->bone.tail;
			else
				eff = effector->transform.global.position;

			/* Move to node space. */
			transform0 = limat_transform_invert (nodes[k]->transform.global);
			tar = limat_transform_transform (transform0, tar);
			eff = limat_transform_transform (transform0, eff);

			/* Check if reached destination. */
			diff = limat_vector_subtract (tar, eff);
			if (limat_vector_dot (diff, diff) < 0.005f)
				break;

			/* Calculate rotation quaternion. */
			rot = limat_vector_dot (eff, tar);
			if (LI_ABS (rot) >= 0.9999f)
				continue;
			rot = acos (rot);
			if (rot > LIMDL_CONSTRAINT_DAMPING (j))
				rot = LIMDL_CONSTRAINT_DAMPING (j);
			axis = limat_vector_cross (eff, tar);
			quat = limat_quaternion_rotation (rot, axis);

			/* Update bone transformation. */
			transform0 = nodes[k]->transform.local;
			transform1 = limat_convert_quaternion_to_transform (quat);
			transform1 = limat_transform_multiply (transform1, transform0);
			limdl_node_set_local_transform (nodes[k], &transform1);
			limdl_node_rebuild (nodes[k], 1);
		}
		if (k < joints)
			break;
	}

	free (nodes);
}

/** @} */
/** @} */
