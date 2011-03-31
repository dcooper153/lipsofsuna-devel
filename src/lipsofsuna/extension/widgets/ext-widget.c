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

static int private_callback_pressed (
	LIScrData* data)
{
	LIScrScript* script = liscr_data_get_script (data);
	lua_State* lua = liscr_script_get_lua (script);

	liscr_pushdata (lua, data);
	lua_getfield (lua, -1, "pressed");
	if (lua_type (lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (lua, -2);
		lua_remove (lua, -3);
		if (lua_pcall (lua, 1, 0, 0) != 0)
		{
			lisys_error_set (LISYS_ERROR_UNKNOWN, "Widget.pressed: %s", lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
		}
		return 0;
	}
	else
		lua_pop (lua, 2);
	return 1;
}

/*****************************************************************************/

/* @luadoc
 * module "core/widgets"
 * --- Manipulate widgets.
 * -- @name Widget
 * -- @class table
 */

/* @luadoc
 * --- Creates a new widget.
 * -- @param clss Widget class.
 * -- @param args Arguments.
 * -- @return New widget.
 * function Widget.new(clss, args)
 */
static void Widget_new (LIScrArgs* args)
{
	LIExtModule* module;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate userdata. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGET);
	self = liwdg_widget_new (module->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, args->clss, liwdg_widget_free);
	if (data == NULL)
	{
		liwdg_widget_free (self);
		return;
	}
	liwdg_widget_set_script (self, data);
	liwdg_widget_insert_callback (self, "paint", liext_widgets_callback_paint, data);
	liwdg_widget_insert_callback (self, "pressed", private_callback_pressed, data);
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
}

/* @luadoc
 * --- Appends a column to the widget.
 * -- @param self Widget.
 * -- @param args List of widgets.
 * function Widget.append_row(self, args)
 */
static void Widget_append_col (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	LIScrData* data;

	/* Append column. */
	liwdg_widget_get_size (args->self, &w, &h);
	if (!liwdg_widget_append_col (args->self))
		return;

	/* Append rows. */
	for (i = 0 ; i < h && liscr_args_geti_data (args, i, LIEXT_SCRIPT_WIDGET, &data) ; i++)
	{
		liwdg_widget_detach (liscr_data_get_data (data));
		liwdg_widget_set_child (args->self, w, i, liscr_data_get_data (data));
	}
}

/* @luadoc
 * --- Appends a row to the widget.
 * --
 * -- @param self Widget.
 * -- @param args List of widgets.
 * function Widget.append_row(self, args)
 */
static void Widget_append_row (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	LIScrData* data;

	/* Append row. */
	liwdg_widget_get_size (args->self, &w, &h);
	if (!liwdg_widget_append_row (args->self))
		return;

	/* Append columns. */
	for (i = 0 ; i < w && liscr_args_geti_data (args, i, LIEXT_SCRIPT_WIDGET, &data) ; i++)
	{
		liwdg_widget_detach (liscr_data_get_data (data));
		liwdg_widget_set_child (args->self, i, h, liscr_data_get_data (data));
	}
}

/* @luadoc
 * --- Clears the canvas of the widget.
 * -- @param self Widget.
 * function Widget.canvas_clear(self)
 */
static void Widget_canvas_clear (LIScrArgs* args)
{
	liwdg_widget_canvas_clear (args->self);
}

/* @luadoc
 * --- Compiles the canvas of the widget.
 * -- @param self Widget.
 * function Widget.canvas_compile(self)
 */
static void Widget_canvas_compile (LIScrArgs* args)
{
	liwdg_widget_canvas_compile (args->self);
}

/* @luadoc
 * --- Packs an image to the canvas of the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>color: RGBA color or nil.</li>
 * --   <li>dest_clip: {x,y,w,h} or nil.</li>
 * --   <li>dest_position: {x,y} or nil.</li>
 * --   <li>dest_size: {w,h} or nil.</li>
 * --   <li>rotation: Rotation angle in radians or nil.</li>
 * --   <li>rotation_center: Rotation center vector or nil.</li>
 * --   <li>source_position: {x,y} or nil.</li>
 * --   <li>source_tiling: {x1,x2,x3,y1,y2,y3} or nil.</li></ul>
 * function Widget.canvas_image(self, args)
 */
static void Widget_canvas_image (LIScrArgs* args)
{
	float color[4];
	int dest_clip[4];
	int dest_position[2];
	int dest_size[2];
	int source_position[2];
	int source_tiling[6];
	float* color_ptr = NULL;
	int* dest_position_ptr = NULL;
	int* dest_size_ptr = NULL;
	int* dest_clip_ptr = NULL;
	int* source_position_ptr = NULL;
	int* source_tiling_ptr = NULL;
	const char* source_image;
	float rotation = 0.0f;
	LIScrData* vector = NULL;
	LIMatVector* rotation_center_ptr = NULL;
	LIRenImage* image;
	LIWdgWidget* widget;
	LIWdgElement* elem;

	/* Process arguments. */
	widget = args->self;
	if (!liscr_args_gets_string (args, "source_image", &source_image))
		return;
	image = liwdg_manager_find_image (widget->manager, source_image);
	if (image == NULL)
		return;
	if (liscr_args_gets_floatv (args, "color", 4, color) == 4)
		color_ptr = color;
	if (liscr_args_gets_intv (args, "dest_clip", 4, dest_clip) == 4)
		dest_clip_ptr = dest_clip;
	if (liscr_args_gets_intv (args, "dest_position", 2, dest_position) == 2)
		dest_position_ptr = dest_position;
	if (liscr_args_gets_intv (args, "dest_size", 2, dest_size) == 2)
		dest_size_ptr = dest_size;
	if (liscr_args_gets_intv (args, "source_position", 2, source_position) == 2)
		source_position_ptr = source_position;
	if (liscr_args_gets_intv (args, "source_tiling", 6, source_tiling) == 6)
		source_tiling_ptr = source_tiling;
	liscr_args_gets_float (args, "rotation", &rotation);
	if (liscr_args_gets_data (args, "rotation_center", LISCR_SCRIPT_VECTOR, &vector))
		rotation_center_ptr = liscr_data_get_data (vector);

	/* Create the canvas element. */
	elem = liwdg_element_new_image (image, color_ptr, dest_clip_ptr, dest_position_ptr,
		dest_size_ptr, source_position_ptr, source_tiling_ptr, rotation, rotation_center_ptr);
	if (elem == NULL)
		return;
	if (!liwdg_widget_canvas_insert (widget, elem))
	{
		liwdg_element_free (elem);
		return;
	}
}

/* @luadoc
 * --- Packs text to the canvas of the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>dest_clip: {x,y,w,h} or nil.</li>
 * --   <li>dest_position: {x,y} or nil.</li>
 * --   <li>dest_size: {w,h} or nil.</li>
 * --   <li>rotation: Rotation angle in radians or nil.</li>
 * --   <li>rotation_center: Rotation center vector or nil.</li>
 * --   <li>text: String.</li></ul>
 * --   <li>text_alignment: {x,y} or nil.</li>
 * --   <li>text_color: {r,g,b,a} or nil.</li></ul>
 * --   <li>text_font: Font name or nil.</li></ul>
 * function Widget.canvas_text(self, args)
 */
static void Widget_canvas_text (LIScrArgs* args)
{
	int dest_clip[4];
	int dest_position[2];
	int dest_size[2];
	float text_align[2];
	float text_color[6];
	int* dest_position_ptr = NULL;
	int* dest_size_ptr = NULL;
	int* dest_clip_ptr = NULL;
	float* text_align_ptr = NULL;
	float* text_color_ptr = NULL;
	const char* text;
	const char* text_font = "default";
	float rotation = 0.0f;
	LIScrData* vector = NULL;
	LIMatVector* rotation_center_ptr = NULL;
	LIFntFont* font;
	LIWdgWidget* widget;
	LIWdgElement* elem;

	/* Process arguments. */
	widget = args->self;
	if (!liscr_args_gets_string (args, "text", &text))
		return;
	liscr_args_gets_string (args, "text_font", &text_font);
	font = lialg_strdic_find (widget->manager->styles->fonts, text_font);
	if (font == NULL)
		return;
	if (liscr_args_gets_intv (args, "dest_clip", 4, dest_clip) == 4)
		dest_clip_ptr = dest_clip;
	if (liscr_args_gets_intv (args, "dest_position", 2, dest_position) == 2)
		dest_position_ptr = dest_position;
	if (liscr_args_gets_intv (args, "dest_size", 2, dest_size) == 2)
		dest_size_ptr = dest_size;
	if (liscr_args_gets_floatv (args, "text_alignment", 2, text_align) == 2)
		text_align_ptr = text_align;
	if (liscr_args_gets_floatv (args, "text_color", 4, text_color) == 4)
		text_color_ptr = text_color;
	liscr_args_gets_float (args, "rotation", &rotation);
	if (liscr_args_gets_data (args, "rotation_center", LISCR_SCRIPT_VECTOR, &vector))
		rotation_center_ptr = liscr_data_get_data (vector);

	/* Create the canvas element. */
	elem = liwdg_element_new_text (font, text, dest_clip_ptr, dest_position_ptr,
		dest_size_ptr, text_align_ptr, text_color_ptr, rotation, rotation_center_ptr);
	if (elem == NULL)
		return;
	if (!liwdg_widget_canvas_insert (widget, elem))
	{
		liwdg_element_free (elem);
		return;
	}
}

/* @luadoc
 * --- Gets a child widget.
 * --
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>row: Row number.</li>
 * --   <li>col: Column number.</li></ul>
 * -- @return Widget or nil.
 * function Widget.get_child(self, args)
 */
static void Widget_get_child (LIScrArgs* args)
{
	int x = 1;
	int y = 1;
	int w;
	int h;
	LIWdgWidget* widget;

	liwdg_widget_get_size (args->self, &w, &h);
	liscr_args_gets_int (args, "col", &x);
	liscr_args_gets_int (args, "row", &y);
	if (x <= 0 || x > w || y <= 0 || y > h)
		return;	
	widget = liwdg_widget_get_child (args->self, x - 1, y - 1);
	if (widget != NULL)
		liscr_args_seti_data (args, liwdg_widget_get_script (widget));
}

/* @luadoc
 * --- Gets the size request of the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>internal: True to get the internal request.</li></ul>
 * function Widget.get_request(self, args)
 */
static void Widget_get_request (LIScrArgs* args)
{
	int internal = 0;
	LIWdgWidget* widget;

	widget = args->self;
	liscr_args_gets_bool (args, "internal", &internal);
	if (internal)
	{
		liscr_args_seti_int (args, widget->request[1].width);
		liscr_args_seti_int (args, widget->request[1].height);
	}
	else
	{
		if (widget->request[2].width >= 0)
			liscr_args_seti_int (args, widget->request[2].width);
		else
			liscr_args_seti_nil (args);
		if (widget->request[2].height >= 0)
			liscr_args_seti_int (args, widget->request[2].height);
	}
}

/* @luadoc
 * --- Inserts a column to the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>1,col: Column index or nil.</li>
 * --   <li>2,widget: Widget or nil.</li></ul>
 * function Widget.insert_col(self, args)
 */
static void Widget_insert_col (LIScrArgs* args)
{
	int w;
	int h;
	int col = 0;
	LIScrData* widget = NULL;

	if (!liscr_args_geti_int (args, 0, &col))
		liscr_args_gets_int (args, "col", &col);
	if (!liscr_args_geti_data (args, 1, LIEXT_SCRIPT_WIDGET, &widget))
		liscr_args_gets_data (args, "widget", LIEXT_SCRIPT_WIDGET, &widget);
	col--;
	liwdg_widget_get_size (args->self, &w, &h);
	if (col < 0) col = 0;
	if (col > w) col = w;

	/* Insert row. */
	if (!liwdg_widget_insert_col (args->self, col))
		return;

	/* Set widget. */
	if (h && widget != NULL)
	{
		liwdg_widget_detach (liscr_data_get_data (widget));
		liwdg_widget_set_child (args->self, col, 0, liscr_data_get_data (widget));
	}
}

/* @luadoc
 * --- Inserts a row to the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>1,row: Row index or nil.</li>
 * --   <li>2,widget: Widget or nil.</li></ul>
 * function Widget.insert_row(self, args)
 */
static void Widget_insert_row (LIScrArgs* args)
{
	int w;
	int h;
	int row = 0;
	LIScrData* widget = NULL;

	if (!liscr_args_geti_int (args, 0, &row))
		liscr_args_gets_int (args, "row", &row);
	if (!liscr_args_geti_data (args, 1, LIEXT_SCRIPT_WIDGET, &widget))
		liscr_args_gets_data (args, "widget", LIEXT_SCRIPT_WIDGET, &widget);
	row--;
	liwdg_widget_get_size (args->self, &w, &h);
	if (row < 0) row = 0;
	if (row > w) row = w;

	/* Insert row. */
	if (!liwdg_widget_insert_row (args->self, row))
		return;

	/* Set widget. */
	if (w && widget != NULL)
	{
		liwdg_widget_detach (liscr_data_get_data (widget));
		liwdg_widget_set_child (args->self, 0, row, liscr_data_get_data (widget));
	}
}

/* @luadoc
 * --- Pops up the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>x: X coordinate.</li>
 * --   <li>y: Y coordinate.</li>
 * --   <li>width: Width allocation.</li>
 * --   <li>height: Height allocation.</li>
 * --   <li>dir: Popup direction. ("left"/"right"/"up"/"down")</li></ul>
 * function Widget.popup(self, args)
 */
static void Widget_popup (LIScrArgs* args)
{
	const char* dir;
	LIWdgRect rect;
	LIWdgSize screen;
	LIWdgSize size;
	LIWdgWidget* widget;

	widget = args->self;
	liwdg_widget_detach (widget);

	/* Calculate position. */
	liwdg_manager_get_size (widget->manager, &screen.width, &screen.height);
	liwdg_widget_get_request (widget, &size);
	rect.x = (screen.width - size.width) / 2;
	rect.y = (screen.height - size.height) / 2;
	rect.width = size.width;
	rect.height = size.height;
	liscr_args_gets_int (args, "x", &rect.x);
	liscr_args_gets_int (args, "y", &rect.y);
	liscr_args_gets_int (args, "width", &rect.width);
	liscr_args_gets_int (args, "height", &rect.height);
	if (liscr_args_gets_string (args, "dir", &dir))
	{
		if (!strcmp (dir, "left")) rect.x -= size.width;
		else if (!strcmp (dir, "right")) rect.x += rect.width;
		else if (!strcmp (dir, "up")) rect.y -= size.height;
		else if (!strcmp (dir, "down")) rect.y += rect.height;
	}

	/* Popup the widget. */
	liwdg_widget_set_visible (widget, 1);
	liwdg_manager_insert_window (widget->manager, widget);
	liwdg_widget_set_allocation (widget, rect.x, rect.y, rect.width, rect.height);
	liscr_data_ref (args->data);
}

/* @luadoc
 * --- Removes a row or a column from the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>col: Column index.</li>
 * --   <li>row: Row index.</li></ul>
 * function Widget.remove(self, args)
 */
static void Widget_remove (LIScrArgs* args)
{
	int w;
	int h;
	int col;
	int row;

	liwdg_widget_get_size (args->self, &w, &h);
	if (liscr_args_gets_int (args, "col", &col) && col >= 1 && col <= w)
		liwdg_widget_remove_col (args->self, col - 1);
	if (liscr_args_gets_int (args, "row", &row) && row >= 1 && row <= h)
		liwdg_widget_remove_row (args->self, row - 1);
}

/* @luadoc
 * --- Places a widget inside the group.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>col: Column number. (required)</li>
 * --   <li>row: Row number. (required)</li>
 * --   <li>widget: Widget.</li></ul>
 * function Widget.set_child(self, args)
 */
static void Widget_set_child (LIScrArgs* args)
{
	int x = 1;
	int y = 1;
	int w;
	int h;
	LIScrData* data = NULL;

	/* Arguments. */
	if (!liscr_args_gets_int (args, "col", &x) ||
	    !liscr_args_gets_int (args, "row", &y))
		return;
	liscr_args_gets_data (args, "widget", LIEXT_SCRIPT_WIDGET, &data);
	liwdg_widget_get_size (args->self, &w, &h);
	if (x < 1 || x > w || y < 1 || y > h)
		return;

	/* Attach new widget. */
	if (data != NULL)
	{
		liwdg_widget_detach (liscr_data_get_data (data));
		liwdg_widget_set_child (args->self, x - 1, y - 1, liscr_data_get_data (data));
	}
	else
		liwdg_widget_set_child (args->self, x - 1, y - 1, NULL);
}

/* @luadoc
 * --- Enables or disables row or column expansion.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>row: Row number.</li>
 * --   <li>col: Column number.</li>
 * --   <li>expand: Boolean.</li></ul>
 * function Widget.set_expand(self, args)
 */
static void Widget_set_expand (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	int expand = 1;

	liwdg_widget_get_size (args->self, &w, &h);
	liscr_args_gets_bool (args, "expand", &expand);
	if (liscr_args_gets_int (args, "col", &i) && i >= 1 && i <= w)
		liwdg_widget_set_col_expand (args->self, i - 1, expand);
	if (liscr_args_gets_int (args, "row", &i) && i >= 1 && i <= h)
		liwdg_widget_set_row_expand (args->self, i - 1, expand);
}

/* @luadoc
 * --- Sets the size request of the widget.
 * -- @param self Widget.
 * -- @param args Arguments.<ul>
 * --   <li>font: Font to use when calculating from text.</li>
 * --   <li>internal: Set the internal request instead of the user request.</li>
 * --   <li>width: Width request.</li>
 * --   <li>height: Height request.</li>
 * --   <li>paddings: Additional paddings to add or nil.</li>
 * --   <li>text: Text to use when calculating from text.</li></ul>
 * function Widget.set_request(self, args)
 */
static void Widget_set_request (LIScrArgs* args)
{
	int internal = 0;
	int paddings[4];
	const char* text;
	const char* font_name;
	LIFntFont* font;
	LIFntLayout* layout;
	LIWdgSize size = { -1, -1 };
	LIWdgWidget* widget;

	widget = args->self;
	liscr_args_gets_bool (args, "internal", &internal);
	liscr_args_gets_int (args, "width", &size.width);
	liscr_args_gets_int (args, "height", &size.height);

	/* Calculate from text if given. */
	if (liscr_args_gets_string (args, "font", &font_name) &&
	    liscr_args_gets_string (args, "text", &text))
	{
		font = lialg_strdic_find (widget->manager->styles->fonts, font_name);
		if (font != NULL)
		{
			layout = lifnt_layout_new ();
			if (layout != NULL)
			{
				if (size.width != -1)
					lifnt_layout_set_width_limit (layout, size.width);
				lifnt_layout_append_string (layout, font, text);
				if (size.width == -1)
					size.width = lifnt_layout_get_width (layout);
				if (size.height == -1)
					size.height = lifnt_layout_get_height (layout);
				lifnt_layout_free (layout);
			}
		}
	}

	/* Add paddings if given. */
	if (liscr_args_gets_intv (args, "paddings", 4, paddings) == 4)
	{
		size.width += paddings[1] + paddings[2];
		size.height += paddings[0] + paddings[3];
	}

	/* Set the request. */
	if (internal)
		liwdg_widget_set_request (args->self, 1, size.width, size.height);
	else
		liwdg_widget_set_request (args->self, 2, size.width, size.height);
}

/* @luadoc
 * --- Behind flag.
 * -- @name Widget.behind
 * -- @class table
 */
static void Widget_get_behind (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_behind (args->self));
}
static void Widget_set_behind (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_behind (args->self, value);
}

