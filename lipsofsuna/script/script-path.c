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
 * \addtogroup liscr Script
 * @{
 * \addtogroup liscrPath Path
 * @{
 */

#include <lipsofsuna/ai.h>
#include <lipsofsuna/script.h>

/* @luadoc
 * module "Core.Common.Path"
 * ---
 * -- Specify paths for your objects to follow.
 * -- @name Path
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Pops a position vector from the beginning of the path.
 * --
 * -- @param self Path.
 * -- @return New vector.
 * function Path.pop(self)
 */
static void Path_pop (LIScrArgs* args)
{
	int index;
	LIMatVector tmp;

	index = liai_path_get_position (args->self);
	if (index < liai_path_get_length (args->self))
	{
		liai_path_get_point (args->self, index, &tmp);
		liai_path_set_position (args->self, index + 1);
		liscr_args_seti_vector (args, &tmp);
	}
}

/* @luadoc
 * ---
 * -- Length.
 * -- @name Path.length
 * -- @class table
 */
static void Path_getter_length (LIScrArgs* args)
{
	liscr_args_seti_int (args,
		liai_path_get_length (args->self) -
		liai_path_get_position (args->self));
}

/*****************************************************************************/

void
liscr_script_path (LIScrClass* self,
                 void*       data)
{
	liscr_class_inherit (self, liscr_script_class, NULL);
	liscr_class_insert_mfunc (self, "pop", Path_pop);
	liscr_class_insert_mvar (self, "length", Path_getter_length, NULL);
}

/** @} */
/** @} */
