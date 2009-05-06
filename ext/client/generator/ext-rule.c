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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#include <algorithm/lips-algorithm.h>
#include <system/lips-system.h>
#include "ext-rule.h"

liextRule*
liext_rule_new ()
{
	liextRule* self;

	self = calloc (1, sizeof (liextRule));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->weight = 1.0f;

	return self;
}

void
liext_rule_free (liextRule* self)
{
	free (self->lines.array);
	free (self);
}

int
liext_rule_create_line (liextRule*            self,
                        const limatTransform* transform,
                        const liengModel*     model)
{
	liextRuleLine tmp;

	tmp.transform = *transform;
	tmp.model = (liengModel*) model;
	return lialg_array_append (&self->lines, &tmp);
}

/**
 * \brief Checks if the object matches the rule.
 *
 * Since the rule may contain multiple similar objects and the matching test
 * is done in relative coordinate space, an object must be chosen as a frame
 * of reference before any further matching can be done.
 *
 * \param self Rule.
 * \param line Number of the line matching the root object.
 * \param root Root object used as a frame of reference.
 * \param object Matched object.
 * \return Line number or -1.
 */
int
liext_rule_matches (const liextRule*   self,
                    int                line,
                    const liengObject* root,
                    const liengObject* object)
{
	int i;
	float len;
	liextRuleLine* ruleline;
	limatVector vector;
	limatVector position;
	limatQuaternion quat;
	limatQuaternion rotation;
	limatQuaternion inverse;
	limatTransform transform0;
	limatTransform transform1;

	/* Get object orientation relative to root. */
	/* TODO: Could be done outside of the function. */
	lieng_object_get_transform (root, &transform0);
	lieng_object_get_transform (object, &transform1);
	inverse = limat_quaternion_conjugate (transform0.rotation);
	position = limat_vector_subtract (transform1.position, transform0.position);
	position = limat_quaternion_transform (inverse, position);
	rotation = limat_quaternion_multiply (transform1.rotation, inverse);

	/* Get object orientation in rule space. */
	ruleline = self->lines.array + line;
	position = limat_quaternion_transform (ruleline->transform.rotation, position);
	position = limat_vector_add (ruleline->transform.position, position);
	rotation = limat_quaternion_multiply (ruleline->transform.rotation, rotation);

/* FIXME */
#define POSITION_DIFF 0.1f
#define ROTATION_DIFF 0.1f

	/* Compare against each rule line. */
	for (i = 0 ; i < self->lines.count ; i++)
	{
		ruleline = self->lines.array + i;

		/* Compare models. */
		if (object->model != ruleline->model)
			continue;

		/* Compare positions. */
		vector = limat_vector_subtract (position, ruleline->transform.position);
		len = limat_vector_dot (vector, vector);
		if (len > POSITION_DIFF * POSITION_DIFF)
			continue;

		/* Compare rotations. */
		quat = limat_quaternion_subtract (rotation, ruleline->transform.rotation);
		len = limat_quaternion_dot (quat, quat);
		if (len > ROTATION_DIFF * ROTATION_DIFF)
			continue;

		return i;
	}

	return -1;
}

/**
 * \brief Gets the relative transformation of an object.
 *
 * \param self Rule.
 * \param root Line number of the root object.
 * \param line Line number of the requested object.
 * \param transform Return location for the transformation.
 */
void
liext_rule_get_transform (const liextRule* self,
                          int              root,
                          int              line,
                          limatTransform*  transform)
{
	liextRuleLine* base;
	liextRuleLine* elem;

	base = self->lines.array + root;
	elem = self->lines.array + line;
	*transform = limat_transform_relative (base->transform, elem->transform);
}

/** @} */
/** @} */
/** @} */
