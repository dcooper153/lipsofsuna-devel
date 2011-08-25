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

#include "lipsofsuna/network.h"
#include "lipsofsuna/system.h"
#include "render-model.h"
#include "render-private.h"
#include "render32/render-private.h"

static void private_free (
	LIRenModel* self);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 * \return Model ID or zero on failure.
 */
int liren_render_model_new (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id)
{
	LIRenModel* self;

	self = lisys_calloc (1, sizeof (LIRenModel));
	if (self == NULL)
		return 0;
	self->id = id;
	self->render = render;

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_model32_new (render->v32, model, id);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return 0;
		}
	}
	else
	{
		self->v21 = liren_model21_new (render->v21, model, id);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return 0;
		}
	}

	/* Find a free model ID. */
	if (!id)
	{
		do
		{
			id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		}
		while (lialg_u32dic_find (render->models, id) != NULL);
		self->id = id;
	}

	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->models, id, self))
	{
		private_free (self);
		return 0;
	}

	return id;
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
	if (model == NULL)
		return;

	private_free (model);
}

int liren_render_model_set_model (
	LIRenRender* self,
	int          id,
	LIMdlModel*  model)
{
	LIRenModel* model_;

	model_ = lialg_u32dic_find (self->models, id);
	if (model_ == NULL)
		return 0;

	if (model_->v32 != NULL)
		return liren_model32_set_model (model_->v32, model);
	else
		return liren_model21_set_model (model_->v21, model);
}

/*****************************************************************************/

static void private_free (
	LIRenModel* self)
{
	lialg_u32dic_remove (self->render->models, self->id);
	if (self->v32 != NULL)
		liren_model32_free (self->v32);
	if (self->v21 != NULL)
		liren_model21_free (self->v21);
	lisys_free (self);
}

/** @} */
/** @} */
