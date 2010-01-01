/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liengObject Object
 * @{
 */

#include <network/lips-network.h>
#include "engine-constraint.h"
#include "engine-object.h"
#include "engine-selection.h"

#warning Engine object refresh radius is hardcoded.
#define REFRESH_RADIUS 10.0f

#define LIENG_OBJECT_APPROACH_TOLERANCE 1.5f

static int
private_warp (liengObject*       self,
              const limatVector* position);

/*****************************************************************************/

/**
 * \brief Creates a new engine object.
 *
 * \param engine Engine.
 * \param model Model or NULL.
 * \param control Control mode for the physics object.
 * \param id Object ID or 0 for unique.
 * \return Engine object or NULL.
 */
liengObject*
lieng_object_new (liengEngine*     engine,
                  liengModel*      model,
                  liphyControlMode control,
                  uint32_t         id)
{
	double rnd;
	liengObject* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liengObject));
	if (self == NULL)
		return NULL;
	self->id = id;
	self->engine = engine;
	self->flags = LIENG_OBJECT_FLAG_SAVE;

	/* Choose object number. */
	while (!self->id)
	{
		rnd = lisys_randf ();
		self->id = engine->range.start + (uint32_t)(engine->range.size * rnd);
		if (!self->id)
			continue;
		if (lialg_u32dic_find (engine->objects, self->id))
			self->id = 0;
	}

	/* Insert to object list. */
	if (!lialg_u32dic_insert (engine->objects, self->id, self))
	{
		lisys_free (self);
		return 0;
	}

	/* Initialize physics. */
	if (model != NULL)
		self->physics = liphy_object_new (engine->physics, self->id, model->physics, control);
	else
		self->physics = liphy_object_new (engine->physics, self->id, NULL, control);
	if (self->physics == NULL)
		goto error;

	/* Allocate pose buffer. */
	self->pose = limdl_pose_new ();
	if (self->pose == NULL)
		goto error;

	/* Set model. */
	if (model != NULL)
	{
		lieng_object_set_model (self, model);
		lieng_object_set_animation (self, 0, "idle", -1, 0.0f);
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-new", lical_marshal_DATA_PTR, self);

	return self;

error:
	if (self->pose != NULL)
		limdl_pose_free (self->pose);
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	lialg_u32dic_remove (engine->objects, self->id);
	lisys_free (self);
	return NULL;
}

/**
 * \brief Frees the object.
 *
 * \param self Object.
 */
void
lieng_object_free (liengObject* self)
{
	liengConstraint* constraint;
	liengConstraint* constraint_next;

	/* Unrealize. */
	lieng_object_set_realized (self, 0);
	lieng_object_set_selected (self, 0);

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-free", lical_marshal_DATA_PTR, self);

	/* Free constraints. */
	/* FIXME: Would be better to have objects remember their own constraints. */
	for (constraint = self->engine->constraints ; constraint != NULL ; constraint = constraint_next)
	{
		constraint_next = constraint->next;
		if (constraint->objects[0] == self ||
		    constraint->objects[1] == self)
		{
			if (constraint->prev != NULL)
				constraint->prev->next = constraint->next;
			else
				self->engine->constraints = constraint->next;
			if (constraint->next != NULL)
				constraint->next->prev = constraint->prev;
			lisys_free (constraint);
		}
	}

	/* Remove from engine. */
	lialg_u32dic_remove (self->engine->objects, self->id);

	/* Free pose. */
	if (self->pose != NULL)
		limdl_pose_free (self->pose);

	/* Free all memory. */
	liphy_object_free (self->physics);
	lisys_free (self);
}

/**
 * \brief References or unreferences the object.
 *
 * If the reference count reaches zero, the object is queued for removal. If
 * the engine is compiled with Lua scripting enabled, the object will be
 * garbage collected by Lua.
 *
 * \param self Object.
 * \param count Number of times to reference, negative to unreference.
 */
