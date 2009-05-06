/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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

#include <stdlib.h>
#include "ext-match.h"

static int
private_intersects_object (const liextMatch*     self,
                           const liengObject*    object,
                           const liengModel*     model,
                           const limatTransform* transform);

/*****************************************************************************/

liextMatch*
liext_match_new (liengObject* root,
                 liextRule*   rule,
                 int          line)
{
	liextMatch* self;

	self = calloc (1, sizeof (liextMatch));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->root = root;
	self->rule = rule;
	self->line = line;
	self->objects.count = rule->lines.count;
	self->objects.array = calloc (rule->lines.count, sizeof (liengObject*));
	if (self->objects.array == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self);
		return NULL;
	}

	return self;
}

void
liext_match_free (liextMatch* self)
{
	free (self->objects.array);
	free (self);
}

/**
 * \brief Gets the index of the object in the match.
 *
 * \param self Match.
 * \param object Object.
 * \return Index in the object array or -1.
 */
int
liext_match_find_object (const liextMatch*  self,
                         const liengObject* object)
{
	int i;

	for (i = 0 ; i < self->objects.count ; i++)
	{
		if (object == self->objects.array[i])
			return i;
	}

	return -1;
}

int
liext_match_intersects_object (const liextMatch*  self,
                               const liengObject* object)
{
	int i;
	liengModel* model;
	limatTransform transform0;
	limatTransform transform1;

	/* Check if part of the match. */
	if (liext_match_find_object (self, object) != -1)
		return 0;

	/* Check against each created model. */
	for (i = 0 ; i < self->objects.count ; i++)
	{
		/* Check if exists already. */
		if (self->objects.array[i] != NULL)
			continue;

		/* Get model orientation. */
		liext_rule_get_transform (self->rule, self->line, i, &transform0);
		lieng_object_get_transform (self->root, &transform1);
		transform0 = limat_transform_multiply (transform1, transform0);
		model = self->rule->lines.array[i].model;

		/* Check for intersection. */
		if (private_intersects_object (self, object, model, &transform0))
			return 1;
	}

	return 0;
}

/*****************************************************************************/

static int
private_intersects_object (const liextMatch*     self,
                           const liengObject*    object,
                           const liengModel*     model,
                           const limatTransform* transform)
{
	limatAabb bounds0;
	limatAabb bounds1;

	/* Get model bounds. */
	lieng_model_get_bounds_transform (model, transform, &bounds0);

	/* Check if intersects with object bounds. */
	lieng_object_get_bounds_transform (object, &bounds1);
	if (!limat_aabb_intersects_aabb (&bounds0, &bounds1))
		return 0;

	/* FIXME: Final test should be done in polygon level. */
	/* TODO: Check if the intersecting object would be a part
	   of a rule that would be completed by the new objects. */
	/* TODO: Make sure that the object is entirely within the visible
	   map area or it might intersect with stuff outside. */

	return 1;
}

/** @} */
/** @} */
/** @} */
