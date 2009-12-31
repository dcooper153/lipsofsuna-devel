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
 * \addtogroup liengEngine Engine
 * @{
 */

#include <stdarg.h>
#include "engine.h"
#include "engine-constraint.h"
#include "engine-iterator.h"
#include "engine-selection.h"

static int
private_init (liengEngine* self);

static void
private_clear_sectors (liengEngine* self);

static void
private_physics_transform (liengEngine* self,
                           liphyObject* object);

/*****************************************************************************/

/**
 * \brief Creates a new game engine.
 *
 * \param calls Callback manager.
 * \param path Module directory.
 * \return New engine or NULL.
 */
liengEngine*
lieng_engine_new (licalCallbacks* calls,
                  const char*     path)
{
	liengEngine* self;

	self = lisys_calloc (1, sizeof (liengEngine));
	if (self == NULL)
		return NULL;
	self->callbacks = calls;
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
lieng_engine_free (liengEngine* self)
{
	lialgU32dicIter iter;
	liengConstraint* constraint;
	liengConstraint* constraint_next;
	liengObject* object;

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
		LI_FOREACH_U32DIC (iter, self->objects)
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

	/* Clear sectors. */
	if (self->sectors != NULL)
		private_clear_sectors (self);

	/* Clear resources. */
	if (self->resources != NULL)
		lieng_resources_clear (self->resources);

	/* Invoke callbacks. */
	lical_callbacks_call (self->callbacks, self, "free", lical_marshal_DATA_PTR, self);

	/* Free subsystems. */
	if (self->physics != NULL)
		liphy_physics_free (self->physics);

	if (self->resources != NULL)
		lieng_resources_free (self->resources);
	if (self->sectors != NULL)
		lialg_u32dic_free (self->sectors);
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);
	if (self->selection != NULL)
		lialg_ptrdic_free (self->selection);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (licalHandle));
	lisys_free (self->config.dir);
	lisys_free (self);
}

/**
 * \brief Clears the current selection.
 *
 * \param self Engine.
 */
void
lieng_engine_clear_selection (liengEngine* self)
{
	lialgPtrdicIter iter;
	liengSelection* selection;

	LI_FOREACH_PTRDIC (iter, self->selection)
	{
		selection = iter.value;
		lieng_selection_free (selection);
	}
	lialg_ptrdic_clear (self->selection);
}

/**
 * \brief Finds or creates a sector.
 *
 * Finds an existing sector or creates an empty one on demand. This is used by
 * various map editing facilities of the server to ensure that edited sectors
 * aren't swapped out.
 *
 * \param self Engine.
 * \param id Sector number.
 * \return Sector or NULL.
 */
liengSector*
lieng_engine_create_sector (liengEngine* self,
                            uint32_t     id)
{
	liengSector* sector;

	sector = lialg_u32dic_find (self->sectors, id);
	if (sector != NULL)
		return sector;
	sector = lieng_sector_new (self, id);

	return sector;
}

liengAnimation*
lieng_engine_find_animation_by_code (liengEngine* self,
                                     int          id)
{
	return lieng_resources_find_animation_by_code (self->resources, id);
}

liengAnimation*
lieng_engine_find_animation_by_name (liengEngine* self,
                                     const char*  name)
{
	return lieng_resources_find_animation_by_name (self->resources, name);
}

liengModel*
lieng_engine_find_model_by_code (liengEngine* self,
                                 uint32_t     id)
{
	liengModel* model;

	model = lieng_resources_find_model_by_code (self->resources, id);
	if (model == NULL)
		return NULL;
	if (!lieng_model_load (model))
		lisys_error_report ();

	return model;
}

