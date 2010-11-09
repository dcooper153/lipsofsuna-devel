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
 * \addtogroup LIEngObject Object
 * @{
 */

#include <lipsofsuna/network.h>
#include "engine-object.h"
#include "engine-selection.h"

#warning Engine object refresh radius is hardcoded.
#define REFRESH_RADIUS 10.0f
#define SCRIPT_POINTER_MODEL ((void*) -1)

static int
private_warp (LIEngObject*       self,
              const LIMatVector* position);

/*****************************************************************************/

/**
 * \brief Creates a new engine object.
 * \param engine Engine.
 * \param id Object ID or 0 for unique.
 * \return Engine object or NULL.
 */
LIEngObject* lieng_object_new (
	LIEngEngine* engine,
	uint32_t     id)
{
	LIEngObject* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngObject));
	if (self == NULL)
		return NULL;
	self->id = id;
	self->engine = engine;
	self->transform = limat_transform_identity ();
	self->smoothing.target = limat_transform_identity ();

	/* Choose object number. */
	while (!self->id)
	{
		self->id = lialg_random_range (&engine->random, LINET_RANGE_ENGINE_START, LINET_RANGE_ENGINE_END);
		if (lieng_engine_find_object (engine, self->id))
			id = 0;
	}

	/* Insert to object list. */
	if (!lialg_u32dic_insert (engine->objects, self->id, self))
	{
		lisys_free (self);
		return 0;
	}

	/* Allocate pose buffer. */
	self->pose = limdl_pose_new ();
	if (self->pose == NULL)
		goto error;

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-new", lical_marshal_DATA_PTR, self);

	return self;

