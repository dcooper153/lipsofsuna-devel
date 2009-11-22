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
 * \addtogroup liscr Script
 * @{
 * \addtogroup liscrClass Class
 * @{
 */

#include <ai/lips-ai.h>
#include <script/lips-script.h>

/* @luadoc
 * module "Core.Common.Class"
 * ---
 * -- Inherit classes.
 * -- @name Class
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Inherits a class from another.
 * --
 * -- @param self Class class.
 * -- @param base Base class.
 * -- @param name Class name used for serialization.
 * -- @return New class.
 * function Class.new(self, base, name)
 */
static int
Class_new (lua_State* lua)
{
	const char* name;
	liscrClass* base;
	liscrClass* self;
	liscrScript* script;

	script = liscr_checkclassdata (lua, 1, LICOM_SCRIPT_CLASS);
	base = liscr_checkanyclass (lua, 2);
	name = luaL_checkstring (lua, 3);

	self = liscr_script_find_class (script, name);
	if (self != NULL)
		luaL_argerror (lua, 3, "duplicate class name");

	self = liscr_class_new_full (script, base, name, 0);
	if (self == NULL)
		return 0;
	if (!liscr_script_insert_class (script, self))
	{
		liscr_class_free (self);
		return 0;
	}
	liscr_pushclass (lua, self);

	return 1;
}

/*****************************************************************************/

void
licomClassScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LICOM_SCRIPT_CLASS, data);
	liscr_class_insert_func (self, "new", Class_new);
}

/** @} */
/** @} */
