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
 * \addtogroup licliscrWidget Widget
 * @{
 */

#include <client/lips-client.h>
#include "lips-client-script.h"

enum
{
	POPUP_LEFT,
	POPUP_RIGHT,
	POPUP_UP,
	POPUP_DOWN,
	POPUP_MAX
};

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Widget"
 * ---
 * -- Manipulate widgets.
 * -- @name Widget
 * -- @class table
 */

static int
Widget___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET);
	if (self->data != NULL)
	{
		licli_script_widget_detach_children (self);
		licli_script_widget_detach (self);
		liwdg_widget_free (self->data);
	}
	liscr_data_free (self);

	return 0;
}

/* @luadoc
 * ---
 * -- Popup below the parent rectangle.
 * -- @name Widget.POPUP_DOWN
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Popup to the left from the parent rectangle.
 * -- @name Widget.POPUP_LEFT
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Popup to the right from the parent rectangle.
 * -- @name Widget.POPUP_RIGHT
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Popup above the parent rectangle.
 * -- @name Widget.POPUP_UP
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Pops up the widget.
 * --
 * -- @param self Widget.
 * -- @param rect Optional rectangle.
 * -- @param dir Optional popup direction from the rectangle.
 * function Widget.popup(self, rect, dir)
 */
static int
Widget_popup (lua_State* lua)
{
	int dir;
	liscrData* self;
	liwdgRect rect;
	liwdgSize screen;
	liwdgSize size;
	liwdgWidget* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET);
	widget = self->data;
	luaL_argcheck (lua, widget->state == LIWDG_WIDGET_STATE_DETACHED, 1, "widget already in use");
	luaL_argcheck (lua, widget->parent == NULL, 1, "widget already in use");

	/* Calculate position. */
	liwdg_widget_get_request (widget, &size);
	if (!lua_isnoneornil (lua, 2))
	{
		/* Get arguments. */
		luaL_checktype (lua, 2, LUA_TTABLE);
		if (!lua_isnoneornil (lua, 3))
		{
			dir = luaL_checkinteger (lua, 3);
			luaL_argcheck (lua, 0 <= dir && dir < POPUP_MAX, 3, "invalid popup direction");
		}
		else
			dir = POPUP_DOWN;
		lua_getfield (lua, 2, "x");
		if (lua_type (lua, -1) == LUA_TNUMBER)
			rect.x = lua_tonumber (lua, -1);
		else
			rect.x = 0;
		lua_pop (lua, 1);
		lua_getfield (lua, 2, "y");
		if (lua_type (lua, -1) == LUA_TNUMBER)
			rect.y = lua_tonumber (lua, -1);
		else
			rect.y = 0;
		lua_pop (lua, 1);
		lua_getfield (lua, 2, "width");
		if (lua_type (lua, -1) == LUA_TNUMBER)
			rect.width = lua_tonumber (lua, -1);
		else
			rect.width = 0;
		lua_pop (lua, 1);
		lua_getfield (lua, 2, "height");
		if (lua_type (lua, -1) == LUA_TNUMBER)
			rect.height = lua_tonumber (lua, -1);
		else
			rect.height = 0;
		lua_pop (lua, 1);

		/* Calculate relative offset. */
		switch (dir)
		{
			case POPUP_LEFT: rect.x -= size.width; break;
			case POPUP_RIGHT: rect.x += rect.width; break;
			case POPUP_UP: rect.y += rect.height; break;
			case POPUP_DOWN: rect.y -= size.height; break;
			default:
				assert (0);
				break;
		}
		rect.width = size.width;
		rect.height = size.height;
	}
	else
	{
		/* Place at center. */
		liwdg_manager_get_size (widget->manager, &screen.width, &screen.height);
		rect.x = (screen.width - size.width) / 2;
		rect.y = (screen.height - size.height) / 2;
		rect.width = size.width;
		rect.height = size.height;
	}

	/* Popup the widget. */
	liwdg_widget_set_visible (widget, 1);
	liwdg_manager_insert_popup (widget->manager, self->data);
	liwdg_widget_set_allocation (widget, rect.x, rect.y, rect.width, rect.height);
	liscr_data_ref (self, NULL);

	return 0;
}

