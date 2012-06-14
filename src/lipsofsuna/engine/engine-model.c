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
 * \addtogroup LIEng Engine
 * @{
 * \addtogroup LIEngModel Model
 * @{
 */

#include "engine-model.h"

static void private_changed (
	LIEngModel* self);

/*****************************************************************************/

LIEngModel* lieng_model_new (
	LIEngEngine* engine)
{
	LIEngModel* self;
	LIMdlModel* model;

	/* Create an empty model. */
	model = limdl_model_new ();
	if (model == NULL)
		return NULL;

	/* Allocate self. */
	self = lieng_model_new_model (engine, model);
	if (self == NULL)
	{
		limdl_model_free (model);
		return NULL;
	}

	return self;
}

LIEngModel* lieng_model_new_copy (
	LIEngModel* model,
	int         shape_keys)
{
	LIEngModel* self;
	LIMdlModel* model_;

	/* Copy the model. */
	model_ = limdl_model_new_copy (model->model, shape_keys);
	if (model_ == NULL)
		return NULL;

	/* Allocate self. */
	self = lieng_model_new_model (model->engine, model_);
	if (self == NULL)
	{
		limdl_model_free (model_);
		return NULL;
	}

	return self;
}

LIEngModel* lieng_model_new_model (
	LIEngEngine* engine,
	LIMdlModel*  model)
{
	LIEngModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngModel));
	if (self == NULL)
		return NULL;
	self->engine = engine;
	self->model = model;

	/* Choose model number. */
	while (!self->id)
	{
		self->id = lialg_random_rand (&engine->random);
		if (!self->id)
			continue;
		if (lialg_u32dic_find (engine->models, self->id) != NULL)
			self->id = 0;
	}
	if (!lialg_u32dic_insert (engine->models, self->id, self))
	{
		lieng_model_free (self);
		return NULL;
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, "model-new", lical_marshal_DATA_PTR, self);

	return self;
}

void lieng_model_free (
	LIEngModel* self)
{
	/* Remove from the dictionary. */
	lialg_u32dic_remove (self->engine->models, self->id);

	/* Invoke callbacks. */
	/* This will, among other things, cause the object manager to remove
	   the model from each object that was using it. */
	lical_callbacks_call (self->engine->callbacks, "model-free", lical_marshal_DATA_PTR, self);

	/* Free data. */
	if (self->model != NULL)
		limdl_model_free (self->model);
	lisys_free (self);
}

/**
 * \brief Emits a model change event.
 *
 * Calling this causes any extensions that use the model to update their
 * copies of it. Notably, the renderer extension requires you to call this
 * after doing custom modifications to the model that you need to display.
 *
 * \param self Model.
 */
void lieng_model_changed (
	LIEngModel* self)
{
	if (self->model != NULL)
		private_changed (self);
}

void lieng_model_calculate_bounds (
	LIEngModel* self)
{
	if (self->model != NULL)
		limdl_model_calculate_bounds (self->model);
}

int lieng_model_load (
	LIEngModel* self,
	const char* name,
	int         mesh)
{
	char* file;
	const char* path;
	LIMdlModel* tmpmdl;

	/* Allocate path. */
	file = lisys_string_concat (name, ".lmdl");
	if (file == NULL)
		return 0;
	path = lipth_paths_find_file (self->engine->paths, file);
	if (path == NULL)
	{
		lisys_error_set (EINVAL, "cannot find model `%s'", file);
		lisys_free (file);
		return 0;
	}
	lisys_free (file);

	/* Load model geometry. */
	tmpmdl = limdl_model_new_from_file (path, mesh);
	if (tmpmdl == NULL)
		return 0;

	/* Replace model data. */
	limdl_model_free (self->model);
	self->model = tmpmdl;
	private_changed (self);

	return 1;
}

/*****************************************************************************/

static void private_changed (
	LIEngModel* self)
{
	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, "model-changed", lical_marshal_DATA_PTR, self);
}

/** @} */
/** @} */
