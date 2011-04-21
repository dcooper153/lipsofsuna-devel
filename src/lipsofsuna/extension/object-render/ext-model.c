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

/* @luadoc
 * module "core/object-render"
 * ---
 * -- Control rendering of models.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * --- Edits the matching material.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>diffuse: Diffuse color to set or nil.</li>
 * --   <li>match_shader: Shader name to match or nil.</li>
 * --   <li>match_texture: Texture name to match or nil.</li>
 * --   <li>specular: Specular color to set or nil.</li>
 * function Model.edit_material(self, args)
 */
static void Model_edit_material (LIScrArgs* args)
{
	int i;
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* shader = NULL;
	const char* texture = NULL;
	LIExtModule* module;
	LIEngModel* model;
	LIMdlMaterial* material;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_RENDER_MODEL);
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

/* @luadoc
 * --- Morphs a model with one of its shape keys.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>1,shape: Shape key name.</li>
 * --   <li>2,value: Shape influence name to match or nil.</li></ul>
 * function Model.morph(self, args)
 */
static void Model_morph (LIScrArgs* args)
{
	float value = 0.5f;
	const char* shape;
	LIEngModel* model;

	model = args->self;
	if (!liscr_args_geti_string (args, 0, &shape) &&
	    !liscr_args_gets_string (args, "shape", &shape))
		return;
	if (!liscr_args_geti_float (args, 1, &value))
		liscr_args_gets_float (args, "value", &value);

	liscr_args_seti_bool (args, limdl_model_morph (model->model, shape, value));
}

/*****************************************************************************/

void liext_script_render_model (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_RENDER_MODEL, data);
	liscr_class_insert_mfunc (self, "edit_material", Model_edit_material);
	liscr_class_insert_mfunc (self, "morph", Model_morph);
}

/** @} */
/** @} */
