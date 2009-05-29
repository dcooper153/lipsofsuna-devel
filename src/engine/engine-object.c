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
#include "engine-object.h"
#include "engine-selection.h"

#define LIENG_OBJECT_VERSION 0
#define LIENG_OBJECT_APPROACH_TOLERANCE 1.0f

static int
private_warp (liengObject*       self,
              const limatVector* position);

/*****************************************************************************/

liengObject*
lieng_object_new (liengEngine*     engine,
                  liengModel*      model,
                  liphyShapeMode   shape_mode,
                  liphyControlMode control_mode,
                  uint32_t         id,
                  void*            data)
{
	return engine->calls.lieng_object_new (engine, model, shape_mode, control_mode, id, data);
}

/**
 * \brief Frees the object.
 *
 * \param self Object.
 */
void
lieng_object_free (liengObject* self)
{
	self->engine->calls.lieng_object_free (self);
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
 * \brief Serializes or deserializes the object from a stream.
 * 
 * \param self Object.
 * \param serialize Serializer.
 * \return Nonzero on success.
 */
int
lieng_object_serialize (liengObject*    self,
                        liarcSerialize* serialize)
{
	return self->engine->calls.lieng_object_serialize (self, serialize);
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
	self->engine->calls.lieng_object_update (self, secs);
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
	liphy_object_get_angular_momentum (self->physics, value);
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
	liphy_object_set_angular_momentum (self->physics, value);
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
 * \param self Object.
 * \param model Model or NULL.
 * \return Nonzero on success.
 */
int
lieng_object_set_model (liengObject* self,
                        liengModel*  model)
{
	return self->engine->calls.lieng_object_set_model (self, model);
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
 * \param model Model number.
 * \return Nonzero on success.
 */
int
lieng_object_set_model_code (liengObject* self,
                             int          id)
{
	liengModel* model;

	model = lieng_engine_find_model_by_code (self->engine, id);
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
 * \param model Model string.
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
	return self->engine->calls.lieng_object_set_realized (self, value);
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
		liphy_object_set_shape (self->physics, shape, liphy_object_get_shape_mode (self->physics));
	else
		liphy_object_set_shape (self->physics, NULL, liphy_object_get_shape_mode (self->physics));
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
	return self->engine->calls.lieng_object_set_transform (self, value);
}

void*
lieng_object_get_userdata (liengObject* self,
                           int          type)
{
	return self->userdata[type];
}

void
lieng_object_set_userdata (liengObject* self,
                           int          type,
                           void*        data)
{
	self->userdata[type] = data;
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
	return self->engine->calls.lieng_object_set_velocity (self, value);
}

/*****************************************************************************/

static liengObject*
private_callback_new (liengEngine*     engine,
                      liengModel*      model,
                      liphyShapeMode   shape_mode,
                      liphyControlMode control_mode,
                      uint32_t         id,
                      void*            data)
{
	double rnd;
	liengObject* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liengObject));
	if (self == NULL)
		return NULL;
	self->id = id;
	self->engine = engine;
	self->flags = LIENG_OBJECT_FLAG_SAVE;

	/* Choose object number. */
	while (!self->id)
	{
		rnd = rand () / (double) RAND_MAX;
		self->id = engine->range.start + (uint32_t)(engine->range.size * rnd);
		if (!self->id)
			continue;
		if (lialg_u32dic_find (engine->objects, self->id))
			self->id = 0;
	}

	/* Insert to object list. */
	if (!lialg_u32dic_insert (engine->objects, self->id, self))
	{
		free (self);
		return 0;
	}

	/* Initialize physics. */
	if (model != NULL)
		self->physics = liphy_object_new (engine->physics, model->physics, shape_mode, control_mode);
	else
		self->physics = liphy_object_new (engine->physics, NULL, shape_mode, control_mode);
	if (self->physics == NULL)
	{
		lialg_u32dic_remove (engine->objects, self->id);
		free (self);
		return NULL;
	}

	/* Initialize graphics. */
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->engine->renderapi != NULL)
	{
		self->render = self->engine->renderapi->lirnd_object_new (engine->render, self->id);
		if (self->render == NULL)
		{
			lialg_u32dic_remove (engine->objects, self->id);
			liphy_object_free (self->physics);
			free (self);
			return NULL;
		}
	}
#endif

	/* Set model. */
	if (model != NULL)
	{
		lieng_object_set_model (self, model);
#ifndef LIENG_DISABLE_GRAPHICS
		if (self->engine->renderapi != NULL)
			self->engine->renderapi->lirnd_object_set_animation (self->render, 0, "idle", -1, 0.0f);
#endif
	}

	return self;
}

static void
private_callback_free (liengObject* self)
{
	/* Remove from engine. */
	if (self->sector != NULL)
		lieng_object_set_realized (self, 0);
	lieng_object_set_selected (self, 0);
	lialg_u32dic_remove (self->engine->objects, self->id);

	/* Free all memory. */
	liphy_object_free (self->physics);
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->engine->renderapi != NULL)
		self->engine->renderapi->lirnd_object_free (self->render);
#endif
	free (self);
}

