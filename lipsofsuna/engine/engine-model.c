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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup LIEngModel Model
 * @{
 */

#include "engine-model.h"

LIEngModel*
lieng_model_new (LIEngEngine* engine,
                 const char*  dir,
                 const char*  name)
{
	LIEngModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngModel));
	if (self == NULL)
		return NULL;
	self->engine = engine;

	/* Allocate name. */
	self->name = listr_dup (name);
	if (self->name == NULL)
		goto error;

	/* Allocate path. */
	self->path = lisys_path_format (engine->config.dir,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, name, ".lmdl", NULL);
	if (self->path == NULL)
		goto error;

	/* Load data. */
	if (!lieng_model_load (self))
		goto error;

	return self;

error:
	lieng_model_free (self);
	return NULL;
}

LIEngModel*
lieng_model_new_copy (LIEngModel* model)
{
	LIEngModel* self;
	LIMdlModel* tmpmdl;
	LIPhyShape* tmpphy;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngModel));
	if (self == NULL)
		return NULL;
	self->engine = model->engine;

	/* Allocate info. */
	if (model->name != NULL)
	{
		self->name = listr_dup (model->name);
		if (self->name == NULL)
		{
			lieng_model_free (self);
			return NULL;
		}
	}
	if (model->path != NULL)
	{
		self->path = listr_dup (model->path);
		if (self->path == NULL)
		{
			lieng_model_free (self);
			return NULL;
		}
	}

	/* Load model geometry. */
	tmpmdl = limdl_model_new_copy (model->model);
	if (tmpmdl == NULL)
	{
		lieng_model_free (self);
		return 0;
	}

	/* Create collision shape. */
	tmpphy = liphy_shape_new (self->engine->physics, tmpmdl);
	if (tmpphy == NULL)
	{
		limdl_model_free (tmpmdl);
		lieng_model_free (self);
		return 0;
	}
	liphy_shape_ref (tmpphy);

	/* Set model and shape. */
	self->bounds = tmpmdl->bounds;
	self->model = tmpmdl;
	self->physics = tmpphy;

	return self;
}

void
lieng_model_free (LIEngModel* self)
{
	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "model-free", lical_marshal_DATA_PTR, self);

	/* Unreference and free shape. */
	if (self->physics != NULL)
	{
		liphy_shape_free (self->physics);
		liphy_shape_free (self->physics);
	}

	/* Free data. */
	if (self->model != NULL)
		limdl_model_free (self->model);
	lisys_free (self->path);
	lisys_free (self->name);
	lisys_free (self);
}

int
lieng_model_load (LIEngModel* self)
{
	LIMdlModel* tmpmdl;
	LIPhyShape* tmpphy;

	if (self->model != NULL)
		return 1;

	/* Load model geometry. */
	tmpmdl = limdl_model_new_from_file (self->path);
	if (tmpmdl == NULL)
		return 0;

	/* Create collision shape. */
	tmpphy = liphy_shape_new (self->engine->physics, tmpmdl);
	if (tmpphy == NULL)
	{
		limdl_model_free (tmpmdl);
		return 0;
	}
	liphy_shape_ref (tmpphy);

	/* Invoke callbacks. */
	lieng_model_unload (self);
	self->bounds = tmpmdl->bounds;
	self->model = tmpmdl;
	self->physics = tmpphy;
	lical_callbacks_call (self->engine->callbacks, self->engine, "model-new", lical_marshal_DATA_PTR, self);

	return 1;
}

void
lieng_model_unload (LIEngModel* self)
{
	/* Invoke callbacks. */
	if (self->model != NULL && self->physics != NULL)
		lical_callbacks_call (self->engine->callbacks, self->engine, "model-free", lical_marshal_DATA_PTR, self);

	if (self->physics != NULL)
	{
		liphy_shape_free (self->physics);
		self->physics = NULL;
	}
	if (self->model != NULL)
	{
		limdl_model_free (self->model);
		self->model = NULL;
	}
}

/**
 * \brief Gets the bounds of the model.
 *
 * \param self Module.
 * \param result Return location for the bounds.
 */
void
lieng_model_get_bounds (const LIEngModel* self,
                        LIMatAabb*        result)
{
	*result = self->bounds;
}

/**
 * \brief Calculates the bounds for the model with certain transformation.
 *
 * \param self Model.
 * \param transform Transformation.
 * \param result Return location for the bounds.
 */
void
lieng_model_get_bounds_transform (const LIEngModel*     self,
                                  const LIMatTransform* transform,
                                  LIMatAabb*            result)
{
	int i;
	LIMatVector v[7];
	LIMatVector min;
	LIMatVector max;

	/* Initialize vertices. */
	v[0] = limat_vector_init (self->bounds.min.x, self->bounds.min.y, self->bounds.max.z);
	v[1] = limat_vector_init (self->bounds.min.x, self->bounds.max.y, self->bounds.min.z);
	v[2] = limat_vector_init (self->bounds.min.x, self->bounds.max.y, self->bounds.max.z);
	v[3] = limat_vector_init (self->bounds.max.x, self->bounds.min.y, self->bounds.min.z);
	v[4] = limat_vector_init (self->bounds.max.x, self->bounds.min.y, self->bounds.max.z);
	v[5] = limat_vector_init (self->bounds.max.x, self->bounds.max.y, self->bounds.min.z);
	v[6] = limat_vector_init (self->bounds.max.x, self->bounds.max.y, self->bounds.max.z);

	/* Find minimum and maximum points. */
	min = max = limat_transform_transform (*transform, self->bounds.min);
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

/** @} */
/** @} */
