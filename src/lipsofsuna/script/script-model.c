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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrObject Object
 * @{
 */

#include <lipsofsuna/engine.h>
#include <lipsofsuna/main.h>
#include <lipsofsuna/script.h>

/* @luadoc
 * module "builtin/model"
 * ---
 * -- Load and manipulate models.
 * -- @name Model
 * -- @class table
 */

/* @luadoc
 * --- Recalculates the bounding box of the model.
 * -- @param self Model.
 * function Model.calculate_bounds(self)
 */
static void Model_calculate_bounds (LIScrArgs* args)
{
	lieng_model_calculate_bounds (args->self);
}

/* @luadoc
 * --- Creates a copy of the model.
 * -- @param self Model.
 * -- @param args Arguments.
 * -- @return New model.
 * function model.copy(self, args)
 */
static void Model_copy (LIScrArgs* args)
{
	LIEngModel* self;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);

	/* Allocate model. */
	self = lieng_model_new_copy (args->self);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, self, args->clss, lieng_model_free);
	if (self->script == NULL)
	{
		lieng_model_free (self);
		return;
	}

	/* Initialize userdata. */
	liscr_args_call_setters (args, self->script);
	liscr_args_seti_data (args, self->script);
	liscr_data_unref (self->script, NULL);
}

/* @luadoc
 * --- Adds an additional model mesh to the model.
 * -- @param self Model.
 * -- @param args Arguments.<ul>
 * --   <li>1,model: Model. (required)</li></ul>
 * function Model.merge(self, args)
 */
static void Model_merge (LIScrArgs* args)
{
	LIScrData* model;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &model) ||
	    liscr_args_gets_data (args, "model", LISCR_SCRIPT_MODEL, &model))
	{
		if (!lieng_model_merge (args->self, model->data))
			lisys_error_report ();
	}
}

/* @luadoc
 * --- Creates a new model.
 * --
 * -- @param clss Model class.
 * -- @param args Arguments.
 * -- @return New model.
 * function Model.new(clss, args)
 */
static void Model_new (LIScrArgs* args)
{
	LIEngModel* self;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);

	/* Allocate model. */
	self = lieng_model_new (program->engine);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, self, args->clss, lieng_model_free);
	if (self->script == NULL)
	{
		lieng_model_free (self);
		return;
	}

	/* Initialize userdata. */
	liscr_args_call_setters (args, self->script);
	liscr_args_seti_data (args, self->script);
	liscr_data_unref (self->script, NULL);
}

/* @luadoc
 * --- Loads the model from a file.
 * -- @name Model.file
 * -- @class table
 */
static void Model_setter_file (LIScrArgs* args)
{
	const char* file;

	if (liscr_args_geti_string (args, 0, &file))
	{
		if (!lieng_model_load (args->self, file))
			lisys_error_report ();
	}
}

/* @luadoc
 * --- Unique ID of the model.
 * -- @name Model.id
 * -- @class table
 */
static void Model_getter_id (LIScrArgs* args)
{
	LIEngModel* self;

	self = args->self;
	liscr_args_seti_int (args, self->id);
}

/*****************************************************************************/

void liscr_script_model (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_OBJECT, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_mfunc (self, "calculate_bounds", Model_calculate_bounds);
	liscr_class_insert_mfunc (self, "copy", Model_copy);
	liscr_class_insert_mfunc (self, "merge", Model_merge);
	liscr_class_insert_cfunc (self, "new", Model_new);
	liscr_class_insert_mvar (self, "file", NULL, Model_setter_file);
	liscr_class_insert_mvar (self, "id", Model_getter_id, NULL);
}

/** @} */
/** @} */