static int
private_callback_moved (liengObject* self)
{
	uint32_t id;
	liengSector* dst;
	liengSector* src;
	limatTransform transform;

	/* Get source sector. */
	lieng_object_get_transform (self, &transform);
	id = LIENG_SECTOR_INDEX_FROM_POINT (transform.position);
	src = self->sector;

	/* Get destination sector. */
	dst = lieng_engine_find_sector (self->engine, id);
	if (dst == NULL)
	{
		dst = lieng_sector_new (self->engine, id, self->engine->config.dir);
		if (dst == NULL)
			return 0;
	}

	/* Update current sector. */
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

static int
private_callback_serialize (liengObject*    self,
                            liarcSerialize* serialize)
{
	int value;
	char* model;
	uint8_t version;
	uint32_t flags;
	liReader* reader = serialize->reader;
	liarcWriter* writer = serialize->writer;
	limatTransform transform;

	if (liarc_serialize_get_write (serialize))
	{
		/* Write header. */
		liarc_writer_append_uint8 (writer, LIENG_OBJECT_VERSION);
		liarc_writer_append_uint32 (writer, self->flags);

		/* Write model. */
		if (self->model != NULL)
			liarc_writer_append_string (writer, self->model->name);
		liarc_writer_append_nul (writer);

		/* Write physics. */
		if (!liphy_object_write (self->physics, writer))
			return 0;

		return !writer->error;
	}
	else
	{
		/* Read version. */
		if (!li_reader_get_uint8 (reader, &version))
			return 0;
		if (version != LIENG_OBJECT_VERSION)
		{
			lisys_error_set (EINVAL, "incorrect engine object version");
			return 0;
		}

		/* Read flags. */
		/* FIXME: Sanity checks? */
		if (!li_reader_get_uint32 (reader, &flags))
			return 0;
		self->flags = flags;

		/* Read model. */
		if (!li_reader_get_text (reader, "", &model))
			return 0;
		lieng_object_set_model_name (self, model);
		free (model);

		/* Read physics. */
		if (!liphy_object_read (self->physics, reader))
			return 0;
		liphy_object_get_transform (self->physics, &transform);
		lieng_object_set_transform (self, &transform);
		value = liphy_object_get_realized (self->physics);
		lieng_object_set_realized (self, value);

		return 1;
	}
}

static void
private_callback_update (liengObject* self,
                         float        secs)
{
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->engine->renderapi != NULL)
		self->engine->renderapi->lirnd_object_update (self->render, self->engine->scene_normal, secs);
#endif
}

static int
private_callback_set_model (liengObject* self,
                            liengModel*  model)
{
//	lirnd_render_remove_object (self->engine->render, self->render);
	if (model != NULL)
	{
#ifndef LIENG_DISABLE_GRAPHICS
		if (self->engine->renderapi != NULL)
			self->engine->renderapi->lirnd_object_set_model (self->render, model->render);
#endif
		liphy_object_set_shape (self->physics, model->physics,
			liphy_object_get_shape_mode (self->physics));
	}
	else
	{
#ifndef LIENG_DISABLE_GRAPHICS
		if (self->engine->renderapi != NULL)
			self->engine->renderapi->lirnd_object_set_model (self->render, NULL);
#endif
		liphy_object_set_shape (self->physics, NULL,
			liphy_object_get_shape_mode (self->physics));
	}
	self->model = model;
//	lirnd_render_insert_object (self->engine->render, self->render);

	return 1;
}

static int
private_callback_set_realized (liengObject* self,
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

		/* Protect from deletion. */
		lieng_object_ref (self, 1);
	}
	else
	{
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

static int
private_callback_set_transform (liengObject*          self,
                                const limatTransform* value)
{
	liphy_object_set_transform (self->physics, value);
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->engine->renderapi != NULL)
		self->engine->renderapi->lirnd_object_set_transform (self->render, value);
#endif
	if (lieng_object_get_realized (self))
		private_warp (self, &value->position);

	return 1;
}

int
private_callback_set_velocity (liengObject*       self,
                               const limatVector* value)
{
	liphy_object_set_velocity (self->physics, value);
	return 1;
}

const liengCalls lieng_default_calls =
{
	private_callback_new,
	private_callback_free,
	private_callback_moved,
	private_callback_serialize,
	private_callback_update,
	private_callback_set_model,
	private_callback_set_realized,
	private_callback_set_transform,
	private_callback_set_velocity,
};

/*****************************************************************************/

static int
private_warp (liengObject*       self,
              const limatVector* position)
{
	uint32_t id;
	uint32_t id1;
	liengSector* dst;
	liengSector* src = self->sector;

	/* Check for changes. */
	id = LIENG_SECTOR_INDEX_FROM_POINT (*position);
	if (src != NULL)
	{
		id1 = LIENG_SECTOR_INDEX (src->x, src->y, src->z);
		if (id == id1)
			return 1;
	}

	/* Find or create new sector. */
	dst = lieng_engine_find_sector (self->engine, id);
	if (dst == NULL)
	{
		if (self->engine->config.flags & LIENG_FLAG_REMOTE_SECTORS)
			dst = lieng_sector_new (self->engine, id, NULL);
		else
			dst = lieng_sector_new (self->engine, id, self->engine->config.dir);
		if (dst == NULL)
			return 0;
	}

	/* Update current sector. */
	assert (src != dst);
	if (!lieng_sector_insert_object (dst, self))
		return 0;
	if (src != NULL)
		lieng_sector_remove_object (src, self);
	self->sector = dst;

	return 1;
}

/** @} */
/** @} */
