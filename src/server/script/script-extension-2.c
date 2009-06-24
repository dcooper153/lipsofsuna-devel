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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvscr Script
 * @{
 * \addtogroup lisrvscrExtension Extension
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "lips-server-script.h"

/* @luadoc
 * module "Core.Server.Extension"
 * ---
 * -- Load extensions.
 * -- @name Extension
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Loads an extension.
 * --
 * -- @param self Extension class.
 * -- @param name Extension name.
 * -- @return True on success.
 * function Extension.new(self, name)
 */
static int
Extension_new (lua_State* lua)
{
	int ret;
	const char* name;
	lisrvServer* server;

	server = liscr_checkclassdata (lua, 1, LISRV_SCRIPT_EXTENSION);
	name = luaL_checkstring (lua, 2);

	ret = lisrv_server_load_extension (server, name);
	lua_pushboolean (lua, ret);
	return 1;
}

/*****************************************************************************/

void
lisrvExtensionScript (liscrClass* self,
                      void*       data)
{
	liscr_class_set_userdata (self, LISRV_SCRIPT_EXTENSION, data);
	liscr_class_insert_func (self, "new", Extension_new);
}

/** @} */
/** @} */
/** @} */