error:
	if (self->pose != NULL)
		limdl_pose_free (self->pose);
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
lieng_object_free (LIEngObject* self)
{
	/* Unrealize. */
	lieng_object_set_realized (self, 0);

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-free", lical_marshal_DATA_PTR, self);

	/* Remove from engine. */
	lialg_u32dic_remove (self->engine->objects, self->id);

	/* Free pose. */
	if (self->pose != NULL)
		limdl_pose_free (self->pose);

	/* Free all memory. */
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
lieng_object_ref (LIEngObject* self,
                  int          count)
{
#ifndef LIENG_DISABLE_SCRIPTS
	int i;

	if (self->script != NULL)
	{
		for (i = count ; i > 0 ; i--)
			liscr_data_ref (self->script);
		for (i = count ; i < 0 ; i++)
			liscr_data_unref (self->script);
		return;
	}
#endif
	self->refs += count;
	lisys_assert (self->refs >= 0);
	if (self->refs <= 0)
		lieng_object_free (self);
}

/**
 * \brief Sets the current animation of a specified channel.
 *
 * \param self Object.
 * \param channel Channel index.
 * \param animation Animation name or NULL.
 * \param permanent Nonzero if should repeat infinitely.
 * \param priority Blending priority.
 * \param time Starting time in seconds.
 * \param fade_in Fade in duration in seconds.
 * \param fade_out Fade out duration in seconds.
 * \return Nonzero if an animation was started or stopped.
 */
int lieng_object_animate (
	LIEngObject* self,
	int          channel,
	const char*  animation,
	int          permanent,
	float        priority,
	float        time,
	float        fade_in,
	float        fade_out)
{
	const char* name;

	/* Avoid restarts in simple cases. */
	if (permanent && channel != -1 && animation != NULL)
	{
		if (limdl_pose_get_channel_state (self->pose, channel) == LIMDL_POSE_CHANNEL_STATE_PLAYING &&
		    limdl_pose_get_channel_repeats (self->pose, channel) == -1)
		{
			name = limdl_pose_get_channel_name (self->pose, channel);
			if (!strcmp (name, animation))
				return 0;
		}
	}

	/* Automatic channel assignment. */
	if (channel == -1)
	{
		for (channel = 254 ; channel > 0 ; channel--)
		{
			if (limdl_pose_get_channel_state (self->pose, channel) == LIMDL_POSE_CHANNEL_STATE_INVALID)
				break;
		}
	}

	/* Clear and set channel. */
	limdl_pose_fade_channel (self->pose, channel, LIMDL_POSE_FADE_AUTOMATIC);
	if (animation != NULL)
	{
		limdl_pose_set_channel_animation (self->pose, channel, animation);
		limdl_pose_set_channel_repeats (self->pose, channel, permanent? -1 : 1);
		limdl_pose_set_channel_priority (self->pose, channel, priority);
		limdl_pose_set_channel_position (self->pose, channel, time);
		limdl_pose_set_channel_state (self->pose, channel, LIMDL_POSE_CHANNEL_STATE_PLAYING);
		limdl_pose_set_channel_fade_in (self->pose, channel, fade_in);
		limdl_pose_set_channel_fade_out (self->pose, channel, fade_out);
	}

	return 1;
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
LIMdlNode*
lieng_object_find_node (LIEngObject* self,
                        const char*  name)
{
	if (self->pose != NULL)
		return limdl_pose_find_node (self->pose, name);
	if (self->model != NULL)
		return limdl_model_find_node (self->model->model, name);

	return NULL;
}

/**
 * \brief Merges a model to the object.
 * \param self Object.
 * \param model Model.
 * \return Nonzero on success.
 */
int lieng_object_merge_model (
	LIEngObject* self,
	LIEngModel*  model)
{
	LIEngModel* tmp;

	if (self->model == NULL)
	{
		tmp = lieng_model_new_copy (model);
		if (tmp == NULL)
			return 0;
		lieng_object_set_model (self, tmp);
	}
	else
	{
		if (!limdl_model_merge (self->model->model, model->model))
			return 0;
		lieng_object_set_model (self, self->model);
	}

	return 1;
}

/**
 * \brief Called when the object has moved.
 *
 * \param self Object.
 */
int
lieng_object_moved (LIEngObject* self)
{
	LIEngSector* dst;
	LIEngSector* src;
	LIMatTransform transform;

	/* Move between sectors. */
	lieng_object_get_transform (self, &transform);
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
 * \brief Refreshes map around the object.
 *
 * Calling this function prevents the map sectors near the object from
 * unloading. One important use case for this is preventing clients from
 * being swapped out when they stand still.
 *
 * \param self Object.
 * \param radius Refresh radius.
 */
void
lieng_object_refresh (LIEngObject* self,
                      float        radius)
{
	LIMatTransform transform;

	if (lieng_object_get_realized (self))
	{
		lieng_object_get_transform (self, &transform);
		lialg_sectors_refresh_point (self->engine->sectors, &transform.position, radius);
	}
}

/**
 * \brief Updates the state of the object.
 * 
 * \param self Object.
 * \param secs Number of seconds since last tick.
 */
void lieng_object_update (
	LIEngObject* self,
	float        secs)
{
	LIMatTransform transform;
	LIMatTransform transform0;
	LIMatTransform transform1;

	/* Smoothing. */
	if (self->smoothing.rot != 0.0f || self->smoothing.pos != 0.0f)
	{
		if (lieng_object_get_realized (self))
		{
			/* Calculate new position. */
			transform0 = self->transform;
			transform1 = self->smoothing.target;
			transform0.rotation = limat_quaternion_get_nearest (transform0.rotation, transform1.rotation);
			transform.position = limat_vector_lerp (
				transform1.position, transform0.position,
				0.5f * self->smoothing.pos);
			transform.rotation = limat_quaternion_nlerp (
				transform1.rotation, transform0.rotation,
				0.5f * self->smoothing.rot);

			/* Set new position. */
			self->transform = transform;
			private_warp (self, &transform.position);

			/* Invoke callbacks. */
			lical_callbacks_call (self->engine->callbacks, self->engine, "object-transform", lical_marshal_DATA_PTR_PTR, self, &transform);
		}
	}
}

/**
 * \brief Gets the bounding box size of the object.
 *
 * \param self Object.
 * \param bounds Return location for the bounding box.
 */
void
lieng_object_get_bounds (const LIEngObject* self,
                         LIMatAabb*         bounds)
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
lieng_object_get_bounds_transform (const LIEngObject* self,
                                   LIMatAabb*         bounds)
{
	LIMatTransform t;

	if (self->model != NULL && self->model->model != NULL)
	{
		lieng_object_get_transform (self, &t);
		lieng_model_get_bounds_transform (self->model, &t, bounds);
	}
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
float
lieng_object_get_distance (const LIEngObject* self,
                           const LIEngObject* object)
{
	LIMatTransform t0;
	LIMatTransform t1;

	if (!lieng_object_get_realized (self) ||
	    !lieng_object_get_realized (object))
		return LIMAT_INFINITE;
	lieng_object_get_transform (self, &t0);
	lieng_object_get_transform (object, &t1);

	return limat_vector_get_length (limat_vector_subtract (t0.position, t1.position));
}

/**
 * \brief Replaces the current model of the object.
 * \param self Object.
 * \param model Model or NULL.
 * \return Nonzero on success.
 */
int lieng_object_set_model (
	LIEngObject* self,
	LIEngModel*  model)
{
	lua_State* lua;
	LIScrScript* script;

	script = liscr_data_get_script (self->script);
	lua = liscr_script_get_lua (script);

	/* Switch model. */
	if (model != NULL)
		limdl_pose_set_model (self->pose, model->model);
	else
		limdl_pose_set_model (self->pose, NULL);
	self->model = model;

	/* Reference the model. */
	liscr_pushpriv (lua, self->script);
	lua_pushlightuserdata (lua, SCRIPT_POINTER_MODEL);
	if (model != NULL)
		liscr_pushdata (lua, model->script);
	else
		lua_pushnil (lua);
	lua_settable (lua, -3);
	lua_pop (lua, 1);

	/* Invoke callbacks. */
	lical_callbacks_call (self->engine->callbacks, self->engine, "object-model", lical_marshal_DATA_PTR_PTR, self, model);

	return 1;
}

/**
 * \brief Checks if the object is added to the simulation.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int
lieng_object_get_realized (const LIEngObject* self)
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
lieng_object_set_realized (LIEngObject* self,
                           int          value)
{
	LIMatTransform transform;

	if (value == lieng_object_get_realized (self))
		return 1;
	if (value)
	{
		/* Link to map. */
		lieng_object_get_transform (self, &transform);
		if (!private_warp (self, &transform.position))
			return 0;

		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, self->engine, "object-visibility", lical_marshal_DATA_PTR_INT, self, 1);

		/* Protect from deletion. */
		lieng_object_ref (self, 1);
	}
	else
	{
		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, self->engine, "object-visibility", lical_marshal_DATA_PTR_INT, self, 0);

		/* Remove from map. */
		lieng_sector_remove_object (self->sector, self);
		self->sector = NULL;

		/* Remove protection. */
		lieng_object_ref (self, -1);
	}

	return 1;
}

