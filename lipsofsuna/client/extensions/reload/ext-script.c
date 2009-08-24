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
 * \addtogroup liextcliReload Reload
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"
#include "ext-reload.h"

/* @luadoc
 * module "Extension.Client.Reload"
 * ---
 * -- Reload modified data files without restarting.
 * -- @name Reload
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Cancels any ongoing reload.
 * --
 * -- @param self Reload class.
 * function Reload.cancel(self)
 */
static int
Reload_cancel (lua_State* lua)
{
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_RELOAD);

	liext_reload_cancel (self->reload);

	return 0;
}

/* @luadoc
 * ---
 * -- Reloads all modified data files.
 * --
 * -- @param self Reload class.
 * function Reload.reload(self)
 */
static int
Reload_reload (lua_State* lua)
{
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_RELOAD);

	liext_reload_run (self->reload);

	return 0;
}

/* @luadoc
 * ---
 * -- Automatic reload flag.
 * --
 * -- Automatic reloading is disabled by default.
 * --
 * -- @name Reload.enabled
 * -- @class table
 */
static int
Reload_getter_enabled (lua_State* lua)
{
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_RELOAD);

	lua_pushboolean (lua, liext_reload_get_enabled (self->reload));
	return 1;
}
static int
Reload_setter_enabled (lua_State* lua)
{
	int value;
	liextModule* self;

	self = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_RELOAD);
	value = lua_toboolean (lua, 3);

	liext_reload_set_enabled (self->reload, value);
	return 0;
}

/*****************************************************************************/

void
liextReloadScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_RELOAD, data);
	liscr_class_insert_func (self, "cancel", Reload_cancel);
	liscr_class_insert_func (self, "reload", Reload_reload);
	liscr_class_insert_getter (self, "enabled", Reload_getter_enabled);
	liscr_class_insert_setter (self, "enabled", Reload_setter_enabled);
}

/** @} */
/** @} */
/** @} */
