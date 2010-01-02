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

#include "lips-client.h"

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Group"
 * ---
 * -- Pack widgets in a grid.
 * -- @name Group
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Appends a column to the widget.
 * --
 * -- @param self Group.
 * -- @param args List of widgets.
 * function Group.append_row(self, args)
 */
static void Group_append_col (liscrArgs* args)
{
	int i;
	int w;
	int h;
	liscrData* data;

	/* Append column. */
	liwdg_group_get_size (args->self, &w, &h);
	if (!liwdg_group_append_col (args->self))
		return;

	/* Append rows. */
	for (i = 0 ; i < h && liscr_args_geti_data (args, i, LICLI_SCRIPT_WIDGET, &data) ; i++)
	{
		licli_script_widget_detach (data);
		liscr_data_ref (data, args->data);
		liwdg_group_set_child (args->self, w, i, data->data);
	}
}

/* @luadoc
 * ---
 * -- Appends a row to the widget.
 * --
 * -- @param self Group.
 * -- @param args List of widgets.
 * function Group.append_row(self, args)
 */
static void Group_append_row (liscrArgs* args)
{
	int i;
	int w;
	int h;
	liscrData* data;

	/* Append row. */
	liwdg_group_get_size (args->self, &w, &h);
	if (!liwdg_group_append_row (args->self))
		return;

	/* Append columns. */
	for (i = 0 ; i < w && liscr_args_geti_data (args, i, LICLI_SCRIPT_WIDGET, &data) ; i++)
	{
		licli_script_widget_detach (data);
		liscr_data_ref (data, args->data);
		liwdg_group_set_child (args->self, i, h, data->data);
	}
}

/* @luadoc
 * ---
 * -- Inserts a column to the widget.
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
	liscrData* self;
	liscrData* widget;

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
			lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
			return 0;
		}
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Inserts a row to the widget.
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
	liscrData* self;
	liscrData* widget;

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
			lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
			return 0;
		}
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new group.
 * --
 * -- @param self Group class.
 * -- @param args Arguments.
 * -- @return New group.
 * function Group.new(self, args)
 */
static void Group_new (liscrArgs* args)
{
	licliClient* client;
	liscrData* data;
	liwdgWidget* self;

	/* Allocate userdata. */
	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_GROUP);
	self = liwdg_group_new (client->widgets);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, self, LICLI_SCRIPT_GROUP, licli_script_widget_free);
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
 * ---
 * -- Removes a column from the widget.
 * --
 * -- @param self Group.
 * -- @param col Column index.
 * function Group.remove_col(self, col)
 */
static int
Group_remove_col (lua_State* lua)
{
	int y;
	int w;
	int h;
	int col;
	liscrData* self;
	liscrData* data;
	liwdgWidget* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	col = luaL_checkint (lua, 2) - 1;
	liwdg_group_get_size (self->data, &w, &h);
	luaL_argcheck (lua, col >= 0 && col < h, 2, "invalid column");

	/* Detach scripted widgets. */
	for (y = 0 ; y < h ; y++)
	{
		widget = liwdg_group_get_child (self->data, col, y);
		if (widget != NULL)
		{
			data = liwdg_widget_get_userdata (widget);
			if (data != NULL)
				licli_script_widget_detach (data);
		}
	}

	/* Remove column. */
	liwdg_group_remove_col (self->data, col);

	return 0;
}

/* @luadoc
 * ---
 * -- Removes a row from the widget.
 * --
 * -- @param self Group.
 * -- @param row Row index.
 * function Group.remove_row(self, row)
 */
static int
Group_remove_row (lua_State* lua)
{
	int x;
	int w;
	int h;
	int row;
	liscrData* self;
	liscrData* data;
	liwdgWidget* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	row = luaL_checkint (lua, 2) - 1;
	liwdg_group_get_size (self->data, &w, &h);
	luaL_argcheck (lua, row >= 0 && row < h, 2, "invalid row");

	/* Detach scripted widgets. */
	for (x = 0 ; x < w ; x++)
	{
		widget = liwdg_group_get_child (self->data, x, row);
		if (widget != NULL)
		{
			data = liwdg_widget_get_userdata (widget);
			if (data != NULL)
				licli_script_widget_detach (data);
		}
	}

	/* Remove row. */
	liwdg_group_remove_row (self->data, row);

	return 0;
}

/* @luadoc
 * ---
 * -- Gets a child widget.
 * --
 * -- Arguments:
 * -- row: Row number.
 * -- col: Column number.
 * --
 * -- @param self Group.
 * -- @return Widget or nil.
 * function Group.get_child(self, args)
 */
