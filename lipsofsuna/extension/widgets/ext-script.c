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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include "ext-module.h"

static int
private_callback_activated (LIScrData* data)
{
	LIScrScript* script = liscr_data_get_script (data);

	liscr_pushdata (script->lua, data);
	lua_getfield (script->lua, -1, "activated");
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		if (lua_pcall (script->lua, 1, 0, 0) != 0)
		{
			lisys_error_set (LISYS_ERROR_UNKNOWN, "Widget.activated: %s", lua_tostring (script->lua, -1));
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
private_callback_menu (LIScrData*     data,
                       LIWdgMenuItem* item)
{
	LIScrScript* script = liscr_data_get_script (data);

	/* Get callback function. */
	liscr_pushdata (script->lua, data);
	lua_pushlightuserdata (script->lua, item);
	lua_gettable (script->lua, -2);
	lua_remove (script->lua, -2);

	/* Call the callback. */
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		liscr_pushdata (script->lua, data);
		if (lua_pcall (script->lua, 1, 0, 0) != 0)
		{
			lisys_error_set (LISYS_ERROR_UNKNOWN, "Menu.callback: %s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
		return 0;
	}
	else
		lua_pop (script->lua, 1);

	return 1;
}

static int
private_callback_pressed (LIScrData* data)
{
	LIScrScript* script = liscr_data_get_script (data);

	liscr_pushdata (script->lua, data);
	lua_getfield (script->lua, -1, "pressed");
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		if (lua_pcall (script->lua, 1, 0, 0) != 0)
		{
			lisys_error_set (LISYS_ERROR_UNKNOWN, "Widget.pressed: %s", lua_tostring (script->lua, -1));
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
private_callback_pressed_tree (LIScrData*    data,
                               LIWdgTreerow* row)
{
	int i;
	LIScrScript* script = liscr_data_get_script (data);

	/* Find row index. */
	i = liwdg_treerow_get_index (row);

	/* Invoke callback. */
	liscr_pushdata (script->lua, data);
	lua_getfield (script->lua, -1, "pressed");
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		lua_pushnumber (script->lua, i + 1);
		if (lua_pcall (script->lua, 2, 0, 0) != 0)
		{
			lisys_error_set (LISYS_ERROR_UNKNOWN, "Tree.pressed: %s", lua_tostring (script->lua, -1));
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
 * module "Extension.Widgets"
 * --- Create buttons.
 * -- @name Button
 * -- @class table
 */

/* @luadoc
 * --- Creates a new button widget.
 * --
 * -- @param clss Button class.
 * -- @param args Arguments.
 * -- @return New button widget.
 * function Button.new(clss, args)
 */
static void Button_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_BUTTON);
	self = liwdg_button_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_BUTTON, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_insert_callback (self, "pressed", private_callback_pressed, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Displayed text.
 * -- @name Button.text
 * -- @class table
 */
static void Button_getter_text (LIScrArgs* args)
{
	liscr_args_seti_string (args, liwdg_button_get_text (args->self));
}
static void Button_setter_text (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liwdg_button_set_text (args->self, value);
}

/* @luadoc
 * --- Edit text.
 * -- @name Entry
 * -- @class table
 */

/* @luadoc
 * --- Creates a new text entry widget.
 * --
 * -- @param clss Entry class.
 * -- @param args Arguments.
 * -- @return New entry widget.
 * function Entry.new(clss, args)
 */
static void Entry_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_ENTRY);
	self = liwdg_entry_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_ENTRY, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "activated", private_callback_activated, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Clears the entry.
 * --
 * -- @param self Entry.
 * function Entry.clear(self)
 */
static void Entry_clear (LIScrArgs* args)
{
	liwdg_entry_clear (args->self);
}

/* @luadoc
 * --- Displayed text.
 * -- @name Entry.text
 * -- @class table
 */
static void Entry_getter_text (LIScrArgs* args)
{
	liscr_args_seti_string (args, liwdg_entry_get_text (args->self));
}
static void Entry_setter_text (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liwdg_entry_set_text (args->self, value);
}

/* @luadoc
 * --- Show images.
 * -- @name Image
 * -- @class table
 */

/* @luadoc
 * --- Creates a new image widget.
 * --
 * -- @param clss Image class.
 * -- @param args Arguments.
 * -- @return New image widget.
 * function Image.new(clss, args)
 */
static void Image_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_IMAGE);
	self = liwdg_image_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_IMAGE, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_insert_callback (self, "pressed", private_callback_pressed, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Displayed image.
 * -- @name Image.image
 * -- @class table
 */
static void Image_getter_image (LIScrArgs* args)
{
	liscr_args_seti_string (args, liwdg_image_get_image (args->self));
}
static void Image_setter_image (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liwdg_image_set_image (args->self, value);
}

/* @luadoc
 * --- Show text labels.
 * -- @name Label
 * -- @class table
 */

/* @luadoc
 * --- Creates a new label widget.
 * --
 * -- @param clss Label class.
 * -- @param args Arguments.
 * -- @return New label widget.
 * function Label.new(clss, args)
 */
static void Label_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_LABEL);
	self = liwdg_label_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_LABEL, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_insert_callback (self, "pressed", private_callback_pressed, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Displayed text.
 * -- @name Label.text
 * -- @class table
 */
static void Label_getter_text (LIScrArgs* args)
{
	liscr_args_seti_string (args, liwdg_label_get_text (args->self));
}
static void Label_setter_text (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liwdg_label_set_text (args->self, value);
}

/* @luadoc
 * --- Show menus.
 * -- @name Menu
 * -- @class table
 */

/* @luadoc
 * --- Creates a new menu widget.
 * --
 * -- @param clss Menu class.
 * -- @param args Arguments.
 * -- @return New menu widget.
 * function Menu.new(clss, args)
 */
static void Menu_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_MENU);
	self = liwdg_menu_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_MENU, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Gets the allocation rectangle of a menu item.
 * --
 * -- @param self Menu.
 * -- @param args Arguments.<ul>
 * --   <li>label: Item text.</li></ul>
 * -- @return Rectangle or nil.
 * function Menu.get_item_rect(self, args)
 */
static void Menu_get_item_rect (LIScrArgs* args)
{
	const char* name;
	LIWdgRect rect;

	if (!liscr_args_gets_string (args, "label", &name))
		return;
	if (!liwdg_menu_get_item_rect (args->self, name, &rect))
		return;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_float (args, "x", rect.x);
	liscr_args_sets_float (args, "y", rect.y);
	liscr_args_sets_float (args, "width", rect.width);
	liscr_args_sets_float (args, "height", rect.height);
}

/* @luadoc
 * --- Inserts a menu item to the menu.
 * --
 * -- @param self Menu.
 * -- @param args Arguments.<ul>
 * --   <li>label: Item label.</li>
 * --   <li>icon: Icon name.</li>
 * --   <li>pressed: Pressed callback.</li></ul>
 * function Menu.insert(self, args)
 */
static void Menu_insert (LIScrArgs* args)
{
	const char* label = "";
	const char* icon = "";
	LIWdgMenuItem* item;

	/* Create item. */
	liscr_args_gets_string (args, "label", &label);
	liscr_args_gets_string (args, "icon", &icon);
	if (!liwdg_menu_insert_item (args->self, label, icon, private_callback_menu, args->data))
		return;
	item = liwdg_menu_get_item (args->self, liwdg_menu_get_item_count (args->self) - 1);

	/* Callback. */
	if (args->input_table)
	{
		liscr_pushdata (args->lua, args->data);
		lua_pushlightuserdata (args->lua, item);
		lua_getfield (args->lua, args->input_table, "pressed");
		lua_settable (args->lua, -3);
		lua_pop (args->lua, 1);
	}
}

/* @luadoc
 * --- When set to true, the menu is hidden automatically after clicked.
 * -- @name Menu.autohide
 * -- @class table
 */
static void Menu_getter_autohide (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_menu_get_autohide (args->self));
}
static void Menu_setter_autohide (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_menu_set_autohide (args->self, value);
}

/* @luadoc
 * --- Displayed orientation, either "horz" or "vert".
 * -- @name Menu.orientation
 * -- @class table
 */
static void Menu_getter_orientation (LIScrArgs* args)
{
	if (liwdg_menu_get_vertical (args->self))
		liscr_args_seti_string (args, "vert");
	else
		liscr_args_seti_string (args, "horz");
}
static void Menu_setter_orientation (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value) && !strcmp (value, "horz"))
		liwdg_menu_set_vertical (args->self, 0);
	else
		liwdg_menu_set_vertical (args->self, 1);
}

/* @luadoc
 * --- Scroll bar widget.
 * -- @name Scroll
 * -- @class table
 */

/* @luadoc
 * --- Creates a new scroll bar widget.
 * --
 * -- @param self Scroll class.
 * -- @param args Arguments.
 * -- @return New scroll widget.
 * function Scroll.new(clss, args)
 */
static void Scroll_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SCROLL);
	self = liwdg_scroll_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_SCROLL, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_insert_callback (self, "pressed", private_callback_pressed, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Sets the scroll range.
 * --
 * -- @param self Scroll widget.
 * -- @param args Arguments.<ul>
 * --   <li>min: Minimum value.</li>
 * --   <li>max: Maximum value.</li></ul>
 * function Scroll.set_range(self, agrs)
 */
static void Scroll_set_range (LIScrArgs* args)
{
	float min = 0.0f;
	float max = 1.0f;

	liscr_args_gets_float (args, "min", &min);
	liscr_args_gets_float (args, "max", &max);
	if (max < min)
		max = min;
	liwdg_scroll_set_range (args->self, min, max);
}

/* @luadoc
 * --- Reference value.
 * -- @name Scroll.reference
 * -- @class table
 */
static void Scroll_getter_reference (LIScrArgs* args)
{
	liscr_args_seti_float (args, liwdg_scroll_get_reference (args->self));
}
static void Scroll_setter_reference (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		liwdg_scroll_set_reference (args->self, value);
}

/* @luadoc
 * --- Current value.
 * -- @name Scroll.value
 * -- @class table
 */
static void Scroll_getter_value (LIScrArgs* args)
{
	liscr_args_seti_float (args, liwdg_scroll_get_value (args->self));
}
static void Scroll_setter_value (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		liwdg_scroll_set_value (args->self, value);
}

/* @luadoc
 * --- Display and edit numeric values.
 * -- @name Spin
 * -- @class table
 */

/* @luadoc
 * --- Creates a new spin widget.
 * --
 * -- @param clss Spin class.
 * -- @param args Arguments.
 * -- @return New spin widget.
 * function Spin.new(clss, args)
 */
static void Spin_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SPIN);
	self = liwdg_spin_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_SPIN, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_insert_callback (self, "pressed", private_callback_pressed, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Current value.
 * -- @name Spin.value
 * -- @class table
 */
static void Spin_getter_value (LIScrArgs* args)
{
	liscr_args_seti_float (args, liwdg_spin_get_value (args->self));
}
static void Spin_setter_value (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		liwdg_spin_set_value (args->self, value);
}

/* @luadoc
 * --- Display a list of items.
 * -- @name Tree
 * -- @class table
 */

/* @luadoc
 * --- Appends an item to the tree.
 * --
 * -- @param self Tree.
 * -- @param args Arguments.<ul>
 * --   <li>text: Text.</li></ul>
 * function Tree.append(self, args)
 */
static void Tree_append (LIScrArgs* args)
{
	const char* text = "";
	LIWdgTreerow* row;

	liscr_args_gets_string (args, "text", &text);
	row = liwdg_tree_get_root (args->self);
	liwdg_treerow_append_row (row, text, NULL);
}

/* @luadoc
 * --- Gets a row from the tree.
 * --
 * -- @param self Tree.
 * -- @param args Arguments.<ul>
 * --   <li>row: Row index.</li></ul>
 * -- @return String or nil.
 * function Tree.get_row(self, args)
 */
static void Tree_get_row (LIScrArgs* args)
{
	int index = 1;
	int count;
	LIWdgTreerow* root;
	LIWdgTreerow* row;

	liscr_args_gets_int (args, "row", &index);
	root = liwdg_tree_get_root (args->self);
	count = liwdg_treerow_get_row_count (root);
	if (index >= 1 && index <= count)
	{
		row = liwdg_treerow_get_row (root, index - 1);
		liscr_args_seti_string (args, liwdg_treerow_get_text (row));
	}
}

/* @luadoc
 * --- Creates a new tree widget.
 * --
 * -- @param clss Tree class.
 * -- @param args Arguments.
 * -- @return New tree widget.
 * function Tree.new(clss, args)
 */
static void Tree_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_TREE);
	self = liwdg_tree_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_TREE, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_userdata (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_insert_callback (self, "pressed", private_callback_pressed_tree, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Removes a row from the tree.
 * --
 * -- @param self Tree.
 * -- @param args Arguments.<ul>
 * --   <li>row: Row index.</li></ul>
 * function Tree.remove(self, args)
 */
static void
Tree_remove (LIScrArgs* args)
{
	int index = 1;
	int count;
	LIWdgTreerow* root;

	liscr_args_gets_int (args, "row", &index);
	root = liwdg_tree_get_root (args->self);
	count = liwdg_treerow_get_row_count (root);
	if (index >= 1 && index <= count)
		liwdg_treerow_remove_row (root, index - 1);
}

/* @luadoc
 * --- Sets a row of the tree.
 * --
 * -- @param self Tree.
 * -- @param args Arguments.<ul>
 * --   <li>row: Row index.</li>
 * --   <li>text: String.</li></ul>
 * -- @return String or nil.
 * function Tree.get_row(self, args)
 */
static void Tree_set_row (LIScrArgs* args)
{
	int index = 1;
	int count;
	const char* text = "";
	LIWdgTreerow* root;
	LIWdgTreerow* row;

	liscr_args_gets_int (args, "row", &index);
	root = liwdg_tree_get_root (args->self);
	count = liwdg_treerow_get_row_count (root);
	if (index >= 1 && index <= count)
	{
		row = liwdg_treerow_get_row (root, index - 1);
		liscr_args_gets_string (args, "text", &text);
		liwdg_treerow_set_text (row, text);
	}
}

/* @luadoc
 * --- Customizable callback for row selection events.
 * -- @param self Tree widget.
 * function Tree.pressed(self)
 */

/* @luadoc
 * --- Index of the selected item or nil for no selection.
 * -- @name Tree.selection
 * -- @class table
 */
static void Tree_getter_selection (LIScrArgs* args)
{
	int i;
	int count;
	LIWdgTreerow* root;
	LIWdgTreerow* row;

	root = liwdg_tree_get_root (args->self);
	count = liwdg_treerow_get_row_count (root);
	for (i = 0 ; i < count ; i++)
	{
		row = liwdg_treerow_get_row (root, i);
		if (liwdg_treerow_get_highlighted (row))
		{
			liscr_args_seti_int (args, i + 1);
			break;
		}
	}
}
static void Tree_setter_selection (LIScrArgs* args)
{
	int i;
	int count;
	int index;
	LIWdgTreerow* root;
	LIWdgTreerow* row;

	root = liwdg_tree_get_root (args->self);
	count = liwdg_treerow_get_row_count (root);
	if (liscr_args_geti_int (args, 0, &index) && index >= 1 && index <= count)
		index--;
	else
		index = -1;

	/* Update row highlights. */
	for (i = 0 ; i < count ; i++)
	{
		row = liwdg_treerow_get_row (root, i);
		liwdg_treerow_set_highlighted (row, i == index);
	}
}

/* @luadoc
 * --- Number of root level items in the tree.
 * -- @name Tree.size
 * -- @class table
 */
static void Tree_getter_size (LIScrArgs* args)
{
	LIWdgTreerow* row;

	row = liwdg_tree_get_root (args->self);
	liscr_args_seti_int (args, liwdg_treerow_get_row_count (row));
}
static void Tree_setter_size (LIScrArgs* args)
{
	int oldsize;
	int newsize;
	LIWdgTreerow* row;

	row = liwdg_tree_get_root (args->self);
	if (liscr_args_geti_int (args, 0, &newsize))
	{
		oldsize = liwdg_treerow_get_row_count (row);
		while (oldsize < newsize)
		{
			if (!liwdg_treerow_append_row (row, "", NULL))
				return;
			oldsize++;
		}
		while (oldsize > newsize)
		{
			liwdg_treerow_remove_row (row, oldsize - 1);
			oldsize--;
		}
	}
}

/* @luadoc
 * --- Create a scrollable viewport.
 * -- @name View
 * -- @class table
 */

/* @luadoc
 * --- Creates a new view widget.
 * --
 * -- @param clss View class.
 * -- @param args Arguments.
 * -- @return New view widget.
 * function View.new(clss, args)
 */
static void
View_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate self. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_VIEW);
	self = liwdg_view_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LIEXT_SCRIPT_VIEW, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_set_userdata (self, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);	
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * --- Child widget.
 * -- @name View.child
 * -- @class table
 */
static void View_getter_child (LIScrArgs* args)
{
	LIWdgWidget* child;

	child = liwdg_view_get_child (args->self);
	if (child != NULL)
		liscr_args_seti_data (args, liwdg_widget_get_userdata (child));
}
static void View_setter_child (LIScrArgs* args)
{
	LIScrData* child = NULL;

	/* Argument checks. */
	if (liscr_args_geti_data (args, 0, LIEXT_SCRIPT_WIDGET, &child))
		liwdg_widget_detach (child->data);

	/* Attach new child. */
	if (child != NULL)
		liwdg_view_set_child (args->self, child->data);
	else
		liwdg_view_set_child (args->self, NULL);
}

/* @luadoc
 * --- Horizontal scrolling flag of the widget.
 * -- @name View.hscroll
 * -- @class table
 */
static void View_getter_hscroll (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_view_get_hscroll (args->self));
}
static void View_setter_hscroll (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_view_set_hscroll (args->self, value);
}

/* @luadoc
 * --- Vertical scrolling flag of the widget.
 * -- @name View.vscroll
 * -- @class table
 */
static void View_getter_vscroll (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_view_get_hscroll (args->self));
}
static void View_setter_vscroll (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_view_set_vscroll (args->self, value);
}

