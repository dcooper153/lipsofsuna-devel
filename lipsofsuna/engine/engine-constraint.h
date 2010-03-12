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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup LIEngConstraint Constraint
 * @{
 */

#ifndef __ENGINE_CONSTRAINT_H__
#define __ENGINE_CONSTRAINT_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "engine-types.h"

struct _LIEngConstraint
{
	char* node_names[2];
	LIEngConstraint* prev;
	LIEngConstraint* next;
	LIEngObject* objects[2];
	LIMdlNode* nodes[2];
};

LIAPICALL (LIEngConstraint*, lieng_constraint_new, (
	LIEngObject* first_object,
	const char*  first_anchor,
	LIEngObject* second_object,
	const char*  second_anchor));

LIAPICALL (void, lieng_constraint_free, (
	LIEngConstraint* self));

LIAPICALL (void, lieng_constraint_update, (
	LIEngConstraint* self,
	float            secs));

LIAPICALL (void, lieng_constraint_rebuild, (
	LIEngConstraint* self));

LIAPICALL (void, lieng_constraint_get_transform, (
	LIEngConstraint* self,
	int              index,
	LIEngMatrix      type,
	LIMatTransform*  value));

/** @} */
/** @} */

#endif
