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
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include "lipsofsuna/network.h"
#include "render-internal.h"

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 * \return Model or NULL on failure.
 */
LIRenModel* liren_model_new (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id)
{
	LIRenModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenModel));
	if (self == NULL)
		return 0;
	self->render = render;

	/* Choose a unique ID. */
	while (!id)
	{
		id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (render->objects, id))
			id = 0;
	}
	self->id = id;

	/* Copy the model. */
	if (model != NULL)
	{
		self->model = limdl_model_new_copy (model);
		if (self->model == NULL)
		{
			lisys_free (self);
			return 0;
		}
	}

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_model32_new (render->v32, model, id);
		if (self->v32 == NULL)
		{
			limdl_model_free (self->model);
			lisys_free (self);
			return 0;
		}
	}
	else
	{
		self->v21 = liren_model21_new (render->v21, model, id);
		if (self->v21 == NULL)
		{
			limdl_model_free (self->model);
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
		liren_model_free (self);
		return 0;
	}

	return self;
}

void liren_model_free (
	LIRenModel* self)
{
	/* Remove from objects. */
	/* Keeping the model alive when it's assigned to objects is the job of scripts.
	   If they don't reference the model, we'll remove it even if it's in use. We
	   prevent crashing by removing it from objects in such a case. */
	if (self->v32 != NULL)
		liren_render32_remove_model (self->render->v32, self->v32);
	else
		liren_render21_remove_model (self->render->v21, self->v21);

	/* Free the model data. */
	if (self->model != NULL)
		limdl_model_free (self->model);

	lialg_u32dic_remove (self->render->models, self->id);
	if (self->v32 != NULL)
		liren_model32_free (self->v32);
	if (self->v21 != NULL)
		liren_model21_free (self->v21);
	lisys_free (self);
}

int liren_model_set_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	LIMdlModel* copy;

	/* Copy the model. */
	copy = limdl_model_new_copy (model);
	if (copy == NULL)
		return 0;
	if (self->model != NULL)
		limdl_model_free (self->model);
	self->model = copy;

	if (self->v32 != NULL)
		return liren_model32_set_model (self->v32, self->model);
	else
		return liren_model21_set_model (self->v21, self->model);
}

/** @} */
/** @} */
/** @} */
