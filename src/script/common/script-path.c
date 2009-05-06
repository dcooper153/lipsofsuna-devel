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
 * \addtogroup licom Common
 * @{
 * \addtogroup licomPath Path
 * @{
 */

#include <ai/lips-ai.h>
#include <script/lips-script.h>
#include "lips-common-script.h"

/* @luadoc
 * module "Core.Common.Path"
 * ---
 * -- Specify paths for your objects to follow.
 * -- @name Path
 * -- @class table
 */

static int
Path___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LICOM_SCRIPT_PATH);

	liai_path_free (self->data);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Pops a position vector from the beginning of the path.
 * --
 * -- @param self Path.
 * -- @return New vector.
 * function Path.pop(self)
 */
static int
Path_pop (lua_State* lua)
{
	int index;
	limatVector tmp;
	liscrData* self;
	liscrData* vector;
	liscrScript* script = liscr_script (lua);

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_PATH);

	index = liai_path_get_position (self->data);
	if (index == liai_path_get_length (self->data))
	{
		lua_pushnil (lua);
		return 1;
	}
	liai_path_get_point (self->data, index, &tmp);
	liai_path_set_position (self->data, index + 1);
	vector = liscr_vector_new (script, &tmp);
	if (vector != NULL)
	{
		liscr_pushdata (lua, vector);
		liscr_data_unref (vector, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}

/* @luadoc
 * ---
 * -- Length.
 * -- @name Path.length
 * -- @class table
 */
static int
Path_getter_length (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_PATH);

	lua_pushnumber (lua, liai_path_get_length (self->data) - liai_path_get_position (self->data));
	return 1;
}

/*****************************************************************************/

void
licomPathScript (liscrClass* self,
                 void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_insert_func (self, "__gc", Path___gc);
	liscr_class_insert_func (self, "pop", Path_pop);
	liscr_class_insert_getter (self, "length", Path_getter_length);
}

/** @} */
/** @} */