void
lieng_object_ref (liengObject* self,
                  int          count)
{
#ifndef LIENG_DISABLE_SCRIPTS
	int i;

	if (self->script != NULL)
	{
		for (i = count ; i > 0 ; i--)
			liscr_data_ref (self->script, NULL);
		for (i = count ; i < 0 ; i++)
			liscr_data_unref (self->script, NULL);
		return;
	}
#endif
	self->refs += count;
	assert (self->refs >= 0);
	if (self->refs <= 0)
		lieng_object_free (self);
}

/**
 * \brief Configures the object to move towards the given target.
 *
 * Configures the physics state of the object so that it heads directly
 * towards the specified target point.
 *
 * \param self Object.
 * \param target Target position vector.
 * \param speed Movement speed.
 */
void
lieng_object_approach (liengObject*       self,
                       const limatVector* target,
                       float              speed)
{
	limatVector tmp;
	limatQuaternion dir;
	limatTransform transform;

	if (!lieng_object_get_realized (self))
		return;

	/* Get direction to target. */
	lieng_object_get_transform (self, &transform);
	tmp = limat_vector_subtract (*target, transform.position);
	tmp.y = 0.0f;
	if (limat_vector_get_length (tmp) < LIENG_OBJECT_APPROACH_TOLERANCE)
	{
		liphy_object_set_movement (self->physics, 0.0f);
		return;
	}

	/* Set look direction. */
	dir = limat_quaternion_look (tmp, limat_vector_init (0.0f, 1.0f, 0.0f));
	transform.rotation = limat_quaternion_conjugate (dir);
	lieng_object_set_transform (self, &transform);

	/* Move towards target. */
	liphy_object_set_movement (self->physics, speed);
}

/**
 * \brief Finds a node by name.
 *
 * Searches for a pose node if the object has a pose associated to it and a
 * model node otherwise.
 *
 * \param self Object.
 * \param name Node name or NULL.
 */
limdlNode*
lieng_object_find_node (liengObject* self,
                        const char*  name)
{
	if (self->pose != NULL)
		return limdl_pose_find_node (self->pose, name);
	if (self->model != NULL)
		return limdl_model_find_node (self->model->model, name);

	return NULL;
}

/**
 * \brief Modifies the velocity of the object with an impulse.
 *
 * \param self Object.
 * \param point Impulse point relative to the body.
 * \param impulse Impulse force.
 */
void
lieng_object_impulse (liengObject*       self,
                      const limatVector* point,
                      const limatVector* impulse)
{
	liphy_object_impulse (self->physics, point, impulse);
}

/**
 * \brief Causes the object to jump.
 *
 * Adds the walking velocity vector of the character to its rigid body
 * velocity and then does the same as #liphy_object_impulse. This causes
 * the character to automatically jump to its walking direction.
 *
 * If the object is not a character, the character specific operations are
 * skipped and the function behaves the same way as #liphy_object_impulse
 * with point of impulse set to origin in body space.
 *
 * \param self Object.
 * \param impulse Jump force.
 */
void
lieng_object_jump (liengObject*       self,
                   const limatVector* impulse)
{
	liphy_object_jump (self->physics, impulse);
}

/**
 * \brief Called when the object has moved.
 *
 * \param self Object.
 */
int
lieng_object_moved (liengObject* self)
{
	liengSector* dst;
	liengSector* src;
	limatTransform transform;

	/* Refresh sector list. */
	lieng_object_get_transform (self, &transform);
	lialg_sectors_refresh_point (self->engine->sectors, &transform.position, REFRESH_RADIUS);

	/* Move between sectors. */
	src = self->sector;
	dst = lialg_sectors_data_point (self->engine->sectors, "engine", &transform.position, 1);
	if (dst == NULL)
		return 0;
	if (src != dst)
	{
		if (!lieng_sector_insert_object (dst, self))
			return 0;
		if (src != NULL)
			lieng_sector_remove_object (src, self);
		self->sector = dst;
	}

	/* Physics defeat smoothing. */
	self->smoothing.target = transform;

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-motion", lical_marshal_DATA_PTR, self);

	return 1;
}

/**
 * \brief Updates the state of the object.
 * 
 * \param self Object.
 * \param secs Number of seconds since last tick.
 */
