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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup ligenBrush Brush
 * @{
 */

#include "generator-brush.h"

/**
 * \brief Creates a new brush.
 *
 * \param width Width in voxels.
 * \param height Height in voxels.
 * \param depth Depth in voxels.
 * \return New brush or NULL.
 */
ligenBrush*
ligen_brush_new (int width,
                 int height,
                 int depth)
{
	ligenBrush* self;

	/* Allocate self. */
	self = calloc (1, sizeof (ligenBrush));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->size[0] = width;
	self->size[1] = height;
	self->size[2] = depth;

	/* Allocate voxels. */
	self->voxels.count = width * height * depth;
	self->voxels.array = calloc (self->voxels.count, sizeof (livoxVoxel));
	if (self->voxels.array == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the brush.
 *
 * \param self Brush.
 */
void
ligen_brush_free (ligenBrush* self)
{
	int i;

	for (i = 0 ; i < self->rules.count ; i++)
		ligen_rule_free (self->rules.array[i]);
	free (self->voxels.array);
	free (self->objects.array);
	free (self->rules.array);
	free (self);
}

/**
 * \brief Inserts a rule to the brush.
 *
 * The ownership of the rule is transferred to the brush if succeeded.
 *
 * \param self Brush.
 * \param rule Rule.
 * \return Nonzero on success.
 */
int
ligen_brush_insert_rule (ligenBrush* self,
                         ligenRule*  rule)
{
	if (!lialg_array_append (&self->rules, &rule))
		return 0;
	return 1;
}

/** @} */
/** @} */
