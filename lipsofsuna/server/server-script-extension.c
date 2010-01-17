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
 * \addtogroup liser Server
 * @{
 * \addtogroup liserscr Script
 * @{
 * \addtogroup liserscrExtension Extension
 * @{
 */

#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>

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
	LISerServer* server;

	server = liscr_checkclassdata (lua, 1, LISER_SCRIPT_EXTENSION);
	name = luaL_checkstring (lua, 2);

	ret = limai_program_insert_extension (server->program, name);
	if (!ret)
		lisys_error_report ();
	lua_pushboolean (lua, ret);
	return 1;
}

/*****************************************************************************/

void
liser_script_extension (LIScrClass* self,
                        void*       data)
{
	liscr_class_set_userdata (self, LISER_SCRIPT_EXTENSION, data);
	liscr_class_insert_func (self, "new", Extension_new);
}

/** @} */
/** @} */
/** @} */
