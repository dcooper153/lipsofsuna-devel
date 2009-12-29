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
 * -- Pops up the widget.
 * --
 * -- Arguments:
 * -- x: X coordinate.
 * -- y: Y coordinate.
 * -- width: Width allocation.
 * -- height: Height allocation.
 * -- dir: Popup direction. ("left"/"right"/"up"/"down")
 * --
 * -- @param self Widget.
 * -- @param args Arguments.
 * function Widget.popup(self, args)
 */
static void Widget_popup (liscrArgs* args)
{
	const char* dir;
	liwdgRect rect;
	liwdgSize screen;
	liwdgSize size;
	liwdgWidget* widget;

	widget = args->self;
	if (widget->state != LIWDG_WIDGET_STATE_DETACHED || widget->parent != NULL)
		return;

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
	liwdg_manager_insert_popup (widget->manager, widget);
	liwdg_widget_set_allocation (widget, rect.x, rect.y, rect.width, rect.height);
	liscr_data_ref (args->data, NULL);
}

/* @luadoc
 * ---
 * -- Sets the user size request of widget.
 * --
 * -- Arguments:
 * -- width: Width request.
 * -- height: Height request.
 * --
 * -- @param self Widget.
 * -- @param args Arguments.
 * function Widget.set_request(self, args)
 */
static void Widget_set_request (liscrArgs* args)
{
	liwdgSize size = { -1, -1 };

	liscr_args_gets_int (args, "width", &size.width);
	liscr_args_gets_int (args, "height", &size.height);
	liwdg_widget_set_request (args->self, size.width, size.height);
}

/* @luadoc
 * ---
 * -- Style name of the widget.
 * -- @name Widget.style
 * -- @class table
 */
static void Widget_getter_style (liscrArgs* args)
{
	liscr_args_seti_string (args, LIWDG_WIDGET (args->self)->style_name);
}
static void Widget_setter_style (liscrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liwdg_widget_set_style (args->self, value);
	else
		liwdg_widget_set_style (args->self, NULL);
}

/* @luadoc
 * ---
 * -- Visibility flag.
 * -- @name Widget.visible
 * -- @class table
 */
static void Widget_getter_visible (liscrArgs* args)
{
	liscr_args_seti_bool (args, liwdg_widget_get_visible (args->self));
}
static void Widget_setter_visible (liscrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		if (LIWDG_WIDGET (args->self)->state == LIWDG_WIDGET_STATE_POPUP)
			liscr_data_unref (args->data, NULL);
		liwdg_widget_set_visible (args->self, value);
	}
}

/* @luadoc
 * ---
 * -- Left edge position.
 * -- @name Widget.x
 * -- @class table
 */
static void Widget_getter_x (liscrArgs* args)
{
	liwdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.x);
}

/* @luadoc
 * ---
 * -- Bottom edge position.
 * -- @name Widget.y
 * -- @class table
 */
static void Widget_getter_y (liscrArgs* args)
{
	liwdgRect rect;

	liwdg_widget_get_allocation (args->self, &rect);
	liscr_args_seti_float (args, rect.y);
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
	liscr_class_insert_mfunc (self, "popup", Widget_popup);
	liscr_class_insert_mfunc (self, "set_request", Widget_set_request);
	liscr_class_insert_mvar (self, "style", Widget_getter_style, Widget_setter_style);
	liscr_class_insert_mvar (self, "visible", Widget_getter_visible, Widget_setter_visible);
	liscr_class_insert_mvar (self, "x", Widget_getter_x, NULL);
	liscr_class_insert_mvar (self, "y", Widget_getter_y, NULL);
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
