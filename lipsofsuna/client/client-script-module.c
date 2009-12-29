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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 */

#include <network/lips-network.h>
#include <system/lips-system.h>
#include "lips-client.h"

/* @luadoc
 * module "Core.Client.Module"
 * ---
 * -- Access and manipulate the state of the active module.
 * -- @name Module
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Cycles widget focus.
 * --
 * -- Arguments:
 * -- @param backward: True if should cycle backward.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * function Module.cycle_focus(self, args)
 */
static void Module_cycle_focus (liscrArgs* args)
{
	int prev = 0;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_focus (module->widgets, !prev);
}

/* @luadoc
 * ---
 * -- Cycles window focus.
 * --
 * -- Arguments:
 * -- @param backward: True if should cycle backward.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * function Module.cycle_focus(self, args)
 */
static void Module_cycle_window_focus (liscrArgs* args)
{
	int prev = 0;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_window_focus (module->widgets, !prev);
}

/* @luadoc
 * ---
 * -- Finds an object by ID.
 * --
 * -- Arguments:
 * -- id: Object ID.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * -- @return Object or nil.
 * function Module.find_object(self, args)
 */
static void Module_find_object (liscrArgs* args)
{
	int id;
	liengObject* object;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	if (!liscr_args_gets_int (args, "id", &id))
		return;
	object = lieng_engine_find_object (module->engine, id);
	if (object != NULL)
		liscr_args_seti_data (args, object->script);
}

/* @luadoc
 * ---
 * -- Launches a server and joins it.
 * --
 * -- If a server has already been launched, it is terminated.
 * --
 * -- Arguments:
 * -- login: Login name.
 * -- password: Password.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * -- @return True on success.
 * function Module.host(self, args)
 */
static void Module_host (liscrArgs* args)
{
	const char* name = NULL;
	const char* pass = NULL;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	liscr_args_gets_string (args, "login", &name);
	liscr_args_gets_string (args, "password", &pass);
	if (!licli_module_host (module))
	{
		lisys_error_report ();
		return;
	}
	if (!licli_module_connect (module, name, pass))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Joins a server.
 * --
 * -- Arguments:
 * -- login: Login name.
 * -- password: Password.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * -- @return True on success.
 * function Module.join(self, args)
 */
static void Module_join (liscrArgs* args)
{
	const char* name = NULL;
	const char* pass = NULL;
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	liscr_args_gets_string (args, "login", &name);
	liscr_args_gets_string (args, "password", &pass);
	if (!licli_module_connect (module, name, pass))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Sends a network packet to the server.
 * --
 * -- Arguments:
 * -- packet: Packet.
 * -- reliable: True for reliable.
 * --
 * -- @param self Module class.
 * -- @param args Arguments.
 * function Module.send(self, args)
 */
static void Module_send (liscrArgs* args)
{
	int reliable = 1;
	licliModule* module;
	liscrData* data;
	liscrPacket* packet;

	if (liscr_args_gets_data (args, "packet", LICOM_SCRIPT_PACKET, &data))
	{
		module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
		packet = data->data;
		if (packet->writer != NULL)
		{
			liscr_args_gets_bool (args, "reliable", &reliable);
			if (reliable)
				licli_module_send (module, packet->writer, GRAPPLE_RELIABLE);
			else
				licli_module_send (module, packet->writer, 0);
		}
	}
}

/* @luadoc
 * ---
 * -- Gets the current cursor position.
 * -- @name Module.cursor_pos
 * -- @class table
 */
static void Module_getter_cursor_pos (liscrArgs* args)
{
	int x;
	int y;
	licliModule* module;
	limatVector tmp;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	module->client->video.SDL_GetMouseState (&x, &y);
	tmp = limat_vector_init (x, module->widgets->height - y - 1, 0.0f);
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * ---
 * -- Movement mode flag.
 * -- @name Module.moving
 * -- @class table
 */
static void Module_getter_moving (liscrArgs* args)
{
	licliModule* module;

	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	liscr_args_seti_bool (args, licli_module_get_moving (module));
}
static void Module_setter_moving (liscrArgs* args)
{
	int value;
	licliModule* module;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
		licli_module_set_moving (module, value);
	}
}

/* @luadoc
 * ---
 * -- Root widget.
 * -- @name Module.root
 * -- @class table
 */
static void Module_setter_root (liscrArgs* args)
{
	licliModule* module;
	liscrData* data;
	liwdgWidget* window;

	/* Detach old root widget. */
	module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
	window = liwdg_manager_get_root (module->widgets);
	if (window != NULL)
	{
		liscr_data_unref (liwdg_widget_get_userdata (window), NULL);
		liwdg_widget_set_visible (window, 0);
	}
	liwdg_manager_set_root (module->widgets, NULL);

	/* Set new root window. */
	if (liscr_args_geti_data (args, 0, LICLI_SCRIPT_WIDGET, &data))
	{
		window = data->data;
		if (window->parent != NULL || window->state != LIWDG_WIDGET_STATE_DETACHED)
			return;
		liwdg_manager_set_root (module->widgets, data->data);
		liscr_data_ref (data, NULL);
	}
}

/* @luadoc
 * ---
 * -- Main window title.
 * -- @name Module.title
 * -- @class table
 */
static void Module_setter_title (liscrArgs* args)
{
	const char* value;
	licliModule* module;

	if (liscr_args_geti_string (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_MODULE);
		module->client->video.SDL_WM_SetCaption (value, value);
	}
}

/*****************************************************************************/

void
licliModuleScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_MODULE, data);
	liscr_class_insert_cfunc (self, "cycle_focus", Module_cycle_focus);
	liscr_class_insert_cfunc (self, "cycle_window_focus", Module_cycle_window_focus);
	liscr_class_insert_cfunc (self, "find_object", Module_find_object);
	liscr_class_insert_cfunc (self, "host", Module_host);
	liscr_class_insert_cfunc (self, "join", Module_join);
	liscr_class_insert_cfunc (self, "send", Module_send);
	liscr_class_insert_mvar (self, "cursor_pos", Module_getter_cursor_pos, NULL);
	liscr_class_insert_mvar (self, "moving", Module_getter_moving, Module_setter_moving);
	liscr_class_insert_mvar (self, "root", NULL, Module_setter_root);
	liscr_class_insert_mvar (self, "title", NULL, Module_setter_title);
}

/** @} */
/** @} */
