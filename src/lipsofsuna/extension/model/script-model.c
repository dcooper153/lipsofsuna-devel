/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtModel Model
 * @{
 */

#include "lipsofsuna/main.h"
#include "lipsofsuna/script.h"

static void Model_new (LIScrArgs* args)
{
	LIMaiProgram* program;
	LIMdlModel* self;
	LIScrData* data;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);

	/* Allocate the model. */
	self = limdl_model_new ();
	if (self == NULL)
		return;

	/* Allocate the unique ID. */
	if (!limdl_manager_add_model (program->models, self))
	{
		limdl_model_free (self);
		return;
	}

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LISCR_SCRIPT_MODEL, limdl_manager_free_model);
	if (data == NULL)
	{
		limdl_model_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Model_copy (LIScrArgs* args)
{
	int shape_keys = 1;
	LIMaiProgram* program;
	LIMdlModel* self;
	LIScrData* data;

	/* Get arguments. */
	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_geti_bool (args, 0, &shape_keys);

	/* Allocate the model. */
	self = limdl_model_new_copy (args->self, shape_keys);
	if (self == NULL)
		return;

	/* Allocate the unique ID. */
	if (!limdl_manager_add_model (program->models, self))
	{
		limdl_model_free (self);
		return;
	}

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LISCR_SCRIPT_MODEL, limdl_manager_free_model);
	if (data == NULL)
	{
		limdl_model_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Model_calculate_bounds (LIScrArgs* args)
{
	limdl_model_calculate_bounds (args->self);
}

static void Model_changed (LIScrArgs* args)
{
	LIMaiProgram* program;

	/* Invoke callbacks. */
	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	lical_callbacks_call (program->callbacks, "model-changed", lical_marshal_DATA_PTR, args->self);
}

static void Model_load (LIScrArgs* args)
{
	int mesh = 1;
	char* file;
	const char* name;
	const char* path;
	LIMdlModel* tmpmdl;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (!liscr_args_geti_string (args, 0, &name) &&
	    !liscr_args_gets_string (args, "file", &name))
		return;
	if (!liscr_args_geti_bool (args, 1, &mesh))
		liscr_args_gets_bool (args, "mesh", &mesh);

	/* Find the absolute path. */
	file = lisys_string_concat (name, ".lmdl");
	if (file == NULL)
		return;
	path = lipth_paths_find_file (program->paths, file);
	if (path == NULL)
	{
		lisys_free (file);
		return;
	}
	lisys_free (file);

	/* Load the new model data. */
	tmpmdl = limdl_model_new_from_file (path, mesh);
	if (tmpmdl == NULL)
		return;

	/* Replace the old model data. */
	if (limdl_model_replace (args->self, tmpmdl))
		liscr_args_seti_bool (args, 1);
	limdl_model_free (tmpmdl);
}

static void Model_get_bounding_box (LIScrArgs* args)
{
	LIMatVector min;
	LIMatVector max;
	LIMdlModel* self;

	self = args->self;
	min = self->bounds.min;
	max = self->bounds.max;
	liscr_args_seti_vector (args, &min);
	liscr_args_seti_vector (args, &max);
}

static void Model_get_center_offset (LIScrArgs* args)
{
	LIMatVector ctr;
	LIMdlModel* self;

	self = args->self;
	ctr = limat_vector_add (self->bounds.min, self->bounds.max);
	ctr = limat_vector_multiply (ctr, 0.5f);
	liscr_args_seti_vector (args, &ctr);
}

static void Model_get_memory_used (LIScrArgs* args)
{
	LIMdlModel* self;

	self = args->self;
	liscr_args_seti_int (args, limdl_model_get_memory (self));
}

static void Model_get_vertex_count (LIScrArgs* args)
{
	LIMdlModel* self;

	self = args->self;
	liscr_args_seti_int (args, self->vertices.count);
}

static void Model_get_total_model_count (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_int (args, program->models->models->size);
}

static void Model_get_total_memory_used (LIScrArgs* args)
{
	int total;
	LIMaiProgram* program;
	LIAlgU32dicIter iter;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);

	total = 0;
	LIALG_U32DIC_FOREACH (iter, program->models->models)
		total += limdl_model_get_memory (iter.value);
	liscr_args_seti_int (args, total);
}

/*****************************************************************************/

void liext_script_model (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_MODEL, "model_new", Model_new);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_copy", Model_copy);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_calculate_bounds", Model_calculate_bounds);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_changed", Model_changed);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_load", Model_load);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_bounding_box", Model_get_bounding_box);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_center_offset", Model_get_center_offset);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_memory_used", Model_get_memory_used);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_vertex_count", Model_get_vertex_count);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_MODEL, "model_get_total_model_count", Model_get_total_model_count);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_MODEL, "model_get_total_memory_used", Model_get_total_memory_used);
}

/** @} */
/** @} */
