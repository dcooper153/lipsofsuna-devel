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
 * \addtogroup LIExtSkeleton Skeleton
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Skeleton"
 * --- Example extension.
 * -- @name Skeleton
 * -- @class table
 */

/* @luadoc
 * --- Example function.
 * --
 * -- @param self Skeleton class.
 * Skeleton.test(self)
 */
static void Skeleton_test (LIScrArgs* args)
{
	printf ("Skeleton.test\n");
}

/*****************************************************************************/

void liext_script_skeleton (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKELETON, data);
	liscr_class_insert_cfunc (self, "test", Skeleton_test);
}

/** @} */
/** @} */