/*****************************************************************************/

/* @luadoc
 * --- Cycles widget focus.
 * --
 * -- @param clss Widgets class.
 * -- @param args Arguments.<ul>
 * --   <li>backward: True if should cycle backward.</li></ul>
 * function Widgets.cycle_focus(clss, args)
 */
static void Widgets_cycle_focus (LIScrArgs* args)
{
	int prev = 0;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_focus (module->widgets, !prev);
}

/* @luadoc
 * --- Cycles window focus.
 * --
 * -- @param clss Widgets class.
 * -- @param args Arguments.<ul>
 * --   <li>backward: True if should cycle backward.</li></ul>
 * function Widgets.cycle_focus(clss, args)
 */
static void Widgets_cycle_window_focus (LIScrArgs* args)
{
	int prev = 0;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_window_focus (module->widgets, !prev);
}

/* @luadoc
 * --- Draws the user interface.
 * -- @param clss Widgets class.
 * function Widgets.draw(clss)
 */
static void Widgets_draw (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	liwdg_manager_render (module->widgets);
}

/*****************************************************************************/

void
liext_script_button (LIScrClass* self,
                     void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_BUTTON, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", Button_new);
	liscr_class_insert_mvar (self, "text", Button_getter_text, Button_setter_text);
}

void
liext_script_entry (LIScrClass* self,
                    void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_ENTRY, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", Entry_new);
	liscr_class_insert_mfunc (self, "clear", Entry_clear);
	liscr_class_insert_mvar (self, "text", Entry_getter_text, Entry_setter_text);
}

