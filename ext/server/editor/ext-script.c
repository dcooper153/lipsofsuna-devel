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
 * \addtogroup liextsrvEditor Editor
 * @{
 */

#include <network/lips-network.h>
#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-editor.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Editor"
 * ---
 * -- Allow dynamic editing of the game.
 * -- @name Editor
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Fills a box with terrain.
 * --
 * -- @param self Editor class.
 * -- @param min Box minimum point.
 * -- @param max Box maximum point.
 * -- @param terrain Terrain type.
 * function Editor.fill_box(self, min, max, terrain)
 */
static int
Editor_fill_box (lua_State* lua)
{
	lialgU32dicIter iter;
	liarcWriter* writer;
	liengTile terrain;
	liengSector* sector;
	liextModule* module;
	limatAabb aabb;
	liscrData* min;
	liscrData* max;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);
	min = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	max = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);
	terrain = luaL_checkinteger (lua, 4);
	luaL_argcheck (lua, terrain >= 0, 4, "invalid terrain type");

	/* Modify sectors. */
	LI_FOREACH_U32DIC (iter, module->server->engine->sectors)
	{
		sector = iter.value;
		aabb.min = *((limatVector*) min->data);
		aabb.max = *((limatVector*) max->data);
		aabb.min = limat_vector_subtract (aabb.min, sector->origin);
		aabb.max = limat_vector_subtract (aabb.max, sector->origin);
		lieng_sector_fill_aabb (sector, &aabb, terrain);
	}

	/* Notify clients. */
	writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_BOX);
	if (writer != NULL)
	{
		aabb.min = *((limatVector*) min->data);
		aabb.max = *((limatVector*) max->data);
		liarc_writer_append_float (writer, aabb.min.x);
		liarc_writer_append_float (writer, aabb.min.y);
		liarc_writer_append_float (writer, aabb.min.z);
		liarc_writer_append_float (writer, aabb.max.x);
		liarc_writer_append_float (writer, aabb.max.y);
		liarc_writer_append_float (writer, aabb.max.z);
		liarc_writer_append_uint32 (writer, terrain);
		LI_FOREACH_U32DIC (iter, module->server->network->clients)
		{
#warning FIXME: Terrain edit events are sent to all clients.
			lisrv_client_send (iter.value, writer, GRAPPLE_RELIABLE);
		}
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Fills a sphere with terrain.
 * --
 * -- @param self Editor class.
 * -- @param center Sphere center.
 * -- @param radius Sphere radius.
 * -- @param terrain Terrain type.
 * function Editor.fill_sphere(self, center, radius, terrain)
 */
static int
Editor_fill_sphere (lua_State* lua)
{
	float radius;
	lialgU32dicIter iter;
	liarcWriter* writer;
	liengTile terrain;
	liengSector* sector;
	liextModule* module;
	limatVector vector;
	liscrData* center;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);
	center = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	radius = luaL_checknumber (lua, 3);
	terrain = luaL_checkinteger (lua, 4);
	luaL_argcheck (lua, radius >= 0.0f, 3, "negative radius");
	luaL_argcheck (lua, terrain >= 0, 4, "invalid terrain type");

	/* Modify sectors. */
	LI_FOREACH_U32DIC (iter, module->server->engine->sectors)
	{
		sector = iter.value;
		vector = *((limatVector*) center->data);
		vector = limat_vector_subtract (vector, sector->origin);
		lieng_sector_fill_sphere (sector, &vector, radius, terrain);
	}

	/* Notify clients. */
	writer = liarc_writer_new_packet (LIEXT_VOXEL_PACKET_SPHERE);
	if (writer != NULL)
	{
		vector = *((limatVector*) center->data);
		liarc_writer_append_float (writer, vector.x);
		liarc_writer_append_float (writer, vector.y);
		liarc_writer_append_float (writer, vector.z);
		liarc_writer_append_float (writer, radius);
		liarc_writer_append_uint32 (writer, terrain);
		LI_FOREACH_U32DIC (iter, module->server->network->clients)
		{
#warning FIXME: Terrain edit events are sent to all clients.
			lisrv_client_send (iter.value, writer, GRAPPLE_RELIABLE);
		}
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Saves the current world map.
 * --
 * -- @param self Editor class.
 * function Editor.save(self)
 */
static int
Editor_save (lua_State* lua)
{
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);

	lieng_engine_save (module->server->engine);

	return 0;
}

/*****************************************************************************/

void
liextEditorScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EDITOR, data);
	liscr_class_insert_func (self, "fill_box", Editor_fill_box);
	liscr_class_insert_func (self, "fill_sphere", Editor_fill_sphere);
	liscr_class_insert_func (self, "save", Editor_save);
}

/** @} */
/** @} */
/** @} */
