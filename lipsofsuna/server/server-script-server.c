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
 * \addtogroup liserscrServer Server
 * @{
 */

#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>

/* @luadoc
 * module "Core.Server.Server"
 * ---
 * -- Control the global server state.
 * -- @name Server
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Request server save.
 * --
 * -- @param self Server class.
 * function Server.save(self)
 */
static void Server_save (LIScrArgs* args)
{
	LISerServer* server;

	server = liscr_class_get_userdata (args->clss, LISER_SCRIPT_SERVER);
	if (!liser_server_save (server))
		lisys_error_report ();
}

/*****************************************************************************/

void
liser_script_server (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LISER_SCRIPT_SERVER, data);
	liscr_class_insert_cfunc (self, "save", Server_save);
}

/** @} */
/** @} */
/** @} */