void
lieng_object_update (liengObject* self,
                     float        secs)
{
	limatTransform transform;
	limatTransform transform0;
	limatTransform transform1;

	/* Animations. */
	if (self->pose != NULL)
		limdl_pose_update (self->pose, secs);

	/* Smoothing. */
	if (self->smoothing.rot != 0.0f || self->smoothing.pos != 0.0f)
	{
		if (lieng_object_get_realized (self))
		{
			/* Calculate new position. */
			liphy_object_get_transform (self->physics, &transform0);
			transform1 = self->smoothing.target;
			transform0.rotation = limat_quaternion_get_nearest (transform0.rotation, transform1.rotation);
			transform.position = limat_vector_lerp (
				transform1.position, transform0.position,
				0.5f * self->smoothing.pos);
			transform.rotation = limat_quaternion_nlerp (
				transform1.rotation, transform0.rotation,
				0.5f * self->smoothing.rot);

			/* Set new position. */
			liphy_object_set_transform (self->physics, &transform);
			private_warp (self, &transform.position);

			/* Invoke callbacks. */
			lical_callbacks_call (self->engine->callbacks, self->engine, "object-transform", lical_marshal_DATA_PTR_PTR, self, &transform);
		}
	}
}

/**
 * \brief Gets the angular momentum of the object.
 *
 * \param self Object.
 * \param value Return location for the angular momentum vector.
 */
void
lieng_object_get_angular_momentum (const liengObject* self,
                                   limatVector*       value)
{
	liphy_object_get_angular (self->physics, value);
}

/**
 * \brief Sets the angular momentum of the object.
 *
 * \param self Object.
 * \param value Angular momentum vector.
 */
void
lieng_object_set_angular_momentum (liengObject*       self,
                                   const limatVector* value)
{
	liphy_object_set_angular (self->physics, value);
}

/**
 * \brief Sets the current animation of a specified channel.
 *
 * \param self Object.
 * \param channel Channel index.
 * \param animation Animation name or NULL.
 * \param repeats Number of time to repeats, -1 for infinite.
 * \param priority Blending priority.
 * \return Nonzero on success.
 */
void
lieng_object_set_animation (liengObject* self,
                            int          channel,
                            const char*  animation,
                            int          repeats,
                            float        priority)
{
	limdl_pose_fade_channel (self->pose, channel, LIMDL_POSE_FADE_AUTOMATIC);
	if (animation != NULL)
	{
		limdl_pose_set_channel_animation (self->pose, channel, animation);
		limdl_pose_set_channel_repeats (self->pose, channel, repeats);
		limdl_pose_set_channel_priority (self->pose, channel, priority);
		limdl_pose_set_channel_state (self->pose, channel, LIMDL_POSE_CHANNEL_STATE_PLAYING);
	}
}

/**
 * \brief Gets the bounding box size of the object.
 *
 * \param self Object.
 * \param bounds Return location for the bounding box.
 */
void
lieng_object_get_bounds (const liengObject* self,
                         limatAabb*         bounds)
{
	if (self->model != NULL && self->model->model != NULL)
		*bounds = self->model->model->bounds;
	else
		limat_aabb_init (bounds);
}

/**
 * \brief Gets the transformed bounding box of the object.
 *
 * \param self Object.
 * \param bounds Return location for the bounding box.
 */
void
lieng_object_get_bounds_transform (const liengObject* self,
                                   limatAabb*         bounds)
{
	limatTransform t;

	if (self->model != NULL && self->model->model != NULL)
	{
		lieng_object_get_transform (self, &t);
		lieng_model_get_bounds_transform (self->model, &t, bounds);
	}
	else
		limat_aabb_init (bounds);
}

/**
 * \brief Gets the collision group of the object.
 *
 * \param self Object.
 * \return Collision group mask.
 */
int
lieng_object_get_collision_group (const liengObject* self)
{
	return liphy_object_get_collision_group (self->physics);
}

/**
 * \brief Sets the collision group of the object.
 *
 * Two objects can collide with each other if the mask of the first object has
 * common bits with the group of the second object and the mask of the second
 * object has common bits with the group of the first object.
 *
 * \param self Object.
 * \param mask Collision group mask.
 */
void
lieng_object_set_collision_group (liengObject* self,
                                  int          mask)
{
	liphy_object_set_collision_group (self->physics, mask);
}