void
liext_script_image (LIScrClass* self,
                    void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_IMAGE, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", Image_new);
	liscr_class_insert_mvar (self, "image", Image_getter_image, Image_setter_image);
}

void
liext_script_label (LIScrClass* self,
                    void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_LABEL, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", Label_new);
	liscr_class_insert_mvar (self, "text", Label_getter_text, Label_setter_text);
}

void
liext_script_menu (LIScrClass* self,
                   void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_MENU, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_mfunc (self, "get_item_rect", Menu_get_item_rect);
	liscr_class_insert_mfunc (self, "insert", Menu_insert);
	liscr_class_insert_cfunc (self, "new", Menu_new);
	liscr_class_insert_mvar (self, "autohide", Menu_getter_autohide, Menu_setter_autohide);
	liscr_class_insert_mvar (self, "orientation", Menu_getter_orientation, Menu_setter_orientation);
}

void
liext_script_scroll (LIScrClass* self,
                     void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_SCROLL, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", Scroll_new);
	liscr_class_insert_mfunc (self, "set_range", Scroll_set_range);
	liscr_class_insert_mvar (self, "reference", Scroll_getter_reference, Scroll_setter_reference);
	liscr_class_insert_mvar (self, "value", Scroll_getter_value, Scroll_setter_value);
}

