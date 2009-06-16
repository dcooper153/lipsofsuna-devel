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
 * -- Fills a box with voxel terrain.
 * --
 * -- @param self Voxel class.
 * -- @param min Box minimum point.
 * -- @param max Box maximum point.
 * -- @param terrain Terrain type.
 * function Voxel.fill_box(self, min, max, terrain)
 */
static int
Voxel_fill_box (lua_State* lua)
{
	liengTile terrain;
	liextModule* module;
	liscrData* min;
	liscrData* max;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);
	min = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	max = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);
	terrain = luaL_checkinteger (lua, 4);
	luaL_argcheck (lua, terrain >= 0, 4, "invalid terrain type");

	liext_module_fill_box (module, min->data, max->data, terrain);

	return 0;
}

/* @luadoc
 * ---
 * -- Fills a sphere with voxel terrain.
 * --
 * -- @param self Voxel class.
 * -- @param center Sphere center.
 * -- @param radius Sphere radius.
 * -- @param terrain Terrain type.
 * function Voxel.fill_sphere(self, center, radius, terrain)
 */
static int
Voxel_fill_sphere (lua_State* lua)
{
	float radius;
	liengTile terrain;
	liextModule* module;
	liscrData* center;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);
	center = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	radius = luaL_checknumber (lua, 3);
	terrain = luaL_checkinteger (lua, 4);
	luaL_argcheck (lua, radius >= 0.0f, 3, "negative radius");
	luaL_argcheck (lua, terrain >= 0, 4, "invalid terrain type");

	liext_module_fill_sphere (module, center->data, radius, terrain);

	return 0;
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
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_insert_func (self, "fill_box", Voxel_fill_box);
	liscr_class_insert_func (self, "fill_sphere", Voxel_fill_sphere);
	liscr_class_insert_func (self, "save", Voxel_save);
}

/** @} */
/** @} */
/** @} */
