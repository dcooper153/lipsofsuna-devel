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
 * \addtogroup ligenRule Rule
 * @{
 */

#include <stdlib.h>
#include <algorithm/lips-algorithm.h>
#include <system/lips-system.h>
#include "generator-rule.h"

/**
 * \brief Creates a new rule.
 *
 * \return New rule or NULL.
 */
ligenRule*
ligen_rule_new ()
{
	ligenRule* self;

	/* Allocate self. */
	self = calloc (1, sizeof (ligenRule));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Allocate name. */
	self->name = calloc (1, sizeof (char));
	if (self->name == NULL)
	{
		free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the rule.
 *
 * \param self Rule.
 */
void
ligen_rule_free (ligenRule* self)
{
	free (self->strokes.array);
	free (self);
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
ligen_rule_insert_stroke (ligenRule* self,
                          int        x,
                          int        y,
                          int        z,
                          int        flags,
                          int        brush)
{
	ligenRulestroke tmp;

	tmp.pos[0] = x;
	tmp.pos[1] = y;
	tmp.pos[2] = z;
	tmp.flags = flags;
	tmp.brush = brush;
	if (!lialg_array_append (&self->strokes, &tmp))
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	return 1;
}

/** @} */
/** @} */
