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
 * \addtogroup licliscrWindow Window
 * @{
 */

#include <client/lips-client.h>
#include "lips-client-script.h"

/* @luadoc
 * module "Core.Client.Window"
 * ---
 * -- Pack widgets in dialogs.
 * -- @name Window
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new window.
 * --
 * -- @param self Window class.
 * -- @param width Number of columns to create.
 * -- @param height Number of rows to create.
 * -- @return New window.
 * function Window.new(self, width, height)
 */
static int
Window_new (lua_State* lua)
{
	int width;
	int height;
	liwdgWidget* window;
	liscrData* self;
	liscrScript* script = liscr_script (lua);
	licliModule* module = liscr_script_get_userdata (script);

	/* Check arguments. */
	width = (int) luaL_checkint (lua, 2);
	height = (int) luaL_checkint (lua, 3);
	luaL_argcheck (lua, width >= 0, 2, "invalid size");
	luaL_argcheck (lua, height >= 0, 3, "invalid size");

	/* Allocate userdata. */
	window = liwdg_window_new (module->widgets, width, height);
	if (window == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	self = liscr_data_new (script, window, LICLI_SCRIPT_WINDOW);
	if (self == NULL)
	{
		liwdg_widget_free (window);
		lua_pushnil (lua);
		return 1;
	}
	if (!liwdg_manager_insert_window (module->widgets, window))
	{
		lua_pushnil (lua);
		return 1;
	}
	liwdg_widget_set_userdata (window, self);

	liscr_pushdata (lua, self);
	return 1;
}

/* @luadoc
 * ---
 * -- Title string.
 * -- @name Window.title
 * -- @class table
 */
static int
Window_getter_title (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WINDOW);

	lua_pushstring (lua, liwdg_window_get_title (self->data));
	return 1;
}
static int
Window_setter_title (lua_State* lua)
{
	const char* value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WINDOW);
	value = luaL_checkstring (lua, 3);

	liwdg_window_set_title (self->data, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Visibility flag.
 * --
 * -- Gets or sets the visibility of the window. Visible windows are
 * -- protected from garbage collection.
 * --
 * -- @name Window.visible
 * -- @class table
 */
static int
Window_getter_visible (lua_State* lua)
{
	int value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WINDOW);

	value = liwdg_widget_get_visible (self->data);
	lua_pushboolean (lua, value);

	return 1;
}
static int
Window_setter_visible (lua_State* lua)
{
	int value;
	licliModule* module;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WINDOW);
	value = lua_toboolean (lua, 3);
	module = liscr_class_get_userdata (self->clss, LICLI_SCRIPT_WINDOW);

	if (liwdg_widget_get_visible (self->data) == value)
		return 0;
	liwdg_widget_set_visible (self->data, value);
	if (value)
	{
		if (!liwdg_manager_insert_window (module->widgets, self->data))
		{
			liwdg_widget_set_visible (self->data, 0);
			return 0;
		}
		liscr_data_ref (self, NULL);
	}
	else
	{
		liwdg_widget_detach (self->data);
		liscr_data_unref (self, NULL);
	}

	return 0;
}

/*****************************************************************************/

void
licliWindowScript (liscrClass* self,
                   void*       data)
{
	liscr_class_inherit (self, licliGroupScript, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_WINDOW, data);
	liscr_class_insert_func (self, "new", Window_new);
	liscr_class_insert_getter (self, "title", Window_getter_title);
	liscr_class_insert_getter (self, "visible", Window_getter_visible);
	liscr_class_insert_setter (self, "title", Window_setter_title);
	liscr_class_insert_setter (self, "visible", Window_setter_visible);
}

/** @} */
/** @} */
/** @} */
