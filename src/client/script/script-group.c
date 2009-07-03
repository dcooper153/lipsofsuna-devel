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
 * \addtogroup licliscrGroup Group
 * @{
 */

#include <client/lips-client.h>
#include "lips-client-script.h"

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
 * -- @param widget Optional widget.
 * function Group.append_row(self, widget)
 */
static int
Group_append_col (lua_State* lua)
{
	int w;
	int h;
	liscrData* self;
	liscrData* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	if (!lua_isnoneornil (lua, 2))
		widget = liscr_checkdata (lua, 2, LICLI_SCRIPT_WIDGET);
	else
		widget = NULL;

	/* Append row. */
	if (!liwdg_group_append_col (self->data))
		return 0;
	liwdg_group_get_size (self->data, &w, &h);

	/* Set widget. */
	if (widget != NULL)
	{
		lua_getfield (lua, 1, "set_child");
		lua_pushvalue (lua, 1);
		lua_pushnumber (lua, w);
		lua_pushnumber (lua, 1);
		lua_pushvalue (lua, 2);
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
 * -- Appends a row to the widget.
 * --
 * -- @param self Group.
 * -- @param widget Optional widget.
 * function Group.append_row(self, widget)
 */
static int
Group_append_row (lua_State* lua)
{
	int w;
	int h;
	liscrData* self;
	liscrData* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	if (!lua_isnoneornil (lua, 2))
		widget = liscr_checkdata (lua, 2, LICLI_SCRIPT_WIDGET);
	else
		widget = NULL;

	/* Append row. */
	if (!liwdg_group_append_row (self->data))
		return 0;
	liwdg_group_get_size (self->data, &w, &h);

	/* Set widget. */
	if (widget != NULL)
	{
		lua_getfield (lua, 1, "set_child");
		lua_pushvalue (lua, 1);
		lua_pushnumber (lua, 1);
		lua_pushnumber (lua, h);
		lua_pushvalue (lua, 2);
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
 * -- @param width Optional number of columns to create.
 * -- @param height Optional number of rows to create.
 * -- @return New group.
 * function Group.new(self, width, height)
 */
static int
Group_new (lua_State* lua)
{
	int width;
	int height;
	licliModule* module;
	liscrData* self;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LICLI_SCRIPT_GROUP);

	/* Check arguments. */
	if (!lua_isnoneornil (lua, 2))
		width = luaL_checkint (lua, 2);
	else
		width = 0;
	if (!lua_isnoneornil (lua, 3))
		height = luaL_checkint (lua, 3);
	else
		height = 0;
	luaL_argcheck (lua, width >= 0, 2, "invalid size");
	luaL_argcheck (lua, height >= 0, 3, "invalid size");

	/* Allocate userdata. */
	widget = liwdg_group_new (module->widgets);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	if (width || height)
	{
		if (!liwdg_group_set_size (LIWDG_GROUP (widget), width, height))
		{
			liwdg_widget_free (widget);
			lua_pushnil (lua);
			return 1;
		}
	}
	self = liscr_data_new (script, widget, LICLI_SCRIPT_GROUP);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}
	liwdg_widget_set_userdata (widget, self);

	liscr_pushdata (lua, self);
	return 1;
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
 * -- Places a widget inside the group.
 * --
 * -- @param self Group.
 * -- @param x Column number.
 * -- @param y Row number.
 * -- @param widget Widget to insert.
 * function Group.set_child(self, x, y, widget)
 */
static int
Group_set_child (lua_State* lua)
{
	int x;
	int y;
	int w;
	int h;
	liwdgWidget* widget;
	liscrData* self;
	liscrData* oldwidget;
	liscrData* newwidget;
	liscrScript* script = liscr_script (lua);

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	x = luaL_checkint (lua, 2) - 1;
	y = luaL_checkint (lua, 3) - 1;
	liwdg_group_get_size (LIWDG_GROUP (self->data), &w, &h);
	luaL_argcheck (lua, x >= 0 && x < w, 2, "invalid column");
	luaL_argcheck (lua, y >= 0 && y < h, 2, "invalid row");
	if (!lua_isnoneornil (lua, 4))
	{
		newwidget = liscr_checkdata (lua, 4, LICLI_SCRIPT_WIDGET);
		widget = newwidget->data;
		luaL_argcheck (lua, widget->state == LIWDG_WIDGET_STATE_DETACHED, 4, "widget already in use");
		luaL_argcheck (lua, widget->parent == NULL, 4, "widget already in use");
	}
	else
		newwidget = NULL;

	/* Detach and unreference old child. */
	widget = liwdg_group_get_child (LIWDG_GROUP (self->data), x, y);
	if (widget != NULL)
	{
		oldwidget = liscr_script_find_data (script, widget);
		luaL_argcheck (lua, oldwidget == NULL, 4, "cell reserved by native widget");
		liscr_data_unref (oldwidget, self);
	}

	/* Insert and reference new widget. */
	if (newwidget != NULL)
	{
		liscr_data_ref (newwidget, self);
		liwdg_group_set_child (LIWDG_GROUP (self->data), x, y, newwidget->data);
	}
	else
		liwdg_group_set_child (LIWDG_GROUP (self->data), x, y, NULL);

	return 0;
}

/* @luadoc
 * ---
 * -- Enables or disables column expansion for a specific column.
 * --
 * -- @param self Group.
 * -- @param x Column number.
 * -- @param expand Boolean indicating whether the column should expand.
 * function Group.set_col_expand(self, x, expand)
 */
static int
Group_set_col_expand (lua_State* lua)
{
	int x;
	int w;
	int h;
	int expand;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	x = luaL_checkint (lua, 2) - 1;
	expand = lua_toboolean (lua, 3);
	liwdg_group_get_size (LIWDG_GROUP (self->data), &w, &h);
	luaL_argcheck (lua, x >= 0 && x < w, 2, "invalid column");

	/* Set row expand. */
	liwdg_group_set_col_expand (LIWDG_GROUP (self->data), x, expand);

	return 0;
}

/* @luadoc
 * ---
 * -- Enables or disables row expansion for a specific row.
 * --
 * -- @param self Group.
 * -- @param y Row number.
 * -- @param expand Boolean indicating whether the row should expand.
 * function Group.set_row_expand(self, y, expand)
 */
static int
Group_set_row_expand (lua_State* lua)
{
	int y;
	int w;
	int h;
	int expand;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);
	y = luaL_checkint (lua, 2) - 1;
	expand = lua_toboolean (lua, 3);
	liwdg_group_get_size (LIWDG_GROUP (self->data), &w, &h);
	luaL_argcheck (lua, y >= 0 && y < h, 2, "invalid row");

	/* Set row expand. */
	liwdg_group_set_row_expand (LIWDG_GROUP (self->data), y, expand);

	return 0;
}

/* @luadoc
 * ---
 * -- Number of rows in the group.
 * -- @name Group.cols
 * -- @class table
 */
static int
Group_getter_cols (lua_State* lua)
{
	int w;
	int h;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);

	liwdg_group_get_size (LIWDG_GROUP (self->data), &w, &h);
	lua_pushnumber (lua, w);

	return 1;
}

/* @luadoc
 * ---
 * -- Number of rows in the group.
 * -- @name Group.rows
 * -- @class table
 */
static int
Group_getter_rows (lua_State* lua)
{
	int w;
	int h;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_GROUP);

	liwdg_group_get_size (LIWDG_GROUP (self->data), &w, &h);
	lua_pushnumber (lua, h);

	return 1;
}

/*****************************************************************************/

void
licliGroupScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licliWidgetScript, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_GROUP, data);
	liscr_class_insert_interface (self, LICLI_SCRIPT_GROUP);
	liscr_class_insert_func (self, "append_col", Group_append_col);
	liscr_class_insert_func (self, "append_row", Group_append_row);
	liscr_class_insert_func (self, "insert_col", Group_insert_col);
	liscr_class_insert_func (self, "insert_row", Group_insert_row);
	liscr_class_insert_func (self, "new", Group_new);
	liscr_class_insert_func (self, "remove_col", Group_remove_col);
	liscr_class_insert_func (self, "remove_row", Group_remove_row);
	liscr_class_insert_func (self, "set_child", Group_set_child);
	liscr_class_insert_func (self, "set_col_expand", Group_set_col_expand);
	liscr_class_insert_func (self, "set_row_expand", Group_set_row_expand);
	liscr_class_insert_getter (self, "cols", Group_getter_cols);
	liscr_class_insert_getter (self, "rows", Group_getter_rows);
}

/** @} */
/** @} */
/** @} */
