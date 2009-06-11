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
 * \addtogroup liextcliWidgets Widgets
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"

static int
private_callback_activated (liscrData* data)
{
	liscrScript* script = liscr_data_get_script (data);

	liscr_pushdata (script->lua, data);
	lua_getfield (script->lua, -1, "activated");
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		if (lua_pcall (script->lua, 1, 0, 0) != 0)
		{
			lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
		return 0;
	}
	else
		lua_pop (script->lua, 2);
	return 1;
}

static int
private_callback_menu (liscrData* data,
                       int        id)
{
	liscrScript* script = liscr_data_get_script (data);

	liscr_pushdata (script->lua, data);
	lua_getfield (script->lua, -1, "callback");
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (script->lua, -3);
		lua_pushnumber (script->lua, id);
		lua_remove (script->lua, -4);
		if (lua_pcall (script->lua, 2, 0, 0) != 0)
		{
			lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
		return 0;
	}
	else
		lua_pop (script->lua, 2);
	return 1;
}

static int
private_callback_pressed (liscrData* data)
{
	liscrScript* script = liscr_data_get_script (data);

	liscr_pushdata (script->lua, data);
	lua_getfield (script->lua, -1, "pressed");
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		if (lua_pcall (script->lua, 1, 0, 0) != 0)
		{
			lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
		return 0;
	}
	else
		lua_pop (script->lua, 2);
	return 1;
}

/*****************************************************************************/

/* @luadoc
 * module "Extension.Client.Widgets"
 * ---
 * -- Create buttons.
 * -- @name Button
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new button widget.
 * --
 * -- @param self Button class.
 * -- @param table Optional table of parameters.
 * -- @return New button widget.
 * function Button.new(self, table)
 */
static int
Button_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_BUTTON);

	/* Allocate widget. */
	widget = liwdg_button_new (module->module->widgets);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_BUTTON);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0, private_callback_pressed, self, NULL);
	liwdg_widget_set_userdata (widget, self);
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Displayed text.
 * -- @name Button.text
 * -- @class table
 */
static int
Button_getter_text (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_BUTTON);

	lua_pushstring (lua, liwdg_button_get_text (LIWDG_BUTTON (self->data)));
	return 1;
}
static int
Button_setter_text (lua_State* lua)
{
	const char* text;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_BUTTON);
	text = luaL_checkstring (lua, 3);

	liwdg_button_set_text (LIWDG_BUTTON (self->data), text);
	return 0;
}

/* @luadoc
 * ---
 * -- Edit text.
 * -- @name Entry
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new text entry widget.
 * --
 * -- @param self Entry class.
 * -- @param table Optional table of parameters.
 * -- @return New entry widget.
 * function Entry.new(self, table)
 */
static int
Entry_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_ENTRY);

	/* Allocate widget. */
	widget = liwdg_entry_new (module->module->widgets);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_ENTRY);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_ACTIVATED, 0, private_callback_activated, self, NULL);
	liwdg_widget_set_userdata (widget, self);
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Clears the entry.
 * --
 * -- @param self Entry.
 * function Entry.clear(self)
 */
static int
Entry_clear (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_ENTRY);

	liwdg_entry_clear (LIWDG_ENTRY (self->data));
	return 0;
}

/* @luadoc
 * ---
 * -- Displayed text.
 * -- @name Label.text
 * -- @class table
 */
static int
Entry_getter_text (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_ENTRY);

	lua_pushstring (lua, liwdg_entry_get_text (LIWDG_ENTRY (self->data)));
	return 1;
}
static int
Entry_setter_text (lua_State* lua)
{
	const char* text;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_ENTRY);
	text = luaL_checkstring (lua, 3);

	liwdg_entry_set_text (LIWDG_ENTRY (self->data), text);
	return 0;
}

/* @luadoc
 * ---
 * -- Show images.
 * -- @name Image
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new image widget.
 * --
 * -- @param self Image class.
 * -- @param table Optional table of parameters.
 * -- @return New image widget.
 * function Image.new(self, table)
 */
static int
Image_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_IMAGE);

	/* Allocate widget. */
	widget = liwdg_image_new (module->module->widgets);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_IMAGE);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0, private_callback_pressed, self, NULL);
	liwdg_widget_set_userdata (widget, self);
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Displayed image.
 * -- @name Image.image
 * -- @class table
 */
static int
Image_getter_image (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_IMAGE);

	lua_pushstring (lua, liwdg_image_get_image (LIWDG_IMAGE (self->data)));
	return 1;
}
static int
Image_setter_image (lua_State* lua)
{
	const char* image;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_IMAGE);
	image = luaL_checkstring (lua, 3);

	liwdg_image_set_image (LIWDG_IMAGE (self->data), image);
	return 0;
}

/* @luadoc
 * ---
 * -- Show text labels.
 * -- @name Label
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new label widget.
 * --
 * -- @param self Label class.
 * -- @param table Optional table of parameters.
 * -- @return New label widget.
 * function Label.new(self, table)
 */
static int
Label_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_LABEL);

	/* Allocate widget. */
	widget = liwdg_label_new (module->module->widgets);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_LABEL);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liwdg_widget_insert_callback (widget, LIWDG_CALLBACK_PRESSED, 0, private_callback_pressed, self, NULL);
	liwdg_widget_set_userdata (widget, self);
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Displayed text.
 * -- @name Label.text
 * -- @class table
 */