/* @luadoc
 * --- Number of columns in the widget.
 * -- @name Widget.cols
 * -- @class table
 */
static void Widget_get_cols (LIScrArgs* args)
{
	int w;
	int h;

	liwdg_widget_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, w);
}
static void Widget_set_cols (LIScrArgs* args)
{
	int w;
	int h;
	int cols;

	liwdg_widget_get_size (args->self, &w, &h);
	if (liscr_args_geti_int (args, 0, &cols) && cols >= 0)
		liwdg_widget_set_size (args->self, cols, h);
}

/* @luadoc
 * --- Floating flag.
 * -- @name Widget.floating
 * -- @class table
 */
static void Widget_get_floating (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_floating (args->self));
}
static void Widget_set_floating (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_floating (args->self, value);
}

/* @luadoc
 * --- Fullscreen flag.
 * -- @name Widget.fullscreen
 * -- @class table
 */
static void Widget_get_fullscreen (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_fullscreen (args->self));
}
static void Widget_set_fullscreen (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_fullscreen (args->self, value);
}

/* @luadoc
 * --- Height of the widget.
 * -- @name Widget.height
 * -- @class table
 */
static void Widget_get_height (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.height);
}

/* @luadoc
 * --- Margin widths.
 * -- @name Widget.margins
 * -- @class table
 */
