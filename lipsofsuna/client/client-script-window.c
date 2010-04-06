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

#include <lipsofsuna/client.h>

/* @luadoc
 * module "Core.Client.Window"
 * --- Pack widgets in dialogs.
 * -- @name Window
 * -- @class table
 */

/* @luadoc
 * --- Creates a new window.
 * --
 * -- @param clss Window class.
 * -- @param args Arguments.
 * -- @return New window.
 * function Window.new(clss)
 */
static void Window_new (LIScrArgs* args)
{
	LICliClient* client;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_WINDOW);
	self = liwdg_window_new (client->widgets, 0, 0);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LICLI_SCRIPT_WINDOW, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
}

/* @luadoc
 * --- Title string.
 * -- @name Window.title
 * -- @class table
 */
static void Window_getter_title (LIScrArgs* args)
{
	liscr_args_seti_string (args, liwdg_window_get_title (args->self));
}
static void Window_setter_title (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liwdg_window_set_title (args->self, value);
}

/* @luadoc
 * --- Visibility flag.
 * --
 * -- Gets or sets the visibility of the window. Visible windows are
 * -- protected from garbage collection.
 * --
 * -- @name Window.visible
 * -- @class table
 */
static void Window_getter_visible (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_visible (args->self));
}
static void Window_setter_visible (LIScrArgs* args)
{
	int value;
	LICliClient* client;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_WINDOW);
		if (liwdg_widget_get_visible (args->self) == value)
			return;
		liwdg_widget_set_visible (args->self, value);
		if (value)
		{
			if (!liwdg_manager_insert_window (client->widgets, args->self))
				liwdg_widget_set_visible (args->self, 0);
			else
				liscr_data_ref (args->data, NULL);
		}
		else
		{
			liwdg_widget_detach (args->self);
			liscr_data_unref (args->data, NULL);
		}
	}
}

/*****************************************************************************/

void
licli_script_window (LIScrClass* self,
                     void*       data)
{
	liscr_class_inherit (self, licli_script_group, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_WINDOW, data);
	liscr_class_insert_cfunc (self, "new", Window_new);
	liscr_class_insert_mvar (self, "title", Window_getter_title, Window_setter_title);
	liscr_class_insert_mvar (self, "visible", Window_getter_visible, Window_setter_visible);
}

/** @} */
/** @} */