/**
 * \brief Gets the collision mask of the object.
 *
 * \param self Object.
 * \return Collision mask.
 */
int
lieng_object_get_collision_mask (const liengObject* self)
{
	return liphy_object_get_collision_mask (self->physics);
}

/**
 * \brief Sets the collision mask of the object.
 *
 * Two objects can collide with each other if the mask of the first object has
 * common bits with the group of the second object and the mask of the second
 * object has common bits with the group of the first object.
 *
 * \param self Object.
 * \param mask Collision mask.
 */
void
lieng_object_set_collision_mask (liengObject* self,
                                 int          mask)
{
	liphy_object_set_collision_mask (self->physics, mask);
}

/**
 * \brief Checks if the simulation state of the object needs to be synchronized.
 *
 * \param self Object.
 * \return Nonzero if synchronization is needed.
 */
int
lieng_object_get_dirty (const liengObject* self)
{
	if (self->flags & LIENG_OBJECT_FLAG_DIRTY)
		return 1;
	return 0;
}

/**
 * \brief Sets the value of the dirty flag.
 *
 * \param self Object.
 * \param value Boolean.
 */
void
lieng_object_set_dirty (liengObject* self,
                        int          value)
{
	if (value)
		self->flags |= LIENG_OBJECT_FLAG_DIRTY;
	else
		self->flags &= ~LIENG_OBJECT_FLAG_DIRTY;
}

/**
 * \brief Gets the distance between the objects.
 *
 * If either of the objects is in not realized, LI_MATH_INFINITE is returned.
 *
 * \param self An object.
 * \param object An object.
 * \return The distance.
 */
float
lieng_object_get_distance (const liengObject* self,
                           const liengObject* object)
{
	limatTransform t0;
	limatTransform t1;

	if (!lieng_object_get_realized (self) ||
	    !lieng_object_get_realized (object))
		return LI_MATH_INFINITE;
	lieng_object_get_transform (self, &t0);
	lieng_object_get_transform (object, &t1);

	return limat_vector_get_length (limat_vector_subtract (t0.position, t1.position));
}

/**
 * \brief Returns nonzero if the object is standing on ground.
 *
 * This only works for character objects. Other types always return zero.
 *
 * \param self Object.
 * \return Nonzero if standing on ground.
 */
int
lieng_object_get_ground (const liengObject* self)
{
	return liphy_object_get_ground (self->physics);
}

int
lieng_object_get_flags (const liengObject* self)
{
	return self->flags;
}

void
lieng_object_set_flags (liengObject* self,
                        int          flags)
{
	self->flags = flags;
}

/**
 * \brief Gets the mass of the object.
 *
 * \param self Object.
 * \return Mass.
 */
float
lieng_object_get_mass (const liengObject* self)
{
	return liphy_object_get_mass (self->physics);
}

/**
 * \brief Sets the mass of the object.
 *
 * \param self Object.
 * \param value Mass.
 */
void
lieng_object_set_mass (liengObject* self,
                       float        value)
{
	liphy_object_set_mass (self->physics, value);
}

/**
 * \brief Replaces the current model of the object.
 *
 * \warning This function is also called by #lieng_engine_load_model when a
 * model has been reloaded so we must not early exit even if the model is the
 * same.
 *
 * \param self Object.
 * \param model Model or NULL.
 * \return Nonzero on success.
 */
int
lieng_object_set_model (liengObject* self,
                        liengModel*  model)
{
	liengConstraint* constraint;

	/* Switch model. */
	if (model != NULL)
	{
		limdl_pose_set_model (self->pose, model->model);
		liphy_object_set_shape (self->physics, model->physics);
	}
	else
	{
		limdl_pose_set_model (self->pose, NULL);
		liphy_object_set_shape (self->physics, NULL);
	}
	self->model = model;

	/* Rebuild constraints. */
	/* TODO: Looping through all constraints in the engine is wasteful. */
	for (constraint = self->engine->constraints ; constraint != NULL ; constraint = constraint->next)
	{
		if (constraint->objects[0] == self ||
		    constraint->objects[1] == self)
			lieng_constraint_rebuild (constraint);
	}

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-model", lical_marshal_DATA_PTR_PTR, self, &model);

	return 1;
}

