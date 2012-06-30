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
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyModel Model
 * @{
 */

#include "lipsofsuna/system.h"
#include "physics-model.h"
#include "physics-private.h"
#include "physics-shape.h"

static int private_build_object (
	LIPhyModel*       self,
	const LIMdlModel* model);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The caller must ensure that the model data remains valid until the physics
 * model is freed.
 *
 * \param physics Physics engine.
 * \param id Unique model ID.
 * \return New model or NULL.
 */
LIPhyModel* liphy_model_new (
	LIPhyPhysics* physics,
	int           id)
{
	LIPhyModel* self;

	/* Allocate self. */
	self = (LIPhyModel*) lisys_calloc (1, sizeof (LIPhyModel));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->id = id;

	/* Allocate shape dictionary. */
	self->shapes = lialg_strdic_new ();
	if (self->shapes == NULL)
	{
		liphy_model_free (self);
		return NULL;
	}

	/* Add to dictionary. */
	if (!lialg_u32dic_insert (physics->models, id, self))
	{
		liphy_model_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the model.
 * \param self Model.
 */
void liphy_model_free (LIPhyModel* self)
{
	LIAlgStrdicIter iter;
	LIPhyShape* shape;

	/* Bullet can't handle objects being deleted during the simulation tick.
	   To avoid problems, the code should never get here during it. */
	lisys_assert (!self->physics->updating);

	/* Remove from dictionary. */
	lialg_u32dic_remove (self->physics->models, self->id);

	/* Free object shapes. */
	if (self->shapes != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->shapes)
		{
			shape = (LIPhyShape*) iter.value;
			liphy_shape_free (shape);
		}
		lialg_strdic_free (self->shapes);
	}

	lisys_free (self);
}

LIPhyShape* liphy_model_find_shape (
	LIPhyModel* self,
	const char* name)
{
	return (LIPhyShape*) lialg_strdic_find (self->shapes, name);
}

int liphy_model_set_model (
	LIPhyModel*       self,
	const LIMdlModel* model)
{
	return private_build_object (self, model);
}

/*****************************************************************************/

static int private_build_object (
	LIPhyModel*       self,
	const LIMdlModel* model)
{
	int i;
	int create;
	LIAlgStrdicIter iter;
	LIPhyShape* physhape;
	LIMatAabb bounds;
	LIMatVector center;
	LIMdlShape* mdlshape;

	/* Clear old shapes. */
	/* We can't free the shapes because they may still be used by objects.
	   Clearing works fine since the shapes retain their pointers. */
	LIALG_STRDIC_FOREACH (iter, self->shapes)
	{
		physhape = (LIPhyShape*) iter.value;
		liphy_shape_clear (physhape);
	}

	/* Create explicit model shapes. */
	for (i = 0 ; i < model->shapes.count ; i++)
	{
		/* Find or create the physics shape. */
		mdlshape = model->shapes.array + i;
		physhape = (LIPhyShape*) lialg_strdic_find (self->shapes, mdlshape->name);
		if (physhape == NULL)
		{
			create = 1;
			physhape = liphy_shape_new (self->physics);
			if (physhape == NULL)
				return 0;
		}
		else
			create = 0;

		/* Set the center of mass. */
		/* This must be done before adding any model shapes. */
		limdl_shape_get_center (mdlshape, &center);
		liphy_shape_set_center_of_mass (physhape, &center);

		/* Add the model shape to the physics shape. */
		if (!liphy_shape_add_model_shape (physhape, mdlshape, NULL, 1.0f))
		{
			liphy_shape_free (physhape);
			return 0;
		}

		/* Add the shape to the dictionary. */
		if (create)
		{
			if (!lialg_strdic_insert (self->shapes, mdlshape->name, physhape))
			{
				liphy_shape_free (physhape);
				return 0;
			}
		}
	}

	/* Create a fallback shape if necessary. */
	/* Ideally all model files would have a shape in them but that isn't the case yet. */
	if (!model->shapes.count)
	{
		/* Find or create the physics shape. */
		mdlshape = model->shapes.array + i;
		physhape = (LIPhyShape*) lialg_strdic_find (self->shapes, "default");
		if (physhape == NULL)
		{
			create = 1;
			physhape = liphy_shape_new (self->physics);
			if (physhape == NULL)
				return 0;
		}
		else
			create = 0;

		/* Use the bounding box of the model. */
		bounds = model->bounds;

		/* Set the center of mass. */
		/* This must be done before adding any model shapes. */
		limat_aabb_get_center (&bounds, &center);
		liphy_shape_set_center_of_mass (physhape, &center);

		/* Add the model mesh to the physics shape. */
		if (!liphy_shape_add_aabb (physhape, &bounds, NULL))
		{
			liphy_shape_free (physhape);
			return 0;
		}

		/* Add the shape to the dictionary. */
		if (create)
		{
			if (!lialg_strdic_insert (self->shapes, "default", physhape))
			{
				liphy_shape_free (physhape);
				return 0;
			}
		}
	}

	return 1;
}

/** @} */
/** @} */
