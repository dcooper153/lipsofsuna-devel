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
 * \addtogroup lieng Render
 * @{
 * \addtogroup liengConstraint Constraint
 * @{
 */

#include "engine-constraint.h"
#include "engine-object.h"

liengConstraint*
lieng_constraint_new (liengObject* first_object,
                      const char*  first_anchor,
                      liengObject* second_object,
                      const char*  second_anchor)
{
	liengConstraint* self;

	self = calloc (1, sizeof (liengConstraint));
	if (self == NULL)
		return NULL;
	self->objects[0] = first_object;
	self->objects[1] = second_object;
	self->nodes[0] = limdl_pose_find_node (first_object->pose, first_anchor);
	self->nodes[1] = limdl_pose_find_node (second_object->pose, second_anchor);

	return self;
}

void
lieng_constraint_free (liengConstraint* self)
{
	assert (self->next == NULL);
	assert (self->prev == NULL);
	free (self);
}

/*****************************************************************************/

static inline limatTransform
limat_transform_snap (limatTransform self,
                      limatTransform child)
{
	limatTransform result;
	limatTransform inverse;

	inverse = limat_transform_invert (child);
	result = limat_transform_multiply (self, inverse);

	return result;
}

/**
 * \brief Updates the constraint.
 *
 * Corrects the orientations of the constrained objects.
 *
 * \param self Constraint.
 * \param secs Tick duration in seconds.
 */
void
lieng_constraint_update (liengConstraint* self,
                         float            secs)
{
	limatTransform child;
	limatTransform object;
	limatTransform parent;

	/* FIXME: Bad physics: always corrects the second object. */
	lieng_constraint_get_transform (self, 0, LIENG_MATRIX_WORLD, &parent);
	lieng_constraint_get_transform (self, 1, LIENG_MATRIX_LOCAL, &child);
	object = limat_transform_snap (parent, child);
	lieng_object_set_transform (self->objects[1], &object);
}

/**
 * \brief Gets the transformation of one of the nodes.
 *
 * \param self Constraint.
 * \param index Node index.
 * \param type Matrix type.
 * \param value Return location for the transformation.
 */
void
lieng_constraint_get_transform (liengConstraint* self,
                                int              index,
                                liengMatrix      type,
                                limatTransform*  value)
{
	limatTransform t0;
	limatTransform t1;

	assert (index >= 0);
	assert (index < 2);

	if (type == LIENG_MATRIX_WORLD)
	{
		if (self->nodes[index] != NULL)
		{
			limdl_node_get_pose_transform (self->nodes[index], &t1);
			lieng_object_get_transform (self->objects[index], &t0);
			t0 = limat_transform_multiply (t0, t1);
		}
		else
			lieng_object_get_transform (self->objects[index], &t0);
	}
	else
	{
		if (self->nodes[index] != NULL)
			limdl_node_get_pose_transform (self->nodes[index], &t0);
		else
			t0 = limat_transform_identity ();
	}

	*value = t0;
}

/** @} */
/** @} */
