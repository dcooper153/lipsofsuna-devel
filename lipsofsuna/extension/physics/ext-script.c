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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtPhysics Physics
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Physics"
 * --- Example extension.
 * -- @name Physics
 * -- @class table
 */

/* @luadoc
 * --- Example function.
 * --
 * -- @param self Physics class.
 * Physics.test(self)
 */
static void Physics_test (LIScrArgs* args)
{
	printf ("Physics.test\n");
}

/*****************************************************************************/

void liext_script_physics (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_PHYSICS, data);
	liscr_class_inherit (self, liscr_script_class, NULL);
	liscr_class_insert_cfunc (self, "test", Physics_test);
}

/** @} */
/** @} */
