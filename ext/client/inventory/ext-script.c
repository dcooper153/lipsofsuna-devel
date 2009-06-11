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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliInventory Inventory
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"
#include "ext-widget.h"

static int
private_callback_activated (liscrData* data,
                            int        slot)
{
	liscrScript* script = liscr_data_get_script (data);

	liscr_pushdata (script->lua, data);
	lua_getfield (script->lua, -1, "activated");
	if (lua_type (script->lua, -1) == LUA_TFUNCTION)
	{
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		lua_pushnumber (script->lua, slot + 1);
		if (lua_pcall (script->lua, 2, 0, 0) != 0)
		{
			lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
		return 0;
	}
	else
		lua_pop (script->lua, 2);
	return 1;
}

/*****************************************************************************/

/* @luadoc
 * module "Extension.Client.Inventory"
 * ---
 * -- Display inventory items.
 * -- @name InventoryWidget
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new inventory widget.
 * --
 * -- @param self Inventory widget class.
 * -- @param table Optional table of arguments.
 * -- @return New inventory widget.
 * function InventoryWidget.new(self, table)
 */
static int
InventoryWidget_new (lua_State* lua)
{
	liscrData* self;
	liextModule* module;
	liscrScript* script;
	liwdgWidget* widget;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_INVENTORY_WIDGET);

	/* Allocate widget. */
	widget = liext_inventory_widget_new (module->inventory);
	if (widget == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Allocate userdata. */
	self = liscr_data_new (script, widget, LIEXT_SCRIPT_INVENTORY_WIDGET);
	if (self == NULL)
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}
	liwdg_widget_set_userdata (widget, self);
	if (!liwdg_widget_insert_callback (widget, LIEXT_CALLBACK_ACTIVATE, 0, private_callback_activated, self, NULL))
	{
		liwdg_widget_free (widget);
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);

	return 1;
}

/*****************************************************************************/

void
liextInventoryWidgetScript (liscrClass* self,
                            void*       data)
{
	liextModule* module = data;

	liscr_class_set_userdata (self, LIEXT_SCRIPT_INVENTORY_WIDGET, data);
	liscr_class_inherit (self, licliWidgetScript, module->module);
	liscr_class_insert_func (self, "new", InventoryWidget_new);
}

/** @} */
/** @} */
/** @} */