int
lieng_object_get_model_code (const liengObject* self)
{
	if (self->model == NULL)
		return LINET_INVALID_MODEL;
	return self->model->id;
}

/**
 * \brief Replaces the current model of the object.
 *
 * \param self Object.
 * \param value Model number.
 * \return Nonzero on success.
 */
int
lieng_object_set_model_code (liengObject* self,
                             int          value)
{
	liengModel* model;

	model = lieng_engine_find_model_by_code (self->engine, value);
	if (model == NULL)
		return 0;
	lieng_object_set_model (self, model);

	return 1;
}

const char*
lieng_object_get_model_name (const liengObject* self)
{
	if (self->model != NULL)
		return self->model->name;
	return "";
}

/**
 * \brief Replaces the current model of the object.
 *
 * \param self Object.
 * \param value Model string.
 * \return Nonzero on success.
 */
int
lieng_object_set_model_name (liengObject* self,
                             const char*  value)
{
	liengModel* model;

	model = lieng_engine_find_model_by_name (self->engine, value);
	if (model == NULL)
		return 0;
	lieng_object_set_model (self, model);

	return 1;
}

/**
 * \brief Checks if the object is added to the simulation.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int
lieng_object_get_realized (const liengObject* self)
{
	if (self->sector == NULL)
		return 0;
	return 1;
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
int
lieng_object_set_realized (liengObject* self,
                           int          value)
{
	limatTransform transform;

	if (value == lieng_object_get_realized (self))
		return 1;
	if (value)
	{
		/* Activate physics. */
		if (!liphy_object_set_realized (self->physics, 1))
			return 0;

		/* Link to map. */
		lieng_object_get_transform (self, &transform);
		if (!private_warp (self, &transform.position))
		{
			liphy_object_set_realized (self->physics, 0);
			return 0;
		}

		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, self->engine, "object-visibility", lical_marshal_DATA_PTR_INT, self, 1);

		/* Protect from deletion. */
		lieng_object_ref (self, 1);
	}
	else
	{
		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, self->engine, "object-visibility", lical_marshal_DATA_PTR_INT, self, 0);

		/* Deactivate physics. */
		liphy_object_set_realized (self->physics, 0);

		/* Remove from map. */
		lieng_sector_remove_object (self->sector, self);
		self->sector = NULL;

		/* Remove protection. */
		lieng_object_ref (self, -1);
	}

	return 1;
}

liengSector*
lieng_object_get_sector (liengObject* self)
{
	return self->sector;
}

/**
 * \brief Returns whether the object is selected.
 * 
 * \param self Object.
 * \return Nonzero if selected.
 */
int
lieng_object_get_selected (const liengObject* self)
{
	liengSelection* selection;

	selection = lialg_ptrdic_find (self->engine->selection, (void*) self);
	if (selection != NULL)
		return 1;

	return 0;
}

/**
 * \brief Selects or deselects the object.
 *
 * \param self Object.
 * \param select Nonzero if should select.
 */
int
lieng_object_set_selected (liengObject* self,
                           int          select)
{
	liengSelection* selection;

	if (select)
	{
		selection = lialg_ptrdic_find (self->engine->selection, self);
		if (selection != NULL)
			return 1;
		selection = lieng_selection_new (self);
		if (selection == NULL)
			return 0;
		if (lialg_ptrdic_insert (self->engine->selection, self, selection) == NULL)
		{
			lieng_selection_free (selection);
			return 0;
		}
	}
	else
	{
		selection = lialg_ptrdic_find (self->engine->selection, self);
		if (selection != NULL)
		{
			lieng_selection_free (selection);
			lialg_ptrdic_remove (self->engine->selection, self);
		}
	}

	return 1;
}

/**
 * \brief Replaces the shape of the object.
 *
 * The camera uses this for creating a ghost object for itself by setting
 * its model to NULL and then assigning a privately managed shape to its
 * engine object.
 *
 * \warning Assigning a shape from another model without incrementing the
 * reference count of the model manually will likely lead to the model being
 * unloaded while still in use and the game crashing.
 *
 * \param self Object.
 * \param shape Collision shape or NULL.
 */