static void Group_get_child (liscrArgs* args)
{
	int x = 1;
	int y = 1;
	int w;
	int h;
	liwdgWidget* widget;

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
 * ---
 * -- Places a widget inside the group.
 * --
 * -- Arguments:
 * -- col: Column number. (required)
 * -- row: Row number. (required)
 * -- widget: Widget.
 * --
 * -- @param self Group.
 * -- @param args Arguments.
 * function Group.set_child(self, args)
 */
static void Group_set_child (liscrArgs* args)
{
	int x = 1;
	int y = 1;
	int w;
	int h;
	liscrData* data;
	liwdgWidget* widget;

	/* Arguments. */
	if (!liscr_args_gets_int (args, "col", &x) ||
	    !liscr_args_gets_int (args, "row", &y))
		return;
	liscr_args_gets_data (args, "widget", LICLI_SCRIPT_WIDGET, &data);
	liwdg_group_get_size (args->self, &w, &h);
	if (x <= 0 || x > w || y <= 0 || y > h)
		return;

	/* Detach old widget. */
	widget = liwdg_group_get_child (args->self, x - 1, y - 1);
	if (widget != NULL)
		licli_script_widget_detach (liwdg_widget_get_userdata (widget));

	/* Attach new widget. */
	if (data != NULL)
	{
		licli_script_widget_detach (data);
		liscr_data_ref (data, args->data);
		liwdg_group_set_child (args->self, x - 1, y - 1, data->data);
	}
	else
		liwdg_group_set_child (args->self, x - 1, y - 1, NULL);
}

/* @luadoc
 * ---
 * -- Enables or disables row or column expansion.
 * --
 * -- Arguments:
 * -- row: Row number.
 * -- col: Column number.
 * -- expand: Boolean.
 * --
 * -- @param self Group.
 * -- @param args Arguments.
 * function Group.set_expand(self, args)
 */
static void Group_set_expand (liscrArgs* args)
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
		liwdg_group_set_col_expand (args->self, i - 1, expand);
}

/* @luadoc
 * ---
 * -- Number of rows in the group.
 * -- @name Group.cols
 * -- @class table
 */
static void Group_getter_cols (liscrArgs* args)
{
	int w;
	int h;

	liwdg_group_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, w);
}
static void Group_setter_cols (liscrArgs* args)
{
	int w;
	int h;
	int y;
	int cols;
	liscrData* data;
	liwdgWidget* widget;

	if (!liscr_args_geti_int (args, 0, &cols) || cols < 0)
		return;

	/* Detach scripted widgets. */
	liwdg_group_get_size (args->self, &w, &h);
	for (w-- ; w >= cols ; w--)
	{
		for (y = 0 ; y < h ; y++)
		{
			widget = liwdg_group_get_child (args->self, w, y);
			if (widget != NULL)
			{
				data = liwdg_widget_get_userdata (widget);
				if (data != NULL)
					licli_script_widget_detach (data);
			}
		}
	}

	/* Set new size. */
	liwdg_group_set_size (args->self, cols, h);
}

/* @luadoc
 * ---
 * -- Number of rows in the group.
 * -- @name Group.rows
 * -- @class table
 */
static void Group_getter_rows (liscrArgs* args)
{
	int w;
	int h;

	liwdg_group_get_size (args->self, &w, &h);
	liscr_args_seti_int (args, h);
}
static void Group_setter_rows (liscrArgs* args)
{
	int w;
	int h;
	int x;
	int rows;
	liscrData* data;
	liwdgWidget* widget;

	if (!liscr_args_geti_int (args, 0, &rows) || rows < 0)
		return;

	/* Detach scripted widgets. */
	liwdg_group_get_size (args->self, &w, &h);
	for (h-- ; h >= rows ; h--)
	{
		for (x = 0 ; x < w ; x++)
		{
			widget = liwdg_group_get_child (args->self, x, h);
			if (widget != NULL)
			{
				data = liwdg_widget_get_userdata (widget);
				if (data != NULL)
					licli_script_widget_detach (data);
			}
		}
	}

	/* Set new size. */
	liwdg_group_set_size (args->self, w, rows);
}

/*****************************************************************************/

void
licliGroupScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licliWidgetScript, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_GROUP, data);
	liscr_class_insert_interface (self, LICLI_SCRIPT_GROUP);
	liscr_class_insert_mfunc (self, "append_col", Group_append_col);
	liscr_class_insert_mfunc (self, "append_row", Group_append_row);
	liscr_class_insert_func (self, "insert_col", Group_insert_col);
	liscr_class_insert_func (self, "insert_row", Group_insert_row);
	liscr_class_insert_cfunc (self, "new", Group_new);
	liscr_class_insert_func (self, "remove_col", Group_remove_col);
	liscr_class_insert_func (self, "remove_row", Group_remove_row);
	liscr_class_insert_mfunc (self, "get_child", Group_get_child);
	liscr_class_insert_mfunc (self, "set_child", Group_set_child);
	liscr_class_insert_mfunc (self, "set_expand", Group_set_expand);
	liscr_class_insert_mvar (self, "cols", Group_getter_cols, Group_setter_cols);
	liscr_class_insert_mvar (self, "rows", Group_getter_rows, Group_setter_rows);
}

/** @} */
/** @} */