static void Widget_get_margins (LIScrArgs* args)
{
	int v[4];

	liwdg_widget_get_margins (args->self, v + 0, v + 1, v + 2, v + 3);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_float (args, "left", v[0]);
	liscr_args_sets_float (args, "right", v[1]);
	liscr_args_sets_float (args, "top", v[2]);
	liscr_args_sets_float (args, "bottom", v[3]);
}
static void Widget_set_margins (LIScrArgs* args)
{
	int v[4];

	liwdg_widget_get_margins (args->self, v + 0, v + 1, v + 2, v + 3);
	liscr_args_geti_int (args, 0, v + 0);
	liscr_args_geti_int (args, 1, v + 1);
	liscr_args_geti_int (args, 2, v + 2);
	liscr_args_geti_int (args, 3, v + 3);
	liscr_args_gets_int (args, "left", v + 0);
	liscr_args_gets_int (args, "right", v + 1);
	liscr_args_gets_int (args, "top", v + 2);
	liscr_args_gets_int (args, "bottom", v + 3);
	liwdg_widget_set_margins (args->self, v[0], v[1], v[2], v[3]);
}

/* @luadoc
 * --- The parent of this widget.
 * -- @name Widget.parent
 * -- @class table
 */
static void Widget_get_parent (LIScrArgs* args)
{
	LIWdgWidget* self = args->self;

	if (self->parent != NULL)
		liscr_args_seti_data (args, self->parent->script);
}