static int
Label_getter_text (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_LABEL);

	lua_pushstring (lua, liwdg_label_get_text (LIWDG_LABEL (self->data)));
	return 1;
}
static int
Label_setter_text (lua_State* lua)
{
	const char* text;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_LABEL);
	text = luaL_checkstring (lua, 3);

	liwdg_label_set_text (LIWDG_LABEL (self->data), text);
	return 0;
}

/* @luadoc
 * ---
 * -- Show menus.
 * -- @name Menu
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new menu widget.
 * --
 * -- @param self Menu class.
 * -- @param table Optional table of parameters.
 * -- @return New menu widget.
 * function Menu.new(self, table)
 */
static int
Menu_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_MENU);

	/* Allocate widget. */
	widget = liwdg_menu_new (module->module->widgets);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_MENU);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liwdg_widget_set_userdata (widget, self);
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Inserts a menu group to the menu.
 * --
 * -- @param self Menu.
 * -- @param group Menu group.
 * function Menu.insert(self, group)
 */
static int
Menu_insert (lua_State* lua)
{
	liscrData* self;
	liscrData* group;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_MENU);
	group = liscr_checkdata (lua, 2, LIEXT_SCRIPT_MENUGROUP);

	if (liwdg_menu_insert_group (self->data, group->data))
		liscr_data_ref (group, self);
	return 0;
}

/* @luadoc
 * ---
 * -- Font.
 * -- @name Menu.font
 * -- @class table
 */
static int
Menu_getter_font (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_MENU);

//	lua_pushstring (lua, liwdg_menu_get_font (LIWDG_MENU (self->data)));
	lua_pushstring (lua, "FIXME");
	return 1;
}
static int
Menu_setter_font (lua_State* lua)
{
	const char* text;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_MENU);
	text = luaL_checkstring (lua, 3);

//	liwdg_menu_set_font (LIWDG_MENU (self->data), text);
	return 0;
}

/* @luadoc
 * ---
 * -- Displayed orientation.
 * -- @name Menu.orientation
 * -- @class table
 */
static int
Menu_getter_orientation (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_MENU);

	lua_pushnumber (lua, liwdg_menu_get_vertical (LIWDG_MENU (self->data)));
	return 1;
}
static int
Menu_setter_orientation (lua_State* lua)
{
	int value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_MENU);
	value = luaL_checkinteger (lua, 3);

	liwdg_menu_set_vertical (LIWDG_MENU (self->data), value);
	return 0;
}

/* @luadoc
 * ---
 * -- Add menu items and handle menu clicks.
 * -- @name MenuGroup
 * -- @class table
 */

static int
MenuGroup___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isiface (lua, 1, LIEXT_SCRIPT_MENUGROUP);

	if (self->data != NULL)
		liwdg_menu_group_free (self->data);

	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new menu group.
 * --
 * -- @param self Menu group class.
 * -- @param markup Menu markup.
 * -- @param table Optional table of parameters.
 * -- @return New menu group.
 * function MenuGroup.new(self, markup, table)
 */
static int
MenuGroup_new (lua_State* lua)
{
	const char* markup;
	liextModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgMenuGroup* group;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_MENUGROUP);
	markup = luaL_checkstring (lua, 2);

	/* Allocate widget. */
	group = liwdg_menu_group_new (markup);
	if (group == NULL)
	{
		lisys_error_report ();
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, group, LIEXT_SCRIPT_MENUGROUP);
	if (self == NULL)
	{
		liwdg_menu_group_free (group);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 3))
		liscr_copyargs (lua, self, 3);

	liwdg_menu_group_set_callback (group, LIWDG_HANDLER (private_callback_menu), self);
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/* @luadoc
 * ---
 * -- Menu click callback.
 * -- @name MenuGroup.callback
 * -- @class table
 */

/*****************************************************************************/

void
liextButtonScript (liscrClass* self,
                   void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_BUTTON, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "new", Button_new);
	liscr_class_insert_getter (self, "text", Button_getter_text);
	liscr_class_insert_setter (self, "text", Button_setter_text);
}

void
liextEntryScript (liscrClass* self,
                  void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_ENTRY, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "new", Entry_new);
	liscr_class_insert_func (self, "clear", Entry_clear);
	liscr_class_insert_getter (self, "text", Entry_getter_text);
	liscr_class_insert_setter (self, "text", Entry_setter_text);
}

void
liextImageScript (liscrClass* self,
                  void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_IMAGE, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "new", Image_new);
	liscr_class_insert_getter (self, "image", Image_getter_image);
	liscr_class_insert_setter (self, "image", Image_setter_image);
}

void
liextLabelScript (liscrClass* self,
                  void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_LABEL, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "new", Label_new);
	liscr_class_insert_getter (self, "text", Label_getter_text);
	liscr_class_insert_setter (self, "text", Label_setter_text);
}

void
liextMenuScript (liscrClass* self,
                 void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_MENU, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_enum (self, "HORIZONTAL", 0);
	liscr_class_insert_enum (self, "VERTICAL", 1);
	liscr_class_insert_func (self, "new", Menu_new);
	liscr_class_insert_func (self, "insert", Menu_insert);
	liscr_class_insert_getter (self, "font", Menu_getter_font);
	liscr_class_insert_getter (self, "orientation", Menu_getter_orientation);
	liscr_class_insert_setter (self, "font", Menu_setter_font);
	liscr_class_insert_setter (self, "orientation", Menu_setter_orientation);
}

void
liextMenuGroupScript (liscrClass* self,
                      void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_MENUGROUP, data);
	liscr_class_insert_func (self, "__gc", MenuGroup___gc);
	liscr_class_insert_func (self, "new", MenuGroup_new);
}

/** @} */
/** @} */
/** @} */
