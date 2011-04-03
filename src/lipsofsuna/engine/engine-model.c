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

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngModel));
	if (self == NULL)
		return NULL;
	self->engine = engine;

	/* Create an empty model. */
	self->model = limdl_model_new ();
	if (self->model == NULL)
	{
		lieng_model_free (self);
		return NULL;
	}

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
	lical_callbacks_call (self->engine->callbacks, self->engine, "model-new", lical_marshal_DATA_PTR, self);

	return self;
}

LIEngModel* lieng_model_new_copy (
	LIEngModel* model)
{
	LIEngModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngModel));
	if (self == NULL)
		return NULL;
	self->engine = model->engine;

	/* Copy model geometry. */
	self->model = limdl_model_new_copy (model->model);
	if (self->model == NULL)
	{
		lieng_model_free (self);
		return 0;
	}

	/* Choose model number. */
	while (!self->id)
	{
		self->id = lialg_random_rand (&model->engine->random);
		if (!self->id)
			continue;
		if (lialg_u32dic_find (model->engine->models, self->id) != NULL)
			self->id = 0;
	}
	if (!lialg_u32dic_insert (model->engine->models, self->id, self))
	{
		lieng_model_free (self);
		return NULL;
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "model-new", lical_marshal_DATA_PTR_PTR, self, model);

	return self;
}

void lieng_model_free (
	LIEngModel* self)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;

	lialg_u32dic_remove (self->engine->models, self->id);

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "model-free", lical_marshal_DATA_PTR, self);

	/* Remove from objects. */
	/* Keeping the model alive when it's assigned to objects is the job of scripts.
	   If they don't reference the model, we'll remove it even if it's in use. We
	   prevent crashing by removing it from objects in such a case. */
	LIALG_U32DIC_FOREACH (iter, self->engine->objects)
	{
		object = iter.value;
		if (object->model == self)
			object->model = NULL;
	}

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
	char* path;
	LIMdlModel* tmpmdl;

	/* Allocate path. */
	file = listr_concat (name, ".lmdl");
	if (name == NULL)
		return 0;
	path = lipth_paths_get_graphics (self->engine->paths, file);
	free (file);
	if (path == NULL)
		return 0;

	/* Load model geometry. */
	tmpmdl = limdl_model_new_from_file (path, mesh);
	lisys_free (path);
	if (tmpmdl == NULL)
		return 0;

	/* Replace model data. */
	limdl_model_free (self->model);
	self->model = tmpmdl;
	private_changed (self);

	return 1;
}

/**
 * \brief Merges the mesh from a model to another.
 * \param self Destination model.
 * \param model Source model.
 * \return Nonzero on success.
 */
int lieng_model_merge (
	LIEngModel* self,
	LIEngModel* model)
{
	return limdl_model_merge (self->model, model->model);
}

/**
 * \brief Gets the bounds of the model.
 * \param self Module.
 * \param result Return location for the bounds.
 */
void lieng_model_get_bounds (
	const LIEngModel* self,
	LIMatAabb*        result)
{
	if (self->model != NULL)
		*result = self->model->bounds;
	else
		limat_aabb_init (result);
}

/**
 * \brief Calculates the bounds for the model with certain transformation.
 *
 * \param self Model.
 * \param transform Transformation.
 * \param result Return location for the bounds.
 */
void lieng_model_get_bounds_transform (
	const LIEngModel*     self,
	const LIMatTransform* transform,
	LIMatAabb*            result)
{
	int i;
	LIMatVector v[7];
	LIMatVector min;
	LIMatVector max;
	LIMatAabb bounds;

	/* Initialize vertices. */
	lieng_model_get_bounds (self, &bounds);
	v[0] = limat_vector_init (bounds.min.x, bounds.min.y, bounds.max.z);
	v[1] = limat_vector_init (bounds.min.x, bounds.max.y, bounds.min.z);
	v[2] = limat_vector_init (bounds.min.x, bounds.max.y, bounds.max.z);
	v[3] = limat_vector_init (bounds.max.x, bounds.min.y, bounds.min.z);
	v[4] = limat_vector_init (bounds.max.x, bounds.min.y, bounds.max.z);
	v[5] = limat_vector_init (bounds.max.x, bounds.max.y, bounds.min.z);
	v[6] = limat_vector_init (bounds.max.x, bounds.max.y, bounds.max.z);

	/* Find minimum and maximum points. */
	min = max = limat_transform_transform (*transform, bounds.min);
	for (i = 0 ; i < 7 ; i++)
	{
		v[i] = limat_transform_transform (*transform, v[i]);
		if (min.x > v[i].x)
			min.x = v[i].x;
		if (min.y > v[i].y)
			min.y = v[i].y;
		if (min.z > v[i].z)
			min.z = v[i].z;
		if (max.x < v[i].x)
			max.x = v[i].x;
		if (max.y < v[i].y)
			max.y = v[i].y;
		if (max.z < v[i].z)
			max.z = v[i].z;
	}
	limat_aabb_init_from_points (result, &min, &max);
}

/*****************************************************************************/

static void private_changed (
	LIEngModel* self)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;

	/* We need to refresh any objects using the model since poses reference
	   the nodes of the model directly and those might have changed. */
	LIALG_U32DIC_FOREACH (iter, self->engine->objects)
	{
		object = iter.value;
		if (object->model == self)
			limdl_pose_set_model (object->pose, self->model);
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "model-changed", lical_marshal_DATA_PTR, self);
}

/** @} */
/** @} */