/* @luadoc
 * --- Number of rows in the widget.
 * -- @name Widget.rows
 * -- @class table
 */
static void Widget_get_rows (LIScrArgs* args)
{
	int w;
	int h;

	liwdg_widget_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, h);
}
static void Widget_set_rows (LIScrArgs* args)
{
	int w;
	int h;
	int rows;

	liwdg_widget_get_size (args->self, &w, &h);
	if (liscr_args_geti_int (args, 0, &rows) || rows >= 0)
		liwdg_widget_set_size (args->self, w, rows);
}

/* @luadoc
 * --- Child spacings.
 * -- @name Widget.margins
 * -- @class table
 */
static void Widget_get_spacings (LIScrArgs* args)
{
	int v[2];

	liwdg_widget_get_spacings (args->self, v + 0, v + 1);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_int (args, "horz", v[0]);
	liscr_args_sets_int (args, "vert", v[1]);
}
static void Widget_set_spacings (LIScrArgs* args)
{
	int v[4];

	liwdg_widget_get_spacings (args->self, v + 0, v + 1);
	liscr_args_geti_int (args, 0, v + 0);
	liscr_args_geti_int (args, 1, v + 1);
	liscr_args_gets_int (args, "horz", v + 0);
	liscr_args_gets_int (args, "vert", v + 1);
	liwdg_widget_set_spacings (args->self, v[0], v[1]);
}

