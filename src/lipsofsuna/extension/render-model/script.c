/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIExtRender-model Render-model
 * @{
 */

#include "module.h"

static void RenderModel_new (LIScrArgs* args)
{
	LIExtRenderModel* self;
	LIExtRenderModelModule* module;
	LIMdlModel* model;
	LIScrData* data;

	/* Find the class data. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_MODEL);

	/* Get the model data. */
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &data))
		return;
	model = liscr_data_get_data (data);

	/* Allocate the model. */
	self = liext_render_model_new (module);
	if (self == NULL)
		return;
	liren_render_model_set_model (self->render, self->id, model);

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_RENDER_MODEL, liext_render_model_free);
	if (data == NULL)
	{
		liext_render_model_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void RenderModel_get_loaded (LIScrArgs* args)
{
	LIExtRenderModel* self;

	self = args->self;
	if (liren_render_model_get_loaded (self->render, self->id))
		liscr_args_seti_bool (args, 1);
}

static void RenderModel_set_model (LIScrArgs* args)
{
	LIMdlModel* model;
	LIExtRenderModel* self;
	LIScrData* value;

	self = args->self;
	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &value))
	{
		model = liscr_data_get_data (value);
		liren_render_model_set_model (self->render, self->id, model);
	}
}

/*****************************************************************************/

void liext_script_render_model (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RENDER_MODEL, "render_model_new", RenderModel_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_MODEL, "render_model_get_loaded", RenderModel_get_loaded);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_RENDER_MODEL, "render_model_set_model", RenderModel_set_model);
}

/** @} */
/** @} */
