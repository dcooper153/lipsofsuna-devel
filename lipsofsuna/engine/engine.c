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
 * \addtogroup LIEngEngine Engine
 * @{
 */

#include <stdarg.h>
#include "engine.h"
#include "engine-constraint.h"
#include "engine-iterator.h"
#include "engine-selection.h"

static int
private_init (LIEngEngine* self);

static void
private_physics_transform (LIEngEngine* self,
                           LIPhyObject* object);

static void
private_sector_load (void*        self,
                     LIAlgSector* sector);

/*****************************************************************************/

/**
 * \brief Creates a new game engine.
 *
 * \param calls Callback manager.
 * \param sectors Sector manager.
 * \param path Module directory.
 * \return New engine or NULL.
 */
LIEngEngine*
lieng_engine_new (LICalCallbacks* calls,
                  LIAlgSectors*   sectors,
                  const char*     path)
{
	LIEngEngine* self;

	self = lisys_calloc (1, sizeof (LIEngEngine));
	if (self == NULL)
		return NULL;
	self->callbacks = calls;
	self->sectors = sectors;
	self->sectors->sector_load_callback.callback = private_sector_load;
	self->sectors->sector_load_callback.userdata = self;
	self->range.start = 0;
	self->range.size = 0xFFFFFFFF;
	self->config.radius = 1;
	self->config.dir = listr_dup (path);
	if (self->config.dir == NULL)
		goto error;
	if (!private_init (self))
		goto error;

	return self;

error:
	lieng_engine_free (self);
	lisys_error_append ("cannot initialize engine");
	return NULL;
}

void
lieng_engine_free (LIEngEngine* self)
{
	LIAlgU32dicIter iter;
	LIEngConstraint* constraint;
	LIEngConstraint* constraint_next;
	LIEngObject* object;

	/* Clear constraints. */
	for (constraint = self->constraints ; constraint != NULL ; constraint = constraint_next)
	{
		constraint_next = constraint->next;
		lieng_constraint_free (constraint);
	}
	self->constraints = NULL;

	/* Clear objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
		{
			object = iter.value;
			lieng_object_set_realized (object, 0);
		}
		while (self->objects->list)
		{
			object = self->objects->list->value;
			lieng_object_free (object);
		}
	}

	/* Clear selection. */
	if (self->selection != NULL)
		lieng_engine_clear_selection (self);

	/* Clear resources. */
	if (self->resources != NULL)
		lieng_resources_clear (self->resources);

	/* Invoke callbacks. */
	lical_callbacks_call (self->callbacks, self, "free", lical_marshal_DATA_PTR, self);

	/* Clear sectors. */
	if (self->sectors != NULL)
		lialg_sectors_remove_content (self->sectors, "engine");

	/* Free subsystems. */
	if (self->physics != NULL)
		liphy_physics_free (self->physics);

	if (self->resources != NULL)
		lieng_resources_free (self->resources);
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);
	if (self->selection != NULL)
		lialg_ptrdic_free (self->selection);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self->config.dir);
	lisys_free (self);
}

/**
 * \brief Clears the current selection.
 *
 * \param self Engine.
 */
void
lieng_engine_clear_selection (LIEngEngine* self)
{
	LIAlgPtrdicIter iter;
	LIEngSelection* selection;

	LIALG_PTRDIC_FOREACH (iter, self->selection)
	{
		selection = iter.value;
		lieng_selection_free (selection);
	}
	lialg_ptrdic_clear (self->selection);
}

LIEngAnimation*
lieng_engine_find_animation_by_code (LIEngEngine* self,
                                     int          id)
{
	return lieng_resources_find_animation_by_code (self->resources, id);
}

LIEngAnimation*
lieng_engine_find_animation_by_name (LIEngEngine* self,
                                     const char*  name)
{
	return lieng_resources_find_animation_by_name (self->resources, name);
}

LIEngModel*
lieng_engine_find_model_by_code (LIEngEngine* self,
                                 uint32_t     id)
{
	LIEngModel* model;

	model = lieng_resources_find_model_by_code (self->resources, id);
	if (model == NULL)
		return NULL;
	if (!lieng_model_load (model))
		lisys_error_report ();

	return model;
}

LIEngModel*
lieng_engine_find_model_by_name (LIEngEngine* self,
                                 const char*  name)
{
	LIEngModel* model;

	model = lieng_resources_find_model_by_name (self->resources, name);
	if (model == NULL)
		return NULL;
	if (!lieng_model_load (model))
		lisys_error_report ();

	return model;
}

/**
 * \brief Find an object by id.
 *
 * \param self Engine.
 * \param id Object number.
 * \return Object or NULL.
 */
LIEngObject*
lieng_engine_find_object (LIEngEngine* self,
                          uint32_t     id)
{
	return lialg_u32dic_find (self->objects, id);
}

/**
 * \brief Register a constraint.
 *
 * \param self Engine.
 * \param constraint Constraint.
 */
void
lieng_engine_insert_constraint (LIEngEngine*     self,
                                LIEngConstraint* constraint)
{
	if (self->constraints != NULL)
		self->constraints->prev = constraint;
	constraint->next = self->constraints;
	self->constraints = constraint;
}

/**
 * \brief Forces the engine to reload a model.
 *
 * Reloads the requested model and updates any objects that reference
 * it to reflect the new model. Any other references to the model become
 * invalid and need to be manually replaced.
 *
 * \param self Engine.
 * \param name Model name.
 * \return Nonzero on success.
 */