/* @luadoc
 * --- Visibility flag.
 * -- @name Widget.visible
 * -- @class table
 */
static void Widget_get_visible (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_visible (args->self));
}
static void Widget_set_visible (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liwdg_widget_set_visible (args->self, value);
}

/* @luadoc
 * --- Width of the widget
 * -- @name Widget.width
 * -- @class table
 */
static void Widget_get_width (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.width);
}

/* @luadoc
 * --- Left edge position.
 * -- @name Widget.x
 * -- @class table
 */
static void Widget_get_x (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.x);
}
static void Widget_set_x (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_geti_int (args, 0, &rect.x);
	liwdg_widget_set_allocation (args->self, rect.x, rect.y, rect.width, rect.height);
}

/* @luadoc
 * --- Top edge position.
 * -- @name Widget.y
 * -- @class table
 */
static void Widget_get_y (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.y);
}
static void Widget_set_y (LIScrArgs* args)
{
	LIWdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_geti_int (args, 0, &rect.y);
	liwdg_widget_set_allocation (args->self, rect.x, rect.y, rect.width, rect.height);
}

/*****************************************************************************/

void liext_script_widget (
	LIScrClass* self,
	void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_WIDGET, data);
	liscr_class_insert_cfunc (self, "new", Widget_new);
	liscr_class_insert_mfunc (self, "append_col", Widget_append_col);
	liscr_class_insert_mfunc (self, "append_row", Widget_append_row);
	liscr_class_insert_mfunc (self, "canvas_clear", Widget_canvas_clear);
	liscr_class_insert_mfunc (self, "canvas_compile", Widget_canvas_compile);
	liscr_class_insert_mfunc (self, "canvas_image", Widget_canvas_image);
	liscr_class_insert_mfunc (self, "canvas_text", Widget_canvas_text);
	liscr_class_insert_mfunc (self, "get_child", Widget_get_child);
	liscr_class_insert_mfunc (self, "get_request", Widget_get_request);
	liscr_class_insert_mfunc (self, "insert_col", Widget_insert_col);
	liscr_class_insert_mfunc (self, "insert_row", Widget_insert_row);
	liscr_class_insert_mfunc (self, "popup", Widget_popup);
	liscr_class_insert_mfunc (self, "remove", Widget_remove);
	liscr_class_insert_mfunc (self, "set_child", Widget_set_child);
	liscr_class_insert_mfunc (self, "set_expand", Widget_set_expand);
	liscr_class_insert_mfunc (self, "set_request", Widget_set_request);
	liscr_class_insert_mfunc (self, "get_behind", Widget_get_behind);
	liscr_class_insert_mfunc (self, "set_behind", Widget_set_behind);
	liscr_class_insert_mfunc (self, "get_cols", Widget_get_cols);
	liscr_class_insert_mfunc (self, "set_cols", Widget_set_cols);
	liscr_class_insert_mfunc (self, "get_floating", Widget_get_floating);
	liscr_class_insert_mfunc (self, "set_floating", Widget_set_floating);
	liscr_class_insert_mfunc (self, "get_fullscreen", Widget_get_fullscreen);
	liscr_class_insert_mfunc (self, "set_fullscreen", Widget_set_fullscreen);
	liscr_class_insert_mfunc (self, "get_height", Widget_get_height);
	liscr_class_insert_mfunc (self, "get_margins", Widget_get_margins);
	liscr_class_insert_mfunc (self, "set_margins", Widget_set_margins);
	liscr_class_insert_mfunc (self, "get_parent", Widget_get_parent);
	liscr_class_insert_mfunc (self, "get_rows", Widget_get_rows);
	liscr_class_insert_mfunc (self, "set_rows", Widget_set_rows);
	liscr_class_insert_mfunc (self, "get_spacings", Widget_get_spacings);
	liscr_class_insert_mfunc (self, "set_spacings", Widget_set_spacings);
	liscr_class_insert_mfunc (self, "get_visible", Widget_get_visible);
	liscr_class_insert_mfunc (self, "set_visible", Widget_set_visible);
	liscr_class_insert_mfunc (self, "get_width", Widget_get_width);
	liscr_class_insert_mfunc (self, "get_x", Widget_get_x);
	liscr_class_insert_mfunc (self, "set_x", Widget_set_x);
	liscr_class_insert_mfunc (self, "get_y", Widget_get_y);
	liscr_class_insert_mfunc (self, "set_y", Widget_set_y);
}

/** @} */
/** @} */
