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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliPackager Packager
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"
#include "ext-packager.h"

/* @luadoc
 * module "Extension.Client.Packager"
 * ---
 * -- Automatize data package creation.
 * -- @name Options
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Cancels any ongoing packaging process.
 * --
 * -- @param self Packager class.
 * function Packager.cancel(self)
 */
static int
Packager_cancel (lua_State* lua)
{
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_PACKAGER);

	liext_packager_cancel (self->packager);

	return 0;
}

/* @luadoc
 * ---
 * -- Creates data package.
 * --
 * -- @param self Packager class.
 * -- @param name File name.
 * function Packager.save(self, name)
 */
static int
Packager_save (lua_State* lua)
{
	const char* name;
	const char* dir;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_PACKAGER);
	name = luaL_checkstring (lua, 2);
	dir = luaL_checkstring (lua, 3);

	if (!liext_packager_save (self->packager, name, dir))
		lua_pushboolean (lua, 0);
	else
		lua_pushboolean (lua, 1);

	return 1;
}

/* @luadoc
 * ---
 * -- Boolean switch for verbose debug messages.
 * -- @name Packager.verbose
 * -- @class table
 */
static int
Packager_getter_verbose (lua_State* lua)
{
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_PACKAGER);

	lua_pushboolean (lua, liext_packager_get_verbose (self->packager));

	return 0;
}
static int
Packager_setter_verbose (lua_State* lua)
{
	int value;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_PACKAGER);
	value = lua_toboolean (lua, 3);

	liext_packager_set_verbose (self->packager, value);

	return 0;
}

/*****************************************************************************/

void
liextPackagerScript (liscrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_PACKAGER, data);
	liscr_class_insert_func (self, "cancel", Packager_cancel);
	liscr_class_insert_func (self, "save", Packager_save);
	liscr_class_insert_getter (self, "verbose", Packager_getter_verbose);
	liscr_class_insert_setter (self, "verbose", Packager_setter_verbose);
}

/** @} */
/** @} */
/** @} */
