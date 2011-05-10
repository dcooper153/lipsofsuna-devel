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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "ext-module.h"

static void Model_edit_material (LIScrArgs* args)
{
	int i;
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* shader = NULL;
	const char* texture = NULL;
	LIExtModule* module;
	LIEngModel* model;
	LIMdlMaterial* material;

	/* Get the engine model. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_MODEL);
	model = args->self;
	liscr_args_gets_string (args, "match_shader", &shader);
	liscr_args_gets_string (args, "match_texture", &texture);

	/* Edit each matching material. */
	for (i = 0 ; i < model->model->materials.count ; i++)
	{
		material = model->model->materials.array + i;
		if (!limdl_material_compare_shader_and_texture (material, shader, texture))
			continue;
		if (liscr_args_gets_floatv (args, "diffuse", 4, color))
			limdl_material_set_diffuse (material, color);
		if (liscr_args_gets_floatv (args, "specular", 4, color))
			limdl_material_set_specular (material, color);
	}
}

static void Model_morph (LIScrArgs* args)
{
	float value = 0.5f;
	const char* shape;
	LIEngModel* model;
	LIEngModel* ref = NULL;
	LIScrData* data;

	model = args->self;
	if (!liscr_args_geti_string (args, 0, &shape) &&
	    !liscr_args_gets_string (args, "shape", &shape))
		return;
	if (!liscr_args_geti_float (args, 1, &value))
		liscr_args_gets_float (args, "value", &value);
	if (!liscr_args_geti_data (args, 2, LISCR_SCRIPT_MODEL, &data))
		liscr_args_gets_data (args, "ref", LISCR_SCRIPT_MODEL, &data);
	if (data != NULL)
		ref = liscr_data_get_data (data);

	liscr_args_seti_bool (args, limdl_model_morph (model->model,
		(ref != NULL)? ref->model : NULL, shape, value));
}

/*****************************************************************************/

void liext_script_render_model (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_edit_material", Model_edit_material);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_morph", Model_morph);
}

/** @} */
/** @} */
