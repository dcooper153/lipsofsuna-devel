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
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* shader = NULL;
	const char* texture = NULL;
	LIExtModule* module;
	LIEngModel* engmdl;
	LIRenModel* model;
	LIRenMaterial* material;

	/* Get the render model. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_RENDER_MODEL);
	engmdl = args->self;
	model = liren_render_find_model (module->client->render, engmdl->id);
	if (model == NULL)
		return;

	/* Find the modified material. */
	liscr_args_gets_string (args, "match_shader", &shader);
	liscr_args_gets_string (args, "match_texture", &texture);
	material = liren_model_find_material (model, shader, texture);
	if (material == NULL)
		return;

	/* Edit the material properties. */
	if (liscr_args_gets_floatv (args, "diffuse", 4, color))
		liren_material_set_diffuse (material, color);
	if (liscr_args_gets_floatv (args, "specular", 4, color))
		liren_material_set_specular (material, color);
}

/* @luadoc
 * --- Vertex usage style. ("dynamic"/"static"/"stream")
 * -- @name Model.vertex_policy
 * -- @class table
 */
static void Model_getter_vertex_policy (LIScrArgs* args)
{
	LIExtModule* module;
	LIEngModel* engmdl;
	LIRenModel* model;

	/* Get render model. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_RENDER_MODEL);
	engmdl = args->self;
	model = liren_render_find_model (module->render, engmdl->id);
	if (model == NULL)
		return;

	/* Set buffer type. */
	switch (liren_model_get_type (model))
	{
		case LIREN_BUFFER_TYPE_DYNAMIC:
			liscr_args_seti_string (args, "dynamic");
			break;
		case LIREN_BUFFER_TYPE_STATIC:
			liscr_args_seti_string (args, "static");
			break;
		case LIREN_BUFFER_TYPE_STREAM:
			liscr_args_seti_string (args, "stream");
			break;
	}
}
static void Model_setter_vertex_policy (LIScrArgs* args)
{
	const char* value;
	LIExtModule* module;
	LIEngModel* engmdl;
	LIRenModel* model;

	/* Get render model. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_RENDER_MODEL);
	engmdl = args->self;
	model = liren_render_find_model (module->render, engmdl->id);
	if (model == NULL)
		return;

	/* Set buffer type. */
	if (!liscr_args_geti_string (args, 0, &value))
		return;
	if (!strcmp (value, "dynamic"))
		liren_model_set_type (model, LIREN_BUFFER_TYPE_DYNAMIC);
	else if (!strcmp (value, "static"))
		liren_model_set_type (model, LIREN_BUFFER_TYPE_STATIC);
	else if (!strcmp (value, "stream"))
		liren_model_set_type (model, LIREN_BUFFER_TYPE_STREAM);
}

/*****************************************************************************/

void liext_script_render_model (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_RENDER_MODEL, data);
	liscr_class_insert_mfunc (self, "edit_material", Model_edit_material);
	liscr_class_insert_mvar (self, "vertex_policy", Model_getter_vertex_policy, Model_setter_vertex_policy);
}

/** @} */
/** @} */
