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

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Group"
 * --- Pack widgets in a grid.
 * -- @name Group
 * -- @class table
 */

/* @luadoc
 * --- Appends a column to the widget.
 * --
 * -- @param self Group.
 * -- @param args List of widgets.
 * function Group.append_row(self, args)
 */
static void Group_append_col (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	LIScrData* data;

	/* Append column. */
	liwdg_group_get_size (args->self, &w, &h);
	if (!liwdg_group_append_col (args->self))
		return;

	/* Append rows. */
	for (i = 0 ; i < h && liscr_args_geti_data (args, i, LICLI_SCRIPT_WIDGET, &data) ; i++)
	{
		liwdg_widget_detach (data->data);
		liwdg_group_set_child (args->self, w, i, data->data);
	}
}

/* @luadoc
 * --- Appends a row to the widget.
 * --
 * -- @param self Group.
 * -- @param args List of widgets.
 * function Group.append_row(self, args)
 */
static void Group_append_row (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	LIScrData* data;

	/* Append row. */
	liwdg_group_get_size (args->self, &w, &h);
	if (!liwdg_group_append_row (args->self))
		return;

	/* Append columns. */
	for (i = 0 ; i < w && liscr_args_geti_data (args, i, LICLI_SCRIPT_WIDGET, &data) ; i++)
	{
		liwdg_widget_detach (data->data);
		liwdg_group_set_child (args->self, i, h, data->data);
	}
}

/* @luadoc
 * --- Inserts a column to the widget.
 * --
 * -- @param self Group.
 * -- @param col Column index.
 * -- @param widget Optional widget.
 * function Group.insert_col(self, col, widget)
 */
static int
Group_insert_col (lua_State* lua)
{
	int w;
	int h;
	int col;
	LIScrData* self;
	LIScrData* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	col = luaL_checkint (lua, 2) - 1;
	if (!lua_isnoneornil (lua, 3))
		widget = liscr_checkdata (lua, 3, LICLI_SCRIPT_WIDGET);
	else
		widget = NULL;
	liwdg_group_get_size (self->data, &w, &h);
	luaL_argcheck (lua, col >= 0 && col <= w, 2, "invalid column");

	/* Insert row. */
	if (!liwdg_group_insert_col (self->data, col))
		return 0;

	/* Set widget. */
	if (widget != NULL)
	{
		lua_getfield (lua, 1, "set_child");
		lua_pushvalue (lua, 1);
		lua_pushnumber (lua, col + 1);
		lua_pushnumber (lua, 1);
		lua_pushvalue (lua, 3);
		if (lua_pcall (lua, 4, 0, 0) != 0)
		{
			lisys_error_set (LISYS_ERROR_UNKNOWN, "%s", lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
			return 0;
		}
	}

	return 0;
}

/* @luadoc
 * --- Inserts a row to the widget.
 * --
 * -- @param self Group.
 * -- @param row Row index.
 * -- @param widget Optional widget.
 * function Group.insert_row(self, row, widget)
 */
static int
Group_insert_row (lua_State* lua)
{
	int w;
	int h;
	int row;
	LIScrData* self;
	LIScrData* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	row = luaL_checkint (lua, 2) - 1;
	if (!lua_isnoneornil (lua, 3))
		widget = liscr_checkdata (lua, 3, LICLI_SCRIPT_WIDGET);
	else
		widget = NULL;
	liwdg_group_get_size (self->data, &w, &h);
	luaL_argcheck (lua, row >= 0 && row <= h, 2, "invalid row");

	/* Insert row. */
	if (!liwdg_group_insert_row (self->data, row))
		return 0;

	/* Set widget. */
	if (widget != NULL)
	{
		lua_getfield (lua, 1, "set_child");
		lua_pushvalue (lua, 1);
		lua_pushnumber (lua, 1);
		lua_pushnumber (lua, row + 1);
		lua_pushvalue (lua, 3);
		if (lua_pcall (lua, 4, 0, 0) != 0)
		{
			lisys_error_set (LISYS_ERROR_UNKNOWN, "%s", lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
			return 0;
		}
	}

	return 0;
}

/* @luadoc
 * --- Creates a new group.
 * --
 * -- @param clss Group class.
 * -- @param args Arguments.
 * -- @return New group.
 * function Group.new(clss, args)
 */
static void Group_new (LIScrArgs* args)
{
	LICliClient* client;
	LIScrData* data;
	LIWdgWidget* self;

	/* Allocate userdata. */
	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_GROUP);
	self = liwdg_group_new (client->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LICLI_SCRIPT_GROUP, liwdg_widget_free);
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
 * --- Removes a row or a column from the widget.
 * --
 * -- @param self Group.
 * -- @param args Arguments.<ul>
 * --   <li>col: Column index.</li>
 * --   <li>row: Row index.</li></ul>
 * function Group.remove(self, args)
 */
static void Group_remove (LIScrArgs* args)
{
	int w;
	int h;
	int col;
	int row;

	liwdg_group_get_size (args->self, &w, &h);
	if (liscr_args_gets_int (args, "col", &col) && col >= 1 && col <= w)
		liwdg_group_remove_col (args->self, col - 1);
	if (liscr_args_gets_int (args, "row", &row) && row >= 1 && row <= h)
		liwdg_group_remove_row (args->self, row - 1);
}

/* @luadoc
 * --- Gets a child widget.
 * --
 * -- @param self Group.
 * -- @param args Arguments.<ul>
 * --   <li>row: Row number.</li>
 * --   <li>col: Column number.</li></ul>
 * -- @return Widget or nil.
 * function Group.get_child(self, args)
 */
static void Group_get_child (LIScrArgs* args)
{
	int x = 1;
	int y = 1;
	int w;
	int h;
	LIWdgWidget* widget;

	liwdg_group_get_size (args->self, &w, &h);
	liscr_args_gets_int (args, "col", &x);
	liscr_args_gets_int (args, "row", &y);
	if (x <= 0 || x > w || y <= 0 || y > h)
		return;	
	widget = liwdg_group_get_child (args->self, x - 1, y - 1);
	if (widget != NULL)
		liscr_args_seti_data (args, liwdg_widget_get_userdata (widget));
}

/* @luadoc
 * --- Places a widget inside the group.
 * --
 * -- @param self Group.
 * -- @param args Arguments.<ul>
 * --   <li>col: Column number. (required)</li>
 * --   <li>row: Row number. (required)</li>
 * --   <li>widget: Widget.</li></ul>
 * function Group.set_child(self, args)
 */
static void Group_set_child (LIScrArgs* args)
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
	liscr_args_gets_data (args, "widget", LICLI_SCRIPT_WIDGET, &data);
	liwdg_group_get_size (args->self, &w, &h);
	if (x < 1 || x > w || y < 1 || y > h)
		return;

	/* Attach new widget. */
	if (data != NULL)
	{
		liwdg_widget_detach (data->data);
		liwdg_group_set_child (args->self, x - 1, y - 1, data->data);
	}
	else
		liwdg_group_set_child (args->self, x - 1, y - 1, NULL);
}

/* @luadoc
 * --- Enables or disables row or column expansion.
 * --
 * -- @param self Group.
 * -- @param args Arguments.<ul>
 * --   <li>row: Row number.</li>
 * --   <li>col: Column number.</li>
 * --   <li>expand: Boolean.</li></ul>
 * function Group.set_expand(self, args)
 */
static void Group_set_expand (LIScrArgs* args)
{
	int i;
	int w;
	int h;
	int expand = 1;

	liwdg_group_get_size (args->self, &w, &h);
	liscr_args_gets_bool (args, "expand", &expand);
	if (liscr_args_gets_int (args, "col", &i) && i >= 1 && i <= w)
		liwdg_group_set_col_expand (args->self, i - 1, expand);
	if (liscr_args_gets_int (args, "row", &i) && i >= 1 && i <= h)
		liwdg_group_set_row_expand (args->self, i - 1, expand);
}

/* @luadoc
 * --- Number of columns in the group.
 * -- @name Group.cols
 * -- @class table
 */
static void Group_getter_cols (LIScrArgs* args)
{
	int w;
	int h;

	liwdg_group_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, w);
}
static void Group_setter_cols (LIScrArgs* args)
{
	int w;
	int h;
	int cols;

	liwdg_group_get_size (args->self, &w, &h);
	if (liscr_args_geti_int (args, 0, &cols) && cols >= 0)
		liwdg_group_set_size (args->self, cols, h);
}

