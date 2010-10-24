/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LIGen Generator
 * @{
 * \addtogroup LIGenBrush Brush
 * @{
 */

#include "generator.h"
#include "generator-brush.h"

/**
 * \brief Creates a new brush.
 *
 * \param generator Generator.
 * \param width Width in voxels.
 * \param height Height in voxels.
 * \param depth Depth in voxels.
 * \return New brush or NULL.
 */
LIGenBrush*
ligen_brush_new (LIGenGenerator* generator,
                 int             width,
                 int             height,
                 int             depth)
{
	LIGenBrush* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIGenBrush));
	if (self == NULL)
		return NULL;
	self->generator = generator;
	self->id = -1;
	self->size[0] = width;
	self->size[1] = height;
	self->size[2] = depth;

	/* Allocate name. */
	self->name = lisys_calloc (1, sizeof (char));
	if (self->name == NULL)
		return NULL;

	return self;
}

/**
 * \brief Frees the brush.
 *
 * \param self Brush.
 */
void
ligen_brush_free (LIGenBrush* self)
{
	int i;

	for (i = 0 ; i < self->rules.count ; i++)
		ligen_rule_free (self->rules.array[i]);
	lisys_free (self->rules.array);
	lisys_free (self->name);
	lisys_free (self);
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
ligen_brush_insert_rule (LIGenBrush* self,
                         LIGenRule*  rule)
{
	if (!lialg_array_append (&self->rules, &rule))
		return 0;
	rule->id = self->rules.count - 1;
	return 1;
}

/**
 * \brief Removes a rule from the brush.
 *
 * The numbers of the other rules may be altered.
 *
 * \param self Brush.
 * \param index Rule number.
 */
void
ligen_brush_remove_rule (LIGenBrush* self,
                         int         index)
{
	int i;
	LIGenRule* rule;

	lisys_assert (index >= 0);
	lisys_assert (index < self->rules.count);

	ligen_rule_free (self->rules.array[index]);
	lialg_array_remove (&self->rules, index);

	for (i = 0 ; i < self->rules.count ; i++)
	{
		rule = self->rules.array[i];
		rule->id = i;
	}
}

int
ligen_brush_set_name (LIGenBrush* self,
                      const char* value)
{
	char* tmp;

	tmp = listr_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->name);
	self->name = tmp;

	return 1;
}

int
ligen_brush_set_size (LIGenBrush* self,
                      int         x,
                      int         y,
                      int         z)
{
	lisys_assert (x > 0);
	lisys_assert (y > 0);
	lisys_assert (z > 0);

	self->size[0] = x;
	self->size[1] = y;
	self->size[2] = z;

	return 1;
}

/** @} */
/** @} */
