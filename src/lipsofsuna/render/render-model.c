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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-model.h"
#include "internal/render-internal.h"

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \return Model ID, or zero on failure.
 */
int liren_render_model_new (
	LIRenRender* render,
	LIMdlModel*  model)
{
	LIRenModel* self;

	self = liren_model_new (render, model, 0);
	if (self == NULL)
		return 0;

	return liren_model_get_id (self);
}

/**
 * \brief Frees the model.
 * \param self Renderer.
 * \param id Model ID.
 */
void liren_render_model_free (
	LIRenRender* self,
	int          id)
{
	LIRenModel* model;

	model = lialg_u32dic_find (self->models, id);
	if (model != NULL)
		liren_model_free (model);
}

/**
 * \brief Returns nonzero if the model has been fully background loaded.
 * \param self Renderer.
 * \param id Model ID.
 * \return Nonzero if background loaded, zero if not.
 */
int liren_render_model_get_loaded (
	LIRenRender* self,
	int          id)
{
	LIRenModel* model;

	model = lialg_u32dic_find (self->models, id);
	if (model != NULL)
		return liren_model_get_loaded (model);
	else
		return 1;
}

void liren_render_model_set_model (
	LIRenRender* self,
	int          id,
	LIMdlModel*  model)
{
	LIRenModel* model_;

	model_ = lialg_u32dic_find (self->models, id);
	if (model_ != NULL)
		liren_model_set_model (model_, model);
}

/** @} */
/** @} */
