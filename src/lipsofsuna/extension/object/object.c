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
 * \addtogroup LIEng Engine
 * @{
 * \addtogroup LIObjObject Object
 * @{
 */

#include "lipsofsuna/network.h"
#include "object.h"
#include "object-sector.h"

static int private_warp (
	LIObjObject*       self,
	const LIMatVector* position);

/*****************************************************************************/

/**
 * \brief Creates a new object.
 * \param manager Object manager.
 * \return Object, or NULL.
 */
LIObjObject* liobj_object_new (
	LIObjManager* manager)
{
	LIObjObject* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIObjObject));
	if (self == NULL)
		return NULL;
	self->manager = manager;
	self->transform = limat_transform_identity ();

	/* Choose object number. */
	self->id = 0;
	while (!self->id)
	{
		self->id = lialg_random_range (&manager->program->engine->random, LINET_RANGE_ENGINE_START, LINET_RANGE_ENGINE_END);
		if (liobj_manager_find_object (manager, self->id))
			self->id = 0;
	}

	/* Insert to object list. */
	if (!lialg_u32dic_insert (manager->objects, self->id, self))
	{
		lisys_free (self);
		return 0;
	}

	/* Invoke program->callbacks. */
	lical_callbacks_call (manager->program->callbacks, "object-new", lical_marshal_DATA_PTR, self);

	return self;
}

/**
 * \brief Frees the object.
 * \param self Object.
 */
void liobj_object_free (
	LIObjObject* self)
{
	/* Unrealize. */
	/* When the execution gets here, the script data of the object has been
	   garbage collected. Hence, we need to be careful not to generate any
	   script events that refer to it. */
	if (liobj_object_get_realized (self))
		liobj_object_set_realized (self, 0);

	/* Invoke program->callbacks. */
	lical_callbacks_call (self->manager->program->callbacks, "object-free", lical_marshal_DATA_PTR, self);

	/* Remove from the manager. */
	lialg_u32dic_remove (self->manager->objects, self->id);

	/* Free all memory. */
	lisys_free (self);
}

/**
 * \brief Called when the object has moved.
 * \param self Object.
 */
int liobj_object_moved (
	LIObjObject* self)
{
	LIMatTransform transform;

	/* Move between sectors. */
	liobj_object_get_transform (self, &transform);
	if (!private_warp (self, &transform.position))
		return 0;

	/* Invoke program->callbacks. */
	lical_callbacks_call (self->manager->program->callbacks, "object-motion", lical_marshal_DATA_PTR, self);

	return 1;
}

/**
 * \brief Refreshes map around the object.
 *
 * Calling this function prevents the map sectors near the object from
 * unloading. One important use case for this is preventing clients from
 * being swapped out when they stand still.
 *
 * \param self Object.
 * \param radius Refresh radius.
 */
void liobj_object_refresh (
	LIObjObject* self,
	float        radius)
{
	LIMatTransform transform;

	if (liobj_object_get_realized (self))
	{
		liobj_object_get_transform (self, &transform);
		lialg_sectors_refresh_point (self->manager->program->sectors, &transform.position, radius);
	}
}

/**
 * \brief Gets the bounding box size of the object.
 * \param self Object.
 * \param bounds Return location for the bounding box.
 */
void liobj_object_get_bounds (
	const LIObjObject* self,
	LIMatAabb*         bounds)
{
	if (self->model != NULL && self->model->model != NULL)
		*bounds = self->model->model->bounds;
	else
		limat_aabb_init (bounds);
}

/**
 * \brief Gets the distance between the objects.
 *
 * If either of the objects is in not realized, LIMAT_INFINITE is returned.
 *
 * \param self An object.
 * \param object An object.
 * \return The distance.
 */
float liobj_object_get_distance (
	const LIObjObject* self,
	const LIObjObject* object)
{
	LIMatTransform t0;
	LIMatTransform t1;

	if (!liobj_object_get_realized (self) ||
	    !liobj_object_get_realized (object))
		return LIMAT_INFINITE;
	liobj_object_get_transform (self, &t0);
	liobj_object_get_transform (object, &t1);

	return limat_vector_get_length (limat_vector_subtract (t0.position, t1.position));
}

/**
 * \brief Gets the external ID of the object.
 * \param self Object.
 * \return ID.
 */
int liobj_object_get_external_id (
	const LIObjObject* self)
{
	return self->external_id;
}

/**
 * \brief Sets the external ID of the object.
 * \param self Object.
 * \param value ID.
 */
void liobj_object_set_external_id (
	LIObjObject* self,
	int          value)
{
	self->external_id = value;
}

/**
 * \brief Replaces the current model of the object.
 * \param self Object.
 * \param model Model or NULL.
 * \return Nonzero on success.
 */
int liobj_object_set_model (
	LIObjObject* self,
	LIEngModel*  model)
{
	/* Switch the model. */
	self->model = model;

	/* Invoke program->callbacks. */
	lical_callbacks_call (self->manager->program->callbacks, "object-model", lical_marshal_DATA_PTR_PTR, self, model);

	return 1;
}

