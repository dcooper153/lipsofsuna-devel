/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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

#include <lipsofsuna/client.h>
#include <lipsofsuna/script.h>
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
 * -- Arguments:
 * -- model: Model name. (required)
 * -- position: Position vector. (required)
 * -- rotation: Quaternion.
 * --
 * -- @param self Editor class.
 * -- @param args Arguments.
 * function Editor.create(self, args)
 */
static void Editor_create (LIScrArgs* args)
{
	const char* name;
	LIEngModel* model;
	LIExtModule* module;
	LIMatTransform transform;

	if (liscr_args_gets_vector (args, "position", &transform.position) &&
	    liscr_args_gets_string (args, "model", &name))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EDITOR);
		model = lieng_engine_find_model_by_name (module->editor->client->engine, name);
		if (model == NULL)
			return;
		if (!liscr_args_gets_quaternion (args, "rotation", &transform.rotation))
			transform.rotation = limat_quaternion_identity ();
		liext_editor_create (module->editor, model->id, &transform);
	}
}

/* @luadoc
 * ---
 * -- Begins a rotation drag.
 * --
 * -- @param self Editor class.
 * function Editor.rotate(self)
 */
static void Editor_rotate (LIScrArgs* args)
{
	int x;
	int y;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EDITOR);
	module->client->video.SDL_GetMouseState (&x, &y);
	liext_editor_begin_rotate (module->editor, x, y);
}

/* @luadoc
 * ---
 * -- Snaps selected objects to grid.
 * --
 * -- Arguments:
 * -- grid: Grid spacing.
 * -- angle: Rotation step size in degrees.
 * --
 * -- @param self Editor class.
 * -- @param args Arguments.
 * function Editor.snap(self, args)
 */
static void Editor_snap (LIScrArgs* args)
{
	float angle = 0.0f;
	float grid = 1.0f;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EDITOR);
	liscr_args_gets_float (args, "angle", &angle);
	liscr_args_gets_float (args, "grid", &grid);
	liext_editor_snap (module->editor, grid, angle / 180.0f * M_PI);
}

/* @luadoc
 * ---
 * -- Begins a translation drag.
 * --
 * -- @param self Editor class.
 * function Editor.translate(self)
 */
static void Editor_translate (LIScrArgs* args)
{
	int x;
	int y;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EDITOR);
	module->client->video.SDL_GetMouseState (&x, &y);
	liext_editor_begin_translate (module->editor, x, y);
}

/* @luadoc
 * ---
 * -- Visibility of the editor window.
 * -- @name Editor.visible
 * -- @class table
 */
static void Editor_getter_visible (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EDITOR);
	liscr_args_seti_bool (args, liwdg_widget_get_visible (module->dialog));
}
static void Editor_setter_visible (LIScrArgs* args)
{
	int value;
	LIExtModule* module;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EDITOR);
		liwdg_widget_set_visible (module->dialog, value);
	}
}

/*****************************************************************************/

void
liext_script_editor (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EDITOR, data);
	liscr_class_insert_cfunc (self, "create", Editor_create);
	liscr_class_insert_cfunc (self, "rotate", Editor_rotate);
	liscr_class_insert_cfunc (self, "snap", Editor_snap);
	liscr_class_insert_cfunc (self, "translate", Editor_translate);
	liscr_class_insert_cvar (self, "visible", Editor_getter_visible, Editor_setter_visible);
}

/** @} */
/** @} */
/** @} */
