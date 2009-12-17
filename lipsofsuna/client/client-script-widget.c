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
 */

#include "lips-client.h"

enum
{
	POPUP_LEFT,
	POPUP_RIGHT,
	POPUP_UP,
	POPUP_DOWN,
	POPUP_MAX
};

static void
private_detach_child (liwdgWidget* parent,
                      liwdgWidget* child);

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Widget"
 * ---
 * -- Manipulate widgets.
 * -- @name Widget
 * -- @class table
 */

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
			case POPUP_UP: rect.y -= size.height; break;
			case POPUP_DOWN: rect.y += rect.height; break;
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
 * -- Sets the user size request of widget.
 * --
 * -- @param self Widget.
 * -- @param width Width in pixels or -1 to unset.
 * -- @param height Height in pixels or -1 to unset.
 * function Widget.set_request(self, width, height)
 */
static int
Widget_set_request (lua_State* lua)
{
	liscrData* self;
	liwdgSize size;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET);
	size.width = luaL_checknumber (lua, 2);
	size.height = luaL_checknumber (lua, 3);
	if (size.width < 0)
		size.width = -1;
	if (size.height < 0)
		size.height = -1;

	liwdg_widget_set_request (self->data, size.width, size.height);

	return 0;
}

/* @luadoc
 * ---
 * -- Style name of the widget.
 * -- @name Widget.style
 * -- @class table
 */
static int
Widget_getter_style (lua_State* lua)
{
	liwdgWidget* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET)->data;

	if (self->style_name == NULL)
		return 0;
	lua_pushstring (lua, self->style_name);

	return 1;
}
static int
Widget_setter_style (lua_State* lua)
{
	const char* value;
	liwdgWidget* self;

	self = liscr_checkdata (lua, 1, LICLI_SCRIPT_WIDGET)->data;
	if (!lua_isnoneornil (lua, 3))
		value = lua_tostring (lua, 3);
	else
		value = NULL;

	liwdg_widget_set_style (self, value);

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
 * \brief Frees the widget without freeing its children.
 *
 * Detaches the children of the widget, leaking them if their memory isn't
 * managed externally in some way, detaches itself from the parent container,
 * and then only frees its own memory.
 *
 * \param self Widget.
 * \param data Script data.
 */
void
licli_script_widget_free (liwdgWidget* self,
                          liscrData*   data)
{
	licli_script_widget_detach_children (data);
	licli_script_widget_detach (data);
	liwdg_widget_free (self);
}

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
	liwdgWidget* widget = self->data;

	if (liwdg_widget_typeis (widget, &liwdgContainerType))
		liwdg_container_foreach_child (LIWDG_CONTAINER (widget), private_detach_child, widget);
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
	liscr_class_insert_func (self, "popup", Widget_popup);
	liscr_class_insert_func (self, "set_request", Widget_set_request);
	liscr_class_insert_getter (self, "style", Widget_getter_style);
	liscr_class_insert_getter (self, "visible", Widget_getter_visible);
	liscr_class_insert_getter (self, "x", Widget_getter_x);
	liscr_class_insert_getter (self, "y", Widget_getter_y);
	liscr_class_insert_setter (self, "style", Widget_setter_style);
	liscr_class_insert_setter (self, "visible", Widget_setter_visible);
}

/*****************************************************************************/

static void
private_detach_child (liwdgWidget* parent,
                      liwdgWidget* child)
{
	if (child != NULL && child->userdata != NULL)
	{
		liwdg_container_detach_child (LIWDG_CONTAINER (parent), child);
		liscr_data_unref (child->userdata, parent->userdata);
	}
}

/** @} */
/** @} */
