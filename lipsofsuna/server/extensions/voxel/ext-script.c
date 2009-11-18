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

static int
Tile___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LIEXT_SCRIPT_TILE);

	lisys_free (self->data);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new tile.
 * --
 * -- @param self Tile class.
 * -- @param args Optional table of arguments.
 * -- @return New tile.
 * function Tile.new(self, args)
 */
static int
Tile_new (lua_State* lua)
{
	liextModule* module;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_TILE);

	/* Allocate self. */
	self = liscr_data_new_alloc (script, sizeof (livoxVoxel), LIEXT_SCRIPT_TILE);
	if (self == NULL)
		return 0;

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, self, 2);

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Damage counter of the tile.
 * --
 * -- @name Tile.damage
 * -- @class table
 */
static int
Tile_getter_damage (lua_State* lua)
{
	liscrData* self;
	livoxVoxel* voxel;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_TILE);
	voxel = self->data;

	lua_pushnumber (lua, voxel->damage);
	return 1;
}
static int
Tile_setter_damage (lua_State* lua)
{
	int value;
	liscrData* self;
	livoxVoxel* voxel;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_TILE);
	value = luaL_checknumber (lua, 3);
	voxel = self->data;

	if (value <= 0)
		voxel->damage = 0;
	else if (value >= 0xFF)
		voxel->damage = 0xFF;
	else
		voxel->damage = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Rotation of the tile.
 * --
 * -- @name Tile.rotation
 * -- @class table
 */
static int
Tile_getter_rotation (lua_State* lua)
{
	liscrData* self;
	livoxVoxel* voxel;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_TILE);
	voxel = self->data;

	lua_pushnumber (lua, voxel->rotation);
	return 1;
}
static int
Tile_setter_rotation (lua_State* lua)
{
	int value;
	liscrData* self;
	livoxVoxel* voxel;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_TILE);
	value = luaL_checknumber (lua, 3);
	voxel = self->data;

	if (value <= 0)
		voxel->rotation = 0;
	else if (value >= 24)
		voxel->rotation = 0;
	else
		voxel->rotation = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Terrain type of the tile.
 * --
 * -- @name Tile.terrain
 * -- @class table
 */
static int
Tile_getter_terrain (lua_State* lua)
{
	liscrData* self;
	livoxVoxel* voxel;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_TILE);
	voxel = self->data;

	lua_pushnumber (lua, voxel->type);
	return 1;
}
static int
Tile_setter_terrain (lua_State* lua)
{
	int value;
	liscrData* self;
	livoxVoxel* voxel;

	self = liscr_checkdata (lua, 1, LIEXT_SCRIPT_TILE);
	value = luaL_checknumber (lua, 3);
	voxel = self->data;

	if (value <= 0)
		voxel->type = 0;
	else if (value >= 0xFFFF)
		voxel->type = 0xFFFF;
	else
		voxel->type = value;
	return 0;
}

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
 * -- Finds the voxel nearest to the given point.
 * --
 * -- @param self Voxel class.
 * -- @param point Point.
 * -- @param flags Optional find flags.
 * -- @return Type, damage, and center vector.
 * function Voxel.find_voxel(self, point, flags)
 */
static int
Voxel_find_voxel (lua_State* lua)
{
	int flags;
	liextModule* module;
	limatVector result;
	liscrData* center;
	liscrData* tile;
	liscrScript* script;
	livoxVoxel* voxel;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);
	center = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	if (lua_isnumber (lua, 3))
		flags = lua_tointeger (lua, 3);
	else
		flags = LIVOX_FIND_ALL;

	/* Find voxel. */
	voxel = livox_manager_find_voxel (module->voxels, flags, center->data, &result);
	if (voxel == NULL)
		return 0;

	/* Return tile object. */
	tile = liscr_data_new_alloc (script, sizeof (livoxVoxel), LIEXT_SCRIPT_TILE);
	if (tile == NULL)
		return 0;
	*((livoxVoxel*) tile->data) = *voxel;
	liscr_pushdata (lua, tile);
	liscr_data_unref (tile, NULL);

	/* Return position vector. */
	center = liscr_vector_new (liscr_script (lua), &result);
	if (center == NULL)
		return 1;
	liscr_pushdata (lua, center);

	return 2;
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
 * -- @param damage Optional damage value.
 * -- @return True if terrain was replaced.
 * function Voxel.replace_voxel(self, point, type, damage)
 */
static int
Voxel_replace_voxel (lua_State* lua)
{
	int type;
	int damage;
	liextModule* module;
	liscrData* center;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_VOXEL);
	center = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	type = luaL_checkinteger (lua, 3);
	if (lua_isnumber (lua, 4))
		damage = lua_tointeger (lua, 4);
	else
		damage = 0;

	if (type)
		lua_pushboolean (lua, livox_manager_replace_voxel (module->voxels, center->data, type, damage));
	else
		lua_pushboolean (lua, livox_manager_erase_voxel (module->voxels, center->data));

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
liextTileScript (liscrClass* self,
                 void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_insert_func (self, "__gc", Tile___gc);
	liscr_class_insert_func (self, "new", Tile_new);
	liscr_class_insert_getter (self, "damage", Tile_getter_damage);
	liscr_class_insert_getter (self, "rotation", Tile_getter_rotation);
	liscr_class_insert_getter (self, "terrain", Tile_getter_terrain);
	liscr_class_insert_setter (self, "damage", Tile_setter_damage);
	liscr_class_insert_setter (self, "rotation", Tile_setter_rotation);
	liscr_class_insert_setter (self, "terrain", Tile_setter_terrain);
}

void
liextVoxelScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_VOXEL, data);
	liscr_class_insert_enum (self, "FIND_ALL", LIVOX_FIND_ALL);
	liscr_class_insert_enum (self, "FIND_EMPTY", LIVOX_FIND_EMPTY);
	liscr_class_insert_enum (self, "FIND_FULL", LIVOX_FIND_FULL);
	liscr_class_insert_func (self, "erase_voxel", Voxel_erase_voxel);
	liscr_class_insert_func (self, "find_voxel", Voxel_find_voxel);
	liscr_class_insert_func (self, "insert_voxel", Voxel_insert_voxel);
	liscr_class_insert_func (self, "replace_voxel", Voxel_replace_voxel);
	liscr_class_insert_func (self, "save", Voxel_save);
}

/** @} */
/** @} */
/** @} */