/* @luadoc
 * --- Number of rows in the group.
 * -- @name Group.rows
 * -- @class table
 */
static void Group_getter_rows (LIScrArgs* args)
{
	int w;
	int h;

	liwdg_group_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, h);
}
static void Group_setter_rows (LIScrArgs* args)
{
	int w;
	int h;
	int rows;

	liwdg_group_get_size (args->self, &w, &h);
	if (liscr_args_geti_int (args, 0, &rows) || rows >= 0)
		liwdg_group_set_size (args->self, w, rows);
}

/*****************************************************************************/

void
licli_script_group (LIScrClass* self,
                    void*       data)
{
	liscr_class_inherit (self, licli_script_widget, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_GROUP, data);
	liscr_class_insert_interface (self, LICLI_SCRIPT_GROUP);
	liscr_class_insert_mfunc (self, "append_col", Group_append_col);
	liscr_class_insert_mfunc (self, "append_row", Group_append_row);
	liscr_class_insert_func (self, "insert_col", Group_insert_col);
	liscr_class_insert_func (self, "insert_row", Group_insert_row);
	liscr_class_insert_cfunc (self, "new", Group_new);
	liscr_class_insert_mfunc (self, "remove", Group_remove);
	liscr_class_insert_mfunc (self, "get_child", Group_get_child);
	liscr_class_insert_mfunc (self, "set_child", Group_set_child);
	liscr_class_insert_mfunc (self, "set_expand", Group_set_expand);
	liscr_class_insert_mvar (self, "cols", Group_getter_cols, Group_setter_cols);
	liscr_class_insert_mvar (self, "rows", Group_getter_rows, Group_setter_rows);
}

/** @} */
/** @} */
