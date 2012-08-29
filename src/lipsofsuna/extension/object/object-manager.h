/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __EXT_OBJECT_MODULE_H__
#define __EXT_OBJECT_MODULE_H__

#include "lipsofsuna/extension.h"
#include "object.h"
#include "object-sector.h"
#include "object-types.h"

#define LIEXT_SCRIPT_OBJECT "Object"

struct _LIObjManager
{
	LIMaiProgram* program;
	LIAlgU32dic* objects;
	LICalHandle calls[1];
};

LIAPICALL (LIObjManager*, liobj_manager_new, (
	LIMaiProgram* program));

LIAPICALL (void, liobj_manager_free, (
	LIObjManager* self));

LIAPICALL (LIObjObject*, liobj_manager_find_object, (
	LIObjManager* self,
	uint32_t      id));

LIAPICALL (void, liobj_manager_notify_object_motion, (
	LIObjManager* self,
	LIObjObject*  object));

/*****************************************************************************/

void liext_script_object (
	LIScrScript* self);

#endif
