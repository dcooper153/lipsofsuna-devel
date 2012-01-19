/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#include "lipsofsuna/extension/input/input.h"
#include "ext-module.h"

static void private_get_pointer (
	LIExtModule* self,
	int*         x,
	int*         y)
{
	LIInpInput* input;

	*x = *y = 0;
	input = limai_program_find_component (self->program, "input");
	if (input != NULL)
		liinp_input_get_pointer (input, x, y);
}

/*****************************************************************************/

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
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_WIDGETS);
	if (!liren_render_load_font (module->widgets->render, name, file, size))
		lisys_error_report ();
}

static void Widgets_draw (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_WIDGETS);
	liwdg_manager_render (module->widgets);
}

static void Widgets_find_widget (LIScrArgs* args)
{
	int x;
	int y;
	LIExtModule* module;
	LIMatVector vector;
	LIScrData* data;
	LIWdgWidget* widget;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_WIDGETS);
	if (!liscr_args_gets_vector (args, "point", &vector) &&
	    !liscr_args_geti_vector (args, 0, &vector))
	{
		private_get_pointer (module, &x, &y);
		vector = limat_vector_init (x, y, 0.0f);
	}

	widget = liwdg_manager_find_widget_by_point (module->widgets, (int) vector.x, (int) vector.y);
	if (widget == NULL)
		return;
	data = liwdg_widget_get_script (widget);
	if (data == NULL)
		return;
	liscr_args_seti_data (args, data);
}

static void Widgets_get_focused_widget (LIScrArgs* args)
{
	int x;
	int y;
	LIExtModule* module;
	LIWdgWidget* widget;
	LIScrData* data;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_WIDGETS);
	private_get_pointer (module, &x, &y);
	widget = liwdg_manager_find_widget_by_point (module->widgets, x, y);
	if (widget == NULL)
		return;
	data = liwdg_widget_get_script (widget);
	if (data == NULL)
		return;
	liscr_args_seti_data (args, data);
}

/*****************************************************************************/

void liext_script_widgets (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WIDGETS, "widgets_add_font_style", Widgets_add_font_style);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WIDGETS, "widgets_draw", Widgets_draw);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WIDGETS, "widgets_find_widget", Widgets_find_widget);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WIDGETS, "widgets_get_focused_widget", Widgets_get_focused_widget);
}

/** @} */
/** @} */
