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
 * \addtogroup LIExtRender Render
 * @{
 */

#include "ext-module.h"

static void Model_get_render_loaded (LIScrArgs* args)
{
	LIExtModule* module;
	LIMdlModel* model = args->self;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RENDER_MODEL);
	if (liren_render_model_get_loaded (module->render, model->id))
		liscr_args_seti_bool (args, 1);
}

/*****************************************************************************/

void liext_script_render_model (
	LIScrScript* self)
{
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_MODEL, "model_get_render_loaded", Model_get_render_loaded);
}

/** @} */
/** @} */
