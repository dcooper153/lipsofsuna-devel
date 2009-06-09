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

static void
private_detach (liscrData* self);

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

	self = liscr_isiface (lua, 1, LICLI_SCRIPT_WIDGET);
	if (self->data != NULL)
	{
		private_detach (self);
		liwdg_widget_free (self->data);
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Pops up the widget.
 * --
 * -- @param self Widget.
 * -- @param x Optional X coordinate.
 * -- @param y Optional Y coordinate.
 * function Widget.popup(self, x, y)
 */
static int
Widget_popup (lua_State* lua)
{
	liscrData* self;
	liwdgRect rect;
	liwdgSize screen;
	liwdgSize size;
	liwdgWidget* widget;

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_WIDGET);
	widget = self->data;
	luaL_argcheck (lua, widget->state == LIWDG_WIDGET_STATE_DETACHED, 1, "widget already in use");
	luaL_argcheck (lua, widget->parent == NULL, 1, "widget already in use");
	liwdg_widget_get_request (widget, &size);
	liwdg_manager_get_size (widget->manager, &screen.width, &screen.height);
	rect.x = (screen.width - size.width) / 2;
	rect.y = (screen.height - size.height) / 2;
	rect.width = size.width;
	rect.height = size.height;
	if (!lua_isnoneornil (lua, 2))
		rect.x = (int) luaL_checknumber (lua, 2);
	if (!lua_isnoneornil (lua, 3))
		rect.y = (int) luaL_checknumber (lua, 3);

	liwdg_manager_insert_popup (widget->manager, self->data);
	liwdg_widget_set_allocation (widget, rect.x, rect.y, rect.width, rect.height);

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

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_WIDGET);

	value = liwdg_widget_get_visible (self->data);
	lua_pushboolean (lua, value);

	return 1;
}
static int
Widget_setter_visible (lua_State* lua)
{
	int value;
	liscrData* self;

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_WIDGET);
	value = lua_toboolean (lua, 3);

	liwdg_widget_set_visible (self->data, value);

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

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_WIDGET);

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

	self = liscr_checkiface (lua, 1, LICLI_SCRIPT_WIDGET);

	liwdg_widget_get_allocation (self->data, &rect);
	lua_pushnumber (lua, rect.y);

	return 1;
}

/*****************************************************************************/

static liscrData*
private_convert (liscrScript* script,
                 void*        data)
{
	return liwdg_widget_get_userdata (data);
}

static void
private_detach (liscrData* self)
{
	int x;
	int y;
	liwdgGroup* group;
	liwdgWidget* child;
	liwdgWidget* widget = self->data;

	/* Unparent all children managed by scripts so that they aren't
	   double removed when their garbage collection methods are called. */
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

	/* Unparent the widget itself so that the widget manager or any
	   potential parent widget doesn't access freed memory. */
	if (widget->parent != NULL && widget->parent->userdata != NULL)
		liscr_data_unref (self, widget->parent->userdata);
	liwdg_widget_detach (widget);
}

void
licliWidgetScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_convert (self, private_convert);
	liscr_class_set_userdata (self, LICLI_SCRIPT_WIDGET, data);
	liscr_class_insert_interface (self, LICLI_SCRIPT_WIDGET);
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