/* @luadoc
 * ---
 * -- Visibility flag.
 * -- @name Widget.visible
 * -- @class table
 */
static int
Widget_getter_visible (lua_State* lua)
{
	int value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET);

	value = liwdg_widget_get_visible (self->data);
	lua_pushboolean (lua, value);

	return 1;
}
static int
Widget_setter_visible (lua_State* lua)
{
	int value;
	liscrData* self;
	liwdgWidget* widget;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET);
	value = lua_toboolean (lua, 3);
	widget = self->data;

	if (widget->state == LIWDG_WIDGET_STATE_POPUP)
		liscr_data_unref (self, NULL);
	liwdg_widget_set_visible (widget, value);

	return 0;
}

/* @luadoc
 * ---
 * -- Left edge position.
 * -- @name Widget.x
 * -- @class table
 */
static int
Widget_getter_x (lua_State* lua)
{
	liscrData* self;
	liwdgRect rect;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET);

	liwdg_widget_get_allocation (self->data, &rect);
	lua_pushnumber (lua, rect.x);

	return 1;
}

/* @luadoc
 * ---
 * -- Bottom edge position.
 * -- @name Widget.y
 * -- @class table
 */
static int
Widget_getter_y (lua_State* lua)
{
	liscrData* self;
	liwdgRect rect;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET);

	liwdg_widget_get_allocation (self->data, &rect);
	lua_pushnumber (lua, rect.y);

	return 1;
}

/*****************************************************************************/

/**
 * \brief Detaches the widget from the user interface.
 *
 * Used for unparenting the widget before certain widget operations, most
 * notably the deletion, so that it isn't double removed by the widget manager.
 *
 * \param self Script widget.
 */
void
licli_script_widget_detach (liscrData* self)
{
	liwdgWidget* widget = self->data;

	if (widget->parent != NULL)
	{
		assert (widget->parent->userdata != NULL);
		if (liscr_data_get_valid (widget->parent->userdata))
			liscr_data_unref (self, widget->parent->userdata);
	}
	liwdg_widget_detach (widget);
}

/**
 * \brief Detaches all scripted child widgets from the widget.
 *
 * Used for unparenting all children managed by scripts before certain widget
 * operations so that they aren't double removed when their garbage collection
 * methods are called.
 *
 * \param self Script widget.
 */
void
licli_script_widget_detach_children (liscrData* self)
{
	int x;
	int y;
	liwdgGroup* group;
	liwdgWidget* child;
	liwdgWidget* widget = self->data;

	if (liwdg_widget_typeis (widget, &liwdgGroupType))
	{
		group = LIWDG_GROUP (widget);
		for (x = 0 ; x < group->width ; x++)
		{
			for (y = 0 ; y < group->height ; y++)
			{
				child = liwdg_group_get_child (group, x, y);
				if (child != NULL && child->userdata != NULL)
				{
					liscr_data_unref (child->userdata, self);
					liwdg_group_set_child (group, x, y, NULL);
				}
			}
		}
	}
}

void
licliWidgetScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_WIDGET, data);
	liscr_class_insert_interface (self, LICLI_SCRIPT_WIDGET);
	liscr_class_insert_enum (self, "POPUP_DOWN", POPUP_DOWN);
	liscr_class_insert_enum (self, "POPUP_LEFT", POPUP_LEFT);
	liscr_class_insert_enum (self, "POPUP_RIGHT", POPUP_RIGHT);
	liscr_class_insert_enum (self, "POPUP_UP", POPUP_UP);
	liscr_class_insert_func (self, "__gc", Widget___gc);
	liscr_class_insert_func (self, "popup", Widget_popup);
	liscr_class_insert_getter (self, "visible", Widget_getter_visible);
	liscr_class_insert_getter (self, "x", Widget_getter_x);
	liscr_class_insert_getter (self, "y", Widget_getter_y);
	liscr_class_insert_setter (self, "visible", Widget_setter_visible);
}

/** @} */
/** @} */
/** @} */