liengModel*
lieng_engine_find_model_by_name (liengEngine* self,
                                 const char*  name)
{
	liengModel* model;

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
liengObject*
lieng_engine_find_object (liengEngine* self,
                          uint32_t     id)
{
	return lialg_u32dic_find (self->objects, id);
}

/**
 * \brief Gets a sector by id.
 *
 * \param self Engine.
 * \param id Sector number.
 * \return Sector or NULL.
 */
liengSector*
lieng_engine_find_sector (liengEngine* self,
                          uint32_t     id)
{
	return lialg_u32dic_find (self->sectors, id);
}

/**
 * \brief Register a constraint.
 *
 * \param self Engine.
 * \param constraint Constraint.
 */
void
lieng_engine_insert_constraint (liengEngine*     self,
                                liengConstraint* constraint)
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
lieng_engine_load_model (liengEngine* self,
                         const char*  name)
{
	lialgU32dicIter iter;
	liengModel* model;
	liengObject* object;

	/* Find model. */
	model = lieng_resources_find_model_by_name (self->resources, name);
	if (model == NULL)
		return 0;

	/* Mark affected objects. */
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		if (object->model == model)
			object->flags |= LIENG_OBJECT_FLAG_RELOAD;
	}

	/* Reload model. */
	lieng_model_unload (model);
	lieng_model_load (model);

	/* Rebuild affected objects. */
	/* TODO: Constraints are currently rebuild by lieng_object_set_model so
	         this loop can result to a lot of unnecessary rebuilds. */
	LI_FOREACH_U32DIC (iter, self->objects)
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
lieng_engine_load_resources (liengEngine* self,
                             liarcReader* reader)
{
#warning Breaks due to old models being lost if called multiple times.
#warning Should unload and reload models so that the list can change without restarting.
	if (reader != NULL)
		return lieng_resources_load_from_stream (self->resources, reader);
	else
		return lieng_resources_load_from_dir (self->resources, self->config.dir);
}

/**
 * \brief Finds or loads a sector.
 *
 * Finds an existing sector or loads one from the disk on demand. This is used
 * by various map editing facilities of the server to ensure that edited sectors
 * aren't swapped out.
 *
 * \param self Engine.
 * \param id Sector number.
 * \return Sector or NULL.
 */
liengSector*
lieng_engine_load_sector (liengEngine* self,
                          uint32_t     id)
{
	liengSector* sector;

	sector = lialg_u32dic_find (self->sectors, id);
	if (sector != NULL)
		return sector;
	sector = lieng_sector_new (self, id);

	return sector;
}

/**
 * \brief Unregister a constraint.
 *
 * \param self Engine.
 * \param constraint Constraint.
 */
void
lieng_engine_remove_constraint (liengEngine*     self,
                                liengConstraint* constraint)
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
lieng_engine_update (liengEngine* self,
                     float        secs)
{
	lialgU32dicIter iter;
	liengConstraint* constraint;
	liengObject* object;
	liengSector* sector;

	/* Update sectors. */
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		lieng_sector_update (sector, secs);
	}

	/* Update objects. */
	LI_FOREACH_U32DIC (iter, self->objects)
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
lieng_engine_get_flags (const liengEngine* self)
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
lieng_engine_set_flags (liengEngine* self,
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
lieng_engine_set_local_range (liengEngine* self,
                              uint32_t     start,
                              uint32_t     end)
{
	self->range.start = start;
	self->range.size = end - start;
}

void*
lieng_engine_get_userdata (liengEngine* self)
{
	return self->userdata;
}

void
lieng_engine_set_userdata (liengEngine* self,
                           void*        value)
{
	self->userdata = value;
}

/*****************************************************************************/

static void
private_clear_sectors (liengEngine* self)
{
	lialgU32dicIter iter;
	liengSector* sector;

	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		lieng_sector_free (sector);
	}
	lialg_u32dic_clear (self->sectors);
}

static int
private_init (liengEngine* self)
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
	self->sectors = lialg_u32dic_new ();
	if (self->sectors == NULL)
		return 0;

	return 1;
}

static void
private_physics_transform (liengEngine* self,
                           liphyObject* object)
{
	liengObject* obj;

	obj = liphy_object_get_userdata (object);
	if (obj == NULL || obj->sector == NULL)
		return;
	lieng_object_moved (obj);
}

/** @} */
/** @} */
