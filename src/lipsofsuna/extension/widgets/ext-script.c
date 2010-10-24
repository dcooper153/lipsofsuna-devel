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

/*****************************************************************************/

/* @luadoc
 * module "core/widgets"
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
	data = liscr_data_new (args->script, self, args->clss, liwdg_widget_free);
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
	data = liscr_data_new (args->script, self, args->clss, liwdg_widget_free);
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
	data = liscr_data_new (args->script, self, args->clss, liwdg_widget_free);
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
	data = liscr_data_new (args->script, self, args->clss, liwdg_widget_free);
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
	data = liscr_data_new (args->script, self, args->clss, liwdg_widget_free);
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
 * --- Adds a font style.
 * -- @param clss Widgets class.
 * -- @param args Arguments.<ul>
 * --   <li>1,name: Name of the style.</li>
 * --   <li>2,string: Style string.</li></ul>
 * function Widgets.add_font_style(clss, args)
 */
static void Widgets_add_font_style (LIScrArgs* args)
{
	const char* name;
	const char* string;
	LIArcReader* reader;
	LIExtModule* module;

	/* Arguments. */
	if (!liscr_args_geti_string (args, 0, &name) &&
	    !liscr_args_gets_string (args, "name", &name))
		return;
	if (!liscr_args_geti_string (args, 1, &string) &&
	    !liscr_args_gets_string (args, "string", &string))
		return;

	/* Parse the string. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	reader = liarc_reader_new (string, strlen (string));
	if (reader == NULL)
		return;
	if (!liwdg_styles_add_font (module->widgets->styles, name, reader))
		lisys_error_report ();
	liarc_reader_free (reader);
}

/* @luadoc
 * --- Adds a font style.
 * -- @param clss Widgets class.
 * -- @param args Arguments.<ul>
 * --   <li>1,name: Name of the style.</li>
 * --   <li>2,string: Style string.</li></ul>
 * function Widgets.add_font_style(clss, args)
 */
static void Widgets_add_widget_style (LIScrArgs* args)
{
	const char* name;
	const char* string;
	LIArcReader* reader;
	LIExtModule* module;

	/* Arguments. */
	if (!liscr_args_geti_string (args, 0, &name) &&
	    !liscr_args_gets_string (args, "name", &name))
		return;
	if (!liscr_args_geti_string (args, 1, &string) &&
	    !liscr_args_gets_string (args, "string", &string))
		return;

	/* Parse the string. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	reader = liarc_reader_new (string, strlen (string));
	if (reader == NULL)
		return;
	if (!liwdg_styles_add_widget (module->widgets->styles, name, reader))
		lisys_error_report ();
	liarc_reader_free (reader);
}

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
	liscr_class_set_userdata (self, LIEXT_SCRIPT_BUTTON, data);
	liscr_class_inherit (self, LIEXT_SCRIPT_WIDGET);
	liscr_class_insert_cfunc (self, "new", Button_new);
	liscr_class_insert_mvar (self, "text", Button_getter_text, Button_setter_text);
}

void
liext_script_entry (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_ENTRY, data);
	liscr_class_inherit (self, LIEXT_SCRIPT_WIDGET);
	liscr_class_insert_cfunc (self, "new", Entry_new);
	liscr_class_insert_mfunc (self, "clear", Entry_clear);
	liscr_class_insert_mvar (self, "text", Entry_getter_text, Entry_setter_text);
}

void
liext_script_image (LIScrClass* self,
                    void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_IMAGE, data);
	liscr_class_inherit (self, LIEXT_SCRIPT_WIDGET);
	liscr_class_insert_cfunc (self, "new", Image_new);
	liscr_class_insert_mvar (self, "image", Image_getter_image, Image_setter_image);
}

void
liext_script_scroll (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SCROLL, data);
	liscr_class_inherit (self, LIEXT_SCRIPT_WIDGET);
	liscr_class_insert_cfunc (self, "new", Scroll_new);
	liscr_class_insert_mfunc (self, "set_range", Scroll_set_range);
	liscr_class_insert_mvar (self, "reference", Scroll_getter_reference, Scroll_setter_reference);
	liscr_class_insert_mvar (self, "value", Scroll_getter_value, Scroll_setter_value);
}

void
liext_script_view (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VIEW, data);
	liscr_class_inherit (self, LIEXT_SCRIPT_WIDGET);
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
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "add_font_style", Widgets_add_font_style);
	liscr_class_insert_cfunc (self, "add_widget_style", Widgets_add_widget_style);
	liscr_class_insert_cfunc (self, "cycle_focus", Widgets_cycle_focus);
	liscr_class_insert_cfunc (self, "cycle_window_focus", Widgets_cycle_window_focus);
	liscr_class_insert_cfunc (self, "draw", Widgets_draw);
}

/** @} */
/** @} */
