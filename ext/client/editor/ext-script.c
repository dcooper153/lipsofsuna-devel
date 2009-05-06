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
 * \addtogroup liextcliEditor Editor
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-editor.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Editor"
 * ---
 * -- Edit the game world.
 * -- @name Editor
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates an object.
 * --
 * -- @param self Editor class.
 * -- @param table position Position vector.
 * -- @param table model Model string.
 * function Editor.create(position, model)
 */
static int
Editor_create (lua_State* lua)
{
	const char* name;
	liengModel* model;
	liextModule* module;
	limatTransform transform;
	liscrData* vector;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);
	vector = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	name = luaL_checkstring (lua, 3);

	model = lieng_engine_find_model_by_name (module->editor->module->engine, name);
	if (model == NULL)
		return 0;
	transform = limat_transform_init (
		*((limatVector*) vector->data),
		limat_quaternion_init (0.0f, 0.0f, 0.0f, 1.0f));
	liext_editor_create (module->editor, model->id, &transform);

	return 0;
}

/* @luadoc
 * ---
 * -- Begins a rotation drag.
 * --
 * -- @param self Editor class.
 * function Editor.rotate(self)
 */
static int
Editor_rotate (lua_State* lua)
{
	int x;
	int y;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);

	SDL_GetMouseState (&x, &y);
	liext_editor_begin_rotate (module->editor, x, y);

	return 0;
}

/* @luadoc
 * ---
 * -- Sends a map save request to the server.
 * --
 * -- @param self Editor class.
 * function Editor.save(self)
 */
static int
Editor_save (lua_State* lua)
{
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);
	liext_editor_save (module->editor);

	return 0;
}

/* @luadoc
 * ---
 * -- Snaps selected objects to grid.
 * --
 * -- @param self Editor class.
 * -- @param [grid] Grid spacing.
 * -- @param [angle] Rotation step size in degrees.
 * function Editor.snap(self, grid)
 */
static int
Editor_snap (lua_State* lua)
{
	float angle;
	float grid;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);
	if (lua_isnumber (lua, 2))
		grid = luaL_checknumber (lua, 2);
	else
		grid = 1.0f;
	if (lua_isnumber (lua, 3))
		angle = luaL_checknumber (lua, 3) / 180.0f * M_PI;
	else
		angle = 0.0f;

	liext_editor_snap (module->editor, grid, angle);

	return 0;
}

/* @luadoc
 * ---
 * -- Begins a translation drag.
 * --
 * -- @param self Editor class.
 * function Editor.translate(self)
 */
static int
Editor_translate (lua_State* lua)
{
	int x;
	int y;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);

	SDL_GetMouseState (&x, &y);
	liext_editor_begin_translate (module->editor, x, y);

	return 0;
}

/* @luadoc
 * ---
 * -- Visibility of the editor window.
 * -- @name Editor.visible
 * -- @class table
 */
static int
Editor_getter_visible (lua_State* lua)
{
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);

	lua_pushboolean (lua, liwdg_widget_get_visible (module->dialog));
	return 1;
}
static int
Editor_setter_visible (lua_State* lua)
{
	int value;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EDITOR);
	value = lua_toboolean (lua, 3);

	liwdg_widget_set_visible (module->dialog, value);
	return 0;
}

/*****************************************************************************/

void
liextEditorScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EDITOR, data);
	liscr_class_insert_func (self, "create", Editor_create);
	liscr_class_insert_func (self, "rotate", Editor_rotate);
	liscr_class_insert_func (self, "save", Editor_save);
	liscr_class_insert_func (self, "snap", Editor_snap);
	liscr_class_insert_func (self, "translate", Editor_translate);
	liscr_class_insert_getter (self, "visible", Editor_getter_visible);
	liscr_class_insert_setter (self, "visible", Editor_setter_visible);
}

/** @} */
/** @} */
/** @} */