void
lieng_object_set_shape (liengObject* self,
                        liphyShape*  shape)
{
	if (shape != NULL)
		liphy_object_set_shape (self->physics, shape);
	else
		liphy_object_set_shape (self->physics, NULL);
}

/**
 * \brief Sets positional and rotation smoothing.
 *
 * By setting either of the smoothing values to non-zero, the engine object is
 * set to interpolation mode. In interpolation mode, all transformations are
 * delayed and happen gradually each time the engine state is updated. Both
 * values default to zero.
 *
 * \param self Object.
 * \param pos Positional smoothing.
 * \param rot Rotational smoothing.
 */
void
lieng_object_set_smoothing (liengObject* self,
                            float        pos,
                            float        rot)
{
	self->smoothing.pos = pos;
	self->smoothing.rot = rot;
}

/**
 * \brief Gets the movement speed of the object.
 *
 * \param self Object.
 * \return Movement speed.
 */
float
lieng_object_get_speed (const liengObject* self)
{
	return liphy_object_get_speed (self->physics);
}

/**
 * \brief Sets the movement speed of the object.
 *
 * \param self Object.
 * \param value Movement speed.
 */
void
lieng_object_set_speed (liengObject* self,
                        float        value)
{
	liphy_object_set_speed (self->physics, value);
}

/**
 * \brief Gets the smoothing target transformation of the object.
 *
 * Works exactly like #lieng_object_get_transform when smoothing is disabled.
 * However, if smoothing is enabled, this returns the target transformation
 * instead of the current interpolation state. 
 *
 * \param self Object.
 * \param value Return location for the transformation.
 */
void
lieng_object_get_target (const liengObject* self,
                         limatTransform*    value)
{
	*value = self->smoothing.target;
}

/**
 * \brief Gets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Return location for the transformation.
 */
void
lieng_object_get_transform (const liengObject* self,
                            limatTransform*    value)
{
	liphy_object_get_transform (self->physics, value);
}

/**
 * \brief Sets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Transformation.
 * \return Nonzero on success.
 */
int
lieng_object_set_transform (liengObject*          self,
                            const limatTransform* value)
{
	int realized;

	realized = lieng_object_get_realized (self);
	if (!realized || (self->smoothing.rot == 0.0f && self->smoothing.pos == 0.0f))
	{
		/* Transform immediately. */
		liphy_object_set_transform (self->physics, value);
		if (realized)
			private_warp (self, &value->position);

		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, self->engine, "object-transform", lical_marshal_DATA_PTR_PTR, self, value);
	}
	self->smoothing.target = *value;

	return 1;
}

void*
lieng_object_get_userdata (liengObject* self)
{
	return self->userdata;
}

void
lieng_object_set_userdata (liengObject* self,
                           void*        data)
{
	self->userdata = data;
}

/**
 * \brief Get the velocity vector of the object.
 *
 * \param self object
 * \param value Return location for the vector.
 */
void
lieng_object_get_velocity (const liengObject* self,
                           limatVector*       value)
{
	liphy_object_get_velocity (self->physics, value);
}

/**
 * \brief Set the velocity vector of the object.
 *
 * \param self object
 * \param value Vector.
 * \return Nonzero on success.
 */
int
lieng_object_set_velocity (liengObject*       self,
                           const limatVector* value)
{
	liphy_object_set_velocity (self->physics, value);
	return 1;
}

/*****************************************************************************/

static int
private_warp (liengObject*       self,
              const limatVector* position)
{
	liengSector* dst;
	liengSector* src;

	/* Refresh sector list. */
	lialg_sectors_refresh_point (self->engine->sectors, position, REFRESH_RADIUS);

	/* Move between sectors. */
	src = self->sector;
	dst = lialg_sectors_data_point (self->engine->sectors, "engine", position, 1);
	if (dst == NULL)
		return 0;
	if (src != dst)
	{
		if (!lieng_sector_insert_object (dst, self))
			return 0;
		if (src != NULL)
			lieng_sector_remove_object (src, self);
		self->sector = dst;
	}

	return 1;
}

/** @} */
/** @} */