/**
 * \brief Checks if the object is added to the simulation.
 * \param self Object.
 * \return Nonzero if realized.
 */
int liobj_object_get_realized (
	const LIObjObject* self)
{
	return (self->flags & LIENG_OBJECT_FLAG_REALIZED) != 0;
}

/**
 * \brief Realizes or unrealizes the object.
 *
 * Unrealized objects don't affect the physics simulation and they
 * are, by default, invisible to clients and don't emit events.
 * Realized objects, on the other hand, contribute to the physics
 * simulation and emit events on state changes.
 *
 * Objects are created unrealized by the engine, but you can realize
 * them with this function if you want to attach them to the world
 * map. Likewise, you can unrealize them if you'd like to remove them
 * from the map.
 *
 * \param self Object.
 * \param value Nonzero if the object should be realized.
 * \return Nonzero on success.
 */
int liobj_object_set_realized (
	LIObjObject* self,
	int          value)
{
	LIMatTransform transform;

	if (value == liobj_object_get_realized (self))
		return 1;
	if (value)
	{
		/* Link to the map. */
		liobj_object_get_transform (self, &transform);
		self->flags |= LIENG_OBJECT_FLAG_REALIZED;
		if (!private_warp (self, &transform.position))
		{
			self->flags &= ~LIENG_OBJECT_FLAG_REALIZED;
			return 0;
		}

		/* Invoke program->callbacks. */
		lical_callbacks_call (self->manager->program->callbacks, "object-visibility", lical_marshal_DATA_PTR_INT, self, 1);
	}
	else
	{
		/* Invoke program->callbacks. */
		lical_callbacks_call (self->manager->program->callbacks, "object-visibility", lical_marshal_DATA_PTR_INT, self, 0);

		/* Remove from the map. */
		if (self->sector != NULL)
		{
			lialg_u32dic_remove (self->sector->objects, self->id);
			self->sector = NULL;
		}
		self->flags &= ~LIENG_OBJECT_FLAG_REALIZED;
	}

	return 1;
}

/**
 * \brief Returns the current sector of the object.
 * \param self Object.
 * \return Sector or NULL.
 */
LIObjSector* liobj_object_get_sector (
	LIObjObject* self)
{
	return self->sector;
}

/**
 * \brief Returns nonzero if the object is static.
 * \param self Object.
 * \return Nonzero if static.
 */
int liobj_object_get_static (
	const LIObjObject* self)
{
	return (self->flags & LIENG_OBJECT_FLAG_STATIC) != 0;
}

/**
 * \brief Sets whether the object is static.
 * \param self Object.
 * \param value Nonzero for static.
 */
void liobj_object_set_static (
	LIObjObject* self,
	int          value)
{
	LIMatTransform transform;

	if (value)
	{
		self->flags |= LIENG_OBJECT_FLAG_STATIC;
		if (self->sector != NULL)
		{
			lialg_u32dic_remove (self->sector->objects, self->id);
			self->sector = NULL;
		}
	}
	else
	{
		self->flags &= ~LIENG_OBJECT_FLAG_STATIC;
		liobj_object_get_transform (self, &transform);
		private_warp (self, &transform.position);
	}
}

/**
 * \brief Gets the world space transformation of the object.
 * \param self Object.
 * \param value Return location for the transformation.
 */
void liobj_object_get_transform (
	const LIObjObject* self,
	LIMatTransform*    value)
{
	*value = self->transform;
}

/**
 * \brief Sets the world space transformation of the object.
 * \param self Object.
 * \param value Transformation.
 * \return Nonzero on success.
 */
int liobj_object_set_transform (
	LIObjObject*          self,
	const LIMatTransform* value)
{
	/* Warp to new position. */
	if (!private_warp (self, &value->position))
		return 0;
	self->transform = *value;

	/* Invoke program->callbacks. */
	lical_callbacks_call (self->manager->program->callbacks, "object-transform", lical_marshal_DATA_PTR_PTR, self, value);

	return 1;
}

/*****************************************************************************/

static int private_warp (
	LIObjObject*       self,
	const LIMatVector* position)
{
	LIObjSector* dst;
	LIObjSector* src;

	/* Do not warp hidden objects. */
	if (!(self->flags & LIENG_OBJECT_FLAG_REALIZED))
	{
		lisys_assert (self->sector == NULL);
		return 1;
	}

	/* Do not add static objects to sectors. */
	if (self->flags & LIENG_OBJECT_FLAG_STATIC)
	{
		lisys_assert (self->sector == NULL);
		return 1;
	}

	/* Get the source and destination sectors. */
	src = self->sector;
	dst = lialg_sectors_data_point (self->manager->program->sectors, LIALG_SECTORS_CONTENT_ENGINE, position, 1);
	if (dst == NULL)
		return 0;

	/* Move between the sectors. */
	if (src != dst)
	{
		if (lialg_u32dic_insert (dst->objects, self->id, self) == NULL)
			return 0;
		if (src != NULL)
			lialg_u32dic_remove (src->objects, self->id);
		self->sector = dst;
	}

	return 1;
}

/** @} */
/** @} */
