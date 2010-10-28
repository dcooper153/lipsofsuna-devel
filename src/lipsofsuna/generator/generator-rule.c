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
 * \addtogroup LIGenRule Rule
 * @{
 */

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include "generator-rule.h"

/**
 * \brief Creates a new rule.
 *
 * \return New rule or NULL.
 */
LIGenRule*
ligen_rule_new ()
{
	LIGenRule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIGenRule));
	if (self == NULL)
		return NULL;

	/* Allocate name. */
	self->name = lisys_calloc (1, sizeof (char));
	if (self->name == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a soft copy of the rule.
 *
 * \param rule Rule to clone.
 * \return New rule or NULL.
 */
LIGenRule*
ligen_rule_new_copy (LIGenRule* rule)
{
	LIGenRule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIGenRule));
	if (self == NULL)
		return NULL;
	self->flags = rule->flags;

	/* Copy name. */
	self->name = listr_dup (rule->name);
	if (self->name == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Copy strokes. */
	if (rule->strokes.count)
	{
		self->strokes.array = lisys_calloc (1, rule->strokes.count * sizeof (LIGenRulestroke));
		if (self->strokes.array == NULL)
		{
			lisys_free (self->name);
			lisys_free (self);
			return NULL;
		}
		memcpy (self->strokes.array, rule->strokes.array, rule->strokes.count * sizeof (LIGenRulestroke));
		self->strokes.count = rule->strokes.count;
	}

	return self;
}

/**
 * \brief Frees the rule.
 *
 * \param self Rule.
 */
void
ligen_rule_free (LIGenRule* self)
{
	lisys_free (self->strokes.array);
	lisys_free (self->name);
	lisys_free (self);
}

/**
 * \brief Inserts a stroke to the rule.
 *
 * \param self Rule.
 * \param x Relative position.
 * \param y Relative position.
 * \param z Relative position.
 * \param flags Stroke flags.
 * \param brush Brush number.
 * \return Nonzero on success.
 */
int
ligen_rule_insert_stroke (LIGenRule* self,
                          int        x,
                          int        y,
                          int        z,
                          int        flags,
                          int        brush)
{
	LIGenRulestroke tmp;

	tmp.pos[0] = x;
	tmp.pos[1] = y;
	tmp.pos[2] = z;
	tmp.flags = flags;
	tmp.brush = brush;
	if (!lialg_array_append (&self->strokes, &tmp))
		return 0;

	return 1;
}

/**
 * \brief Removes a stroke from the rule.
 *
 * \param self Rule.
 * \param index Stroke index.
 */
void
ligen_rule_remove_stroke (LIGenRule* self,
                          int        index)
{
	lisys_assert (index >= 0);
	lisys_assert (index < self->strokes.count);

	lialg_array_remove (&self->strokes, index);
}

int
ligen_rule_set_name (LIGenRule*  self,
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

/** @} */
/** @} */