LIEngSector*
lieng_object_get_sector (LIEngObject* self)
{
	return self->sector;
}

/**
 * \brief Gets positional and rotation smoothing.
 *
 * \param self Object.
 * \param pos Return location for positional smoothing.
 * \param rot Return location for rotational smoothing.
 */
void lieng_object_get_smoothing (
	LIEngObject* self,
	float*       pos,
	float*       rot)
{
	*pos = self->smoothing.pos;
	*rot = self->smoothing.rot;
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
void lieng_object_set_smoothing (
	LIEngObject* self,
	float        pos,
	float        rot)
{
	self->smoothing.pos = pos;
	self->smoothing.rot = rot;
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
lieng_object_get_target (const LIEngObject* self,
                         LIMatTransform*    value)
{
	int realized;

	realized = lieng_object_get_realized (self);
	if (!realized || (self->smoothing.rot == 0.0f && self->smoothing.pos == 0.0f))
		*value = self->transform;
	else
		*value = self->smoothing.target;
}

/**
 * \brief Gets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Return location for the transformation.
 */
void lieng_object_get_transform (
	const LIEngObject* self,
	LIMatTransform*    value)
{
	*value = self->transform;
}

/**
 * \brief Sets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Transformation.
 * \return Nonzero on success.
 */
int
lieng_object_set_transform (LIEngObject*          self,
                            const LIMatTransform* value)
{
	int realized;

	realized = lieng_object_get_realized (self);
	if (!realized || (self->smoothing.rot == 0.0f && self->smoothing.pos == 0.0f))
	{
		/* Transform immediately. */
		if (realized)
		{
			if (!private_warp (self, &value->position))
				return 0;
		}
		self->transform = *value;

		/* Invoke callbacks. */
		lical_callbacks_call (self->engine->callbacks, self->engine, "object-transform", lical_marshal_DATA_PTR_PTR, self, value);
	}
	self->smoothing.target = *value;

	return 1;
}

void*
lieng_object_get_userdata (LIEngObject* self)
{
	return self->userdata;
}

void
lieng_object_set_userdata (LIEngObject* self,
                           void*        data)
{
	self->userdata = data;
}

/*****************************************************************************/

static int
private_warp (LIEngObject*       self,
              const LIMatVector* position)
{
	LIEngSector* dst;
	LIEngSector* src;

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
