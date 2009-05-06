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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndConstraint Constraint
 * @{
 */

#ifndef __RENDER_CONSTRAINT_H__
#define __RENDER_CONSTRAINT_H__

#include <model/lips-model.h>
#include "render-types.h"

struct _lirndConstraint
{
	lirndConstraint* prev;
	lirndConstraint* next;
	lirndObject* objects[2];
	limdlNode* nodes[2];
};

lirndConstraint*
lirnd_constraint_new (lirndObject* first_object,
                      const char*     first_anchor,
                      lirndObject* second_object,
                      const char*     second_anchor);

void
lirnd_constraint_free (lirndConstraint* self);

void
lirnd_constraint_update (lirndConstraint* self,
                         float            secs);

void
lirnd_constraint_get_transform (lirndConstraint* self,
                                int              index,
                                lirndMatrix      type,
                                limatTransform*  value);

/** @} */
/** @} */

#endif