void
liext_script_spin (LIScrClass* self,
                   void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_SPIN, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", Spin_new);
	liscr_class_insert_mvar (self, "value", Spin_getter_value, Spin_setter_value);
}

void
liext_script_tree (LIScrClass* self,
                   void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_TREE, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_mfunc (self, "append", Tree_append);
	liscr_class_insert_mfunc (self, "get_row", Tree_get_row);
	liscr_class_insert_cfunc (self, "new", Tree_new);
	liscr_class_insert_mfunc (self, "remove", Tree_remove);
	liscr_class_insert_mfunc (self, "set_row", Tree_set_row);
	liscr_class_insert_mvar (self, "size", Tree_getter_size, Tree_setter_size);
	liscr_class_insert_mvar (self, "selection", Tree_getter_selection, Tree_setter_selection);
}

void
liext_script_view (LIScrClass* self,
                   void*       data)
{
	LIExtModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_VIEW, data);
	liscr_class_inherit (self, liext_script_widget, module->client);
	liscr_class_insert_cfunc (self, "new", View_new);
	liscr_class_insert_mvar (self, "child", View_getter_child, View_setter_child);
	liscr_class_insert_mvar (self, "hscroll", View_getter_hscroll, View_setter_hscroll);
	liscr_class_insert_mvar (self, "vscroll", View_getter_vscroll, View_setter_vscroll);
}

void liext_script_widgets (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_WIDGETS, data);
	liscr_class_insert_cfunc (self, "cycle_focus", Widgets_cycle_focus);
	liscr_class_insert_cfunc (self, "cycle_window_focus", Widgets_cycle_window_focus);
	liscr_class_insert_cfunc (self, "draw", Widgets_draw);
}

/** @} */
/** @} */
