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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengConstraint Constraint
 * @{
 */

#ifndef __ENGINE_CONSTRAINT_H__
#define __ENGINE_CONSTRAINT_H__

#include <model/lips-model.h>
#include "engine-types.h"

struct _liengConstraint
{
	char* node_names[2];
	liengConstraint* prev;
	liengConstraint* next;
	liengObject* objects[2];
	limdlNode* nodes[2];
};

liengConstraint*
lieng_constraint_new (liengObject* first_object,
                      const char*  first_anchor,
                      liengObject* second_object,
                      const char*  second_anchor);

void
lieng_constraint_free (liengConstraint* self);

void
lieng_constraint_update (liengConstraint* self,
                         float            secs);

void
lieng_constraint_rebuild (liengConstraint* self);

void
lieng_constraint_get_transform (liengConstraint* self,
                                int              index,
                                liengMatrix      type,
                                limatTransform*  value);

/** @} */
/** @} */

#endif
