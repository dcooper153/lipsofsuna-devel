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
 * -- Creates a new group.
 * --
 * -- @param self Group class.
 * -- @param width Number of columns to create.
 * -- @param height Number of rows to create.
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
	width = (int) luaL_checkint (lua, 2);
	height = (int) luaL_checkint (lua, 3);
	luaL_argcheck (lua, width >= 0, 2, "invalid size");
	luaL_argcheck (lua, height >= 0, 3, "invalid size");

	/* Allocate userdata. */
	widget = liwdg_group_new (module->widgets);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}
	if (!liwdg_group_set_size (LIWDG_GROUP (widget), width, height))
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
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

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_GROUP);
	x = luaL_checkint (lua, 2);
	y = luaL_checkint (lua, 3);
	liwdg_group_get_size (LIWDG_GROUP (self->data), &w, &h);
	luaL_argcheck (lua, x >= 0 && x < w, 2, "invalid column");
	luaL_argcheck (lua, y >= 0 && y < h, 2, "invalid row");
	newwidget = liscr_checkiface (lua, 4, LICLI_SCRIPT_WIDGET);
	widget = newwidget->data;
	luaL_argcheck (lua, widget->state == LIWDG_WIDGET_STATE_DETACHED, 4, "widget already in use");
	luaL_argcheck (lua, widget->parent == NULL, 4, "widget already in use");

	/* Detach and unreference old child. */
	widget = liwdg_group_get_child (LIWDG_GROUP (self->data), x, y);
	if (widget != NULL)
	{
		oldwidget = liscr_script_find_data (script, widget);
		luaL_argcheck (lua, oldwidget == NULL, 4, "cell reserved by native widget");
		liscr_data_unref (oldwidget, self);
	}

	/* Insert and reference new widget. */
	liscr_data_ref (newwidget, self);
	liwdg_group_set_child (LIWDG_GROUP (self->data), x, y, newwidget->data);

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

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_GROUP);
	x = luaL_checkint (lua, 2);
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

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_GROUP);
	y = luaL_checkint (lua, 2);
	expand = lua_toboolean (lua, 3);
	liwdg_group_get_size (LIWDG_GROUP (self->data), &w, &h);
	luaL_argcheck (lua, y >= 0 && y < h, 2, "invalid row");

	/* Set row expand. */
	liwdg_group_set_row_expand (LIWDG_GROUP (self->data), y, expand);

	return 0;
}

/*****************************************************************************/

void
licliGroupScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licliWidgetScript, data);
	liscr_class_set_userdata (self, LICLI_SCRIPT_GROUP, data);
	liscr_class_insert_interface (self, LICLI_SCRIPT_GROUP);
	liscr_class_insert_func (self, "new", Group_new);
	liscr_class_insert_func (self, "set_child", Group_set_child);
	liscr_class_insert_func (self, "set_col_expand", Group_set_col_expand);
	liscr_class_insert_func (self, "set_row_expand", Group_set_row_expand);
}

/** @} */
/** @} */
/** @} */
