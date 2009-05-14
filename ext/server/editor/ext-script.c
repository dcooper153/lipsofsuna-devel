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

	LI_FOREACH_U32DIC (iter, module->server->engine->sectors)
	{
		sector = iter.value;
		vector = *((limatVector*) center->data);
		vector = limat_vector_subtract (vector, sector->origin);
		lieng_sector_fill_sphere (sector, &vector, radius, terrain);
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
	liscr_class_insert_func (self, "fill_sphere", Editor_fill_sphere);
	liscr_class_insert_func (self, "save", Editor_save);
}

/** @} */
/** @} */
/** @} */