int
lieng_engine_load_model (LIEngEngine* self,
                         const char*  name)
{
	LIAlgU32dicIter iter;
	LIEngModel* model;
	LIEngObject* object;

	/* Find model. */
	model = lieng_resources_find_model_by_name (self->resources, name);
	if (model == NULL)
		return 0;

	/* Mark affected objects. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = iter.value;
		if (object->model == model)
			object->flags |= LIENG_OBJECT_FLAG_RELOAD;
	}

	/* Reload model. */
	lieng_model_unload (model);
	lieng_model_load (model);
	lical_callbacks_call (self->callbacks, self, "model-reload", lical_marshal_DATA_PTR_PTR, name, model);

	/* Rebuild affected objects. */
	/* TODO: Constraints are currently rebuild by lieng_object_set_model so
	         this loop can result to a lot of unnecessary rebuilds. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = iter.value;
		if (object->flags & LIENG_OBJECT_FLAG_RELOAD)
		{
			object->flags &= ~LIENG_OBJECT_FLAG_RELOAD;
			lieng_object_set_model (object, model);
		}
	}

	return 1;
}

/**
 * \brief Loads the resource list of the engine.
 *
 * If a stream reader is provided, the resource list is loaded from that.
 * Otherwise, the list is constructed by iterating through files in the
 * currently set data directory.
 *
 * \param self Engine.
 * \param reader Reader or NULL.
 * \return Nonzero on success.
 */
int
lieng_engine_load_resources (LIEngEngine* self,
                             LIArcReader* reader)
{
#warning Breaks due to old models being lost if called multiple times.
#warning Should unload and reload models so that the list can change without restarting.
	if (reader != NULL)
		return lieng_resources_load_from_stream (self->resources, reader);
	else
		return lieng_resources_load_from_dir (self->resources, self->config.dir);
}

/**
 * \brief Unregister a constraint.
 *
 * \param self Engine.
 * \param constraint Constraint.
 */
void
lieng_engine_remove_constraint (LIEngEngine*     self,
                                LIEngConstraint* constraint)
{
	if (constraint->next != NULL)
		constraint->next->prev = constraint->prev;
	if (constraint->prev != NULL)
		constraint->prev->next = constraint->next;
	else
		self->constraints = constraint->next;
	constraint->next = NULL;
	constraint->prev = NULL;
}

/**
 * \brief Updates the scene.
 *
 * \param self Engine.
 * \param secs Number of seconds since the last update.
 */
void
lieng_engine_update (LIEngEngine* self,
                     float        secs)
{
	LIAlgSectorsIter siter;
	LIAlgU32dicIter iter;
	LIEngConstraint* constraint;
	LIEngObject* object;
	LIEngSector* sector;

	/* Update sectors. */
	LIALG_SECTORS_FOREACH (siter, self->sectors)
	{
		sector = lialg_strdic_find (siter.sector->content, "engine");
		if (sector != NULL)
			lieng_sector_update (sector, secs);
	}

	/* Update objects. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = iter.value;
		lieng_object_update (object, secs);
	}

	/* Update physics. */
	liphy_physics_update (self->physics, secs);

	/* Maintain callbacks. */
	lical_callbacks_update (self->callbacks);

	/* Update constraints. */
	for (constraint = self->constraints ;
	     constraint != NULL ;
	     constraint = constraint->next)
	{
		lieng_constraint_update (constraint, secs);
	}
}

/**
 * \brief Gets the engine flags.
 *
 * \param self Engine.
 * \return Flags.
 */
int
lieng_engine_get_flags (const LIEngEngine* self)
{
	return self->config.flags;
}

/**
 * \brief Sets the engine flags.
 *
 * \param self Engine.
 * \param flags Flags.
 */
void
lieng_engine_set_flags (LIEngEngine* self,
                        int          flags)
{
	self->config.flags = flags;
}

/**
 * \brief Sets the range of local object IDs.
 *
 * \param self Engine.
 * \param start Range start.
 * \param end Range end.
 */
void
lieng_engine_set_local_range (LIEngEngine* self,
                              uint32_t     start,
                              uint32_t     end)
{
	self->range.start = start;
	self->range.size = end - start;
}

void*
lieng_engine_get_userdata (LIEngEngine* self)
{
	return self->userdata;
}

void
lieng_engine_set_userdata (LIEngEngine* self,
                           void*        value)
{
	self->userdata = value;
}

/*****************************************************************************/

static int
private_init (LIEngEngine* self)
{
	/* Objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
		return 0;

	/* Selection. */
	self->selection = lialg_ptrdic_new ();
	if (self->selection == NULL)
		return 0;

	/* Physics. */
	self->physics = liphy_physics_new (self->callbacks);
	if (self->physics == NULL)
		return 0;
	lical_callbacks_insert (self->callbacks, self->physics, "object-transform", 0, private_physics_transform, self, self->calls + 0);

	/* Resources. */
	self->resources = lieng_resources_new (self);
	if (self->resources == NULL)
		return 0;

	/* Sectors. */
	if (!lialg_sectors_insert_content (self->sectors, "engine", self,
	     	(LIAlgSectorFreeFunc) lieng_sector_free,
	     	(LIAlgSectorLoadFunc) lieng_sector_new))
		return 0;

	return 1;
}

static void
private_physics_transform (LIEngEngine* self,
                           LIPhyObject* object)
{
	LIEngObject* obj;

	obj = liphy_object_get_userdata (object);
	if (obj == NULL || obj->sector == NULL)
		return;
	lieng_object_moved (obj);
}

static void
private_sector_load (void*        self,
                     LIAlgSector* sector)
{
	LIEngEngine* engine = self;
	LIEngSector* sector_;

	/* Invoke callbacks. */
	sector_ = lialg_strdic_find (sector->content, "engine");
	assert (sector_ != NULL);
	lical_callbacks_call (engine->callbacks, self, "sector-load", lical_marshal_DATA_PTR, sector_);
}

/** @} */
/** @} */
