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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/client.h>

/* @luadoc
 * module "Core.Client.Client"
 * --- Access and manipulate the state of the client.
 * -- @name Client
 * -- @class table
 */

/* @luadoc
 * --- Cycles widget focus.
 * --
 * -- @param clss Client class.
 * -- @param args Arguments.<ul>
 * --   <li>backward: True if should cycle backward.</li></ul>
 * function Client.cycle_focus(clss, args)
 */
static void Client_cycle_focus (LIScrArgs* args)
{
	int prev = 0;
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_focus (client->widgets, !prev);
}

/* @luadoc
 * --- Cycles window focus.
 * --
 * -- @param clss Client class.
 * -- @param args Arguments.<ul>
 * --   <li>backward: True if should cycle backward.</li></ul>
 * function Client.cycle_focus(clss, args)
 */
static void Client_cycle_window_focus (LIScrArgs* args)
{
	int prev = 0;
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_window_focus (client->widgets, !prev);
}

/* @luadoc
 * --- Launches a server.
 * --
 * -- @param clss Client class.
 * -- @param args Arguments.<ul>
 * --   <li>1,args: String to pass to the server.</li></ul>
 * -- @return True on success.
 * function Client.host(clss, args)
 */
static void Client_host (LIScrArgs* args)
{
	const char* str = NULL;
	LICliClient* client;

	if (!liscr_args_gets_string (args, "args", &str))
	    liscr_args_geti_string (args, 0, &str);

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	if (!licli_client_host (client, str))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- Gets the current cursor position.
 * -- @name Client.cursor_pos
 * -- @class table
 */
static void Client_getter_cursor_pos (LIScrArgs* args)
{
	int x;
	int y;
	LICliClient* client;
	LIMatVector tmp;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	client->video.SDL_GetMouseState (&x, &y);
	tmp = limat_vector_init (x, client->widgets->height - y - 1, 0.0f);
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * --- Short term average frames per second.
 * --
 * -- @name Client.tick
 * -- @class table
 */
static void Client_getter_fps (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_float (args, client->program->fps);
}

/* @luadoc
 * --- Movement mode flag.
 * -- @name Client.moving
 * -- @class table
 */
static void Client_getter_moving (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_bool (args, licli_client_get_moving (client));
}
static void Client_setter_moving (LIScrArgs* args)
{
	int value;
	LICliClient* client;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
		licli_client_set_moving (client, value);
	}
}

/* @luadoc
 * --- Main window title.
 * -- @name Client.title
 * -- @class table
 */
static void Client_setter_title (LIScrArgs* args)
{
	const char* value;
	LICliClient* client;

	if (liscr_args_geti_string (args, 0, &value))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
		client->video.SDL_WM_SetCaption (value, value);
	}
}

/*****************************************************************************/

void
licli_script_client (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_CLIENT, data);
	liscr_class_insert_cfunc (self, "cycle_focus", Client_cycle_focus);
	liscr_class_insert_cfunc (self, "cycle_window_focus", Client_cycle_window_focus);
	liscr_class_insert_cfunc (self, "host", Client_host);
	liscr_class_insert_cvar (self, "cursor_pos", Client_getter_cursor_pos, NULL);
	liscr_class_insert_cvar (self, "fps", Client_getter_fps, NULL);
	liscr_class_insert_cvar (self, "moving", Client_getter_moving, Client_setter_moving);
	liscr_class_insert_cvar (self, "title", NULL, Client_setter_title);
}

/** @} */
/** @} */

