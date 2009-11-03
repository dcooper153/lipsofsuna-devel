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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvVoxel Voxel
 * @{
 */

#include <network/lips-network.h>
#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-listener.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Voxel"
 * ---
 * -- Use dynamic voxel terrain.
 * -- @name Voxel
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Erases a voxel near the given point.
 * --
 * -- @param self Voxel class.
 * -- @param point Point.
 * -- @return True if terrain was erased.
 * function Voxel.erase_voxel(self, point)
 */
static int
Voxel_erase_voxel (lua_State* lua)
{
	liextModule* module;
	liscrData* center;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);
	center = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	lua_pushboolean (lua, livox_manager_erase_voxel (module->voxels, center->data));

	return 1;
}

/* @luadoc
 * ---
 * -- Inserts a voxel near the given point.
 * --
 * -- @param self Voxel class.
 * -- @param point Point.
 * -- @param type Terrain type.
 * -- @return True if terrain was filled.
 * function Voxel.insert_voxel(self, point, type)
 */
static int
Voxel_insert_voxel (lua_State* lua)
{
	int type;
	liextModule* module;
	liscrData* center;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);
	center = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	type = luaL_checkinteger (lua, 3);

	lua_pushboolean (lua, livox_manager_insert_voxel (module->voxels, center->data, type));

	return 1;
}

/* @luadoc
 * ---
 * -- Replaces the voxel near the given point.
 * --
 * -- @param self Voxel class.
 * -- @param point Point.
 * -- @param type Terrain type.
 * -- @return True if terrain was replaced.
 * function Voxel.replace_voxel(self, point, type)
 */
static int
Voxel_replace_voxel (lua_State* lua)
{
	int type;
	liextModule* module;
	liscrData* center;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);
	center = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	type = luaL_checkinteger (lua, 3);

	lua_pushboolean (lua, livox_manager_replace_voxel (module->voxels, center->data, type));

	return 1;
}

/* @luadoc
 * ---
 * -- Saves the terrain of the currently loaded sectors.
 * --
 * -- @param self Voxel class.
 * function Voxel.save(self)
 */
static int
Voxel_save (lua_State* lua)
{
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);

	if (!liext_module_write (module, module->server->sql))
		lisys_error_report ();

	return 0;
}

/*****************************************************************************/

void
liextVoxelScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_insert_func (self, "erase_voxel", Voxel_erase_voxel);
	liscr_class_insert_func (self, "insert_voxel", Voxel_insert_voxel);
	liscr_class_insert_func (self, "replace_voxel", Voxel_replace_voxel);
	liscr_class_insert_func (self, "save", Voxel_save);
}

/** @} */
/** @} */
/** @} */
