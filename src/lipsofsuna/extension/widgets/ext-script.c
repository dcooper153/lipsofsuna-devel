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

/* @luadoc
 * --- Adds a font style.
 * -- @param clss Widgets class.
 * -- @param args Arguments.<ul>
 * --   <li>1,name: Font name.</li>
 * --   <li>2,file: Font file.</li>
 * --   <li>3,size: Font size.</li></ul>
 * function Widgets.add_font_style(clss, args)
 */
static void Widgets_add_font_style (LIScrArgs* args)
{
	int size = 16;
	const char* file;
	const char* name;
	LIExtModule* module;

	/* Arguments. */
	if (!liscr_args_geti_string (args, 0, &name) &&
	    !liscr_args_gets_string (args, "name", &name))
		return;
	if (!liscr_args_geti_string (args, 1, &file) &&
	    !liscr_args_gets_string (args, "file", &file))
		return;
	if (!liscr_args_geti_int (args, 2, &size))
	    liscr_args_gets_int (args, "size", &size);

	/* Load the font. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	if (!liwdg_styles_load_font (module->widgets->styles, name, file, size))
		lisys_error_report ();
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

/* @luadoc
 * --- Currently focused widget.
 * -- @name Widgets.focused_widget
 * -- @class table
 */
static void Widgets_getter_focused_widget (LIScrArgs* args)
{
	LIExtModule* module;
	LIWdgWidget* widget;
	LIScrData* data;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	widget = liwdg_manager_find_widget_by_point (module->widgets,
		module->widgets->pointer.x, module->widgets->pointer.y);
	if (widget == NULL)
		return;
	data = liwdg_widget_get_script (widget);
	if (data == NULL)
		return;
	liscr_args_seti_data (args, data);
}

/*****************************************************************************/

void liext_script_widgets (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_WIDGETS, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "add_font_style", Widgets_add_font_style);
	liscr_class_insert_cfunc (self, "cycle_focus", Widgets_cycle_focus);
	liscr_class_insert_cfunc (self, "cycle_window_focus", Widgets_cycle_window_focus);
	liscr_class_insert_cfunc (self, "draw", Widgets_draw);
	liscr_class_insert_cvar (self, "focused_widget", Widgets_getter_focused_widget, NULL);
}

/** @} */
/** @} */
