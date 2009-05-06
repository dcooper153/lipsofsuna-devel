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
#include <sector/lips-sector.h>
#include "engine.h"
#include "engine-selection.h"

static int
private_init (liengEngine* self,
              int          gfx);

static void
private_clear_sectors (liengEngine* self);

static void
private_physics_transform (liphyObject* object);

#ifndef LIENG_DISABLE_GRAPHICS
static int
private_scene_begin_realized (lirndSceneIter* iter,
                              lirndScene*     scene);

static int
private_scene_begin_selected (lirndSceneIter* iter,
                              lirndScene*     scene);

static int
private_scene_next_realized (lirndSceneIter* iter);

static int
private_scene_next_selected (lirndSceneIter* iter);
#endif

/*****************************************************************************/

/**
 * \brief Creates a new game engine.
 *
 * \param dir Data directory root.
 * \param gfx Nonzero if should load graphics.
 * \return New engine or NULL.
 */
liengEngine*
lieng_engine_new (const char* dir,
                  int         gfx)
{
	liengEngine* self;

	self = calloc (1, sizeof (liengEngine));
	if (self == NULL)
		return NULL;
	self->range.start = 0;
	self->range.size = 0xFFFFFFFF;
	self->calls = lieng_default_calls;
	self->config.radius = 1;
	self->config.dir = strdup (dir);
	if (self->config.dir == NULL)
		goto error;

	if (!private_init (self, gfx))
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
	liengObject* object;

	/* Unrealize objects. */
	if (self->objects != NULL)
	{
		LI_FOREACH_U32DIC (iter, self->objects)
		{
			object = iter.value;
			lieng_object_set_realized (object, 0);
		}
	}

	/* Free objects. */
	if (self->objects != NULL)
	{
		while (self->objects->list)
		{
			object = self->objects->list->value;
			lieng_object_free (object);
		}
		lialg_u32dic_free (self->objects);
	}

	/* Free selection. */
	if (self->selection != NULL)
	{
		lieng_engine_clear_selection (self);
		lialg_ptrdic_free (self->selection);
	}

	/* Free sectors. */
	if (self->sectors != NULL)
	{
		private_clear_sectors (self);
		lialg_u32dic_free (self->sectors);
	}

	/* Free callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_free (self->callbacks);

	/* Free resource list. */
	if (self->resources != NULL)
		lieng_resources_free (self->resources);

	/* Free subsystems. */
	if (self->physics != NULL)
		liphy_physics_free (self->physics);
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->renderapi != NULL)
		lieng_render_free (self->renderapi);
#endif

	free (self);
}

int
lieng_engine_call (liengEngine* self,
                   licalType    type,
                                ...)
{
	int ret;
	va_list args;

	va_start (args, type);
	ret = lical_callbacks_callva (self->callbacks, type, args);
	va_end (args);

	return ret;
}

licalHandle
lieng_engine_call_insert (liengEngine* self,
                          licalType    type,
                          int          priority,
                          void*        call,
                          void*        data)
{
	return lical_callbacks_insert_callback (self->callbacks, type, priority, call, data);
}

void
lieng_engine_call_remove (liengEngine* self,
                          licalType    type,
                          licalHandle  handle)
{
	lical_callbacks_remove_callback (self->callbacks, type, handle);
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
	liengModel dummy;
	liengModel* model;
	liengObject* object;

	if (self->render == NULL)
		return 1;

	/* Find model. */
	model = lieng_resources_find_model_by_name (self->resources, name);
	if (model == NULL)
		return 0;

	/* Remove old instances. */
	memset (&dummy, 0, sizeof (dummy));
	dummy.invalid = 1;
	dummy.engine = self;
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		if (object->model == model)
		{
			lieng_object_set_model (object, NULL);
			lieng_object_set_model (object, &dummy);
		}
	}

	/* Reload model. */
	lieng_model_unload (model);
	lieng_model_load (model);

	/* Create new instances. */
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		if (object->model == &dummy)
			lieng_object_set_model (object, model);
	}

	return 1;
}

/**
 * \brief Forces the engine to reload a texture.
 *
 * Reloads the requested texture and updates any objects that reference
 * it to reflect the new texture. Any other references to the texture become
 * invalid and need to be manually replaced.
 *
 * \param self Engine.
 * \param name Texture name.
 * \return Nonzero on success.
 */
int
lieng_engine_load_texture (liengEngine* self,
                           const char*  name)
{
#ifndef LIENG_DISABLE_GRAPHICS
	lialgU32dicIter iter;
	liengObject* object;
	lirndImage dummy;
	lirndImage* image;

	/* Find old texture. */
	image = self->renderapi->lirnd_render_find_image (self->render, name);
	if (image == NULL)
		return 1;

	/* Remove old instances. */
	memset (&dummy, 0, sizeof (dummy));
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		self->renderapi->lirnd_object_replace_image (object->render, image, &dummy);
	}

	/* Reload the texture. */
	if (!self->renderapi->lirnd_render_load_image (self->render, name))
	{
		LI_FOREACH_U32DIC (iter, self->objects)
		{
			object = iter.value;
			self->renderapi->lirnd_object_replace_image (object->render, &dummy, image);
		}
		return 0;
	}
	image = self->renderapi->lirnd_render_find_image (self->render, name);

	/* Create new instances. */
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		self->renderapi->lirnd_object_replace_image (object->render, &dummy, image);
	}
#endif

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
                             liReader*    reader)
{
#warning Breaks due to old models being lost if called multiple times.
#warning Should unload and reload models so that the list can change without restarting.
	if (reader != NULL)
		return lieng_resources_load_from_stream (self->resources, reader);
	else
		return lieng_resources_load_from_dir (self->resources, self->config.dir);
}

/**
 * \brief Deserializes a serialized object.
 *
 * \param self Engine.
 * \param serialize Deserializer.
 * \param value Return location for the deserialized object.
 * \return Nonzero on success.
 */
int
lieng_engine_read_object (liengEngine*    self,
                          liarcSerialize* serialize,
                          liengObject**   value)
{
	uint8_t mode;
	uint32_t id;

	if (!li_reader_get_uint8 (serialize->reader, &mode))
		return 0;
	switch (mode)
	{
		case 0:
			*value = NULL;
			return 1;
		case 1:
			if (!li_reader_get_uint32 (serialize->reader, &id))
				return 0;
			*value = liarc_serialize_find_object (serialize, id);
			if (*value == NULL)
			{
				lisys_error_set (EINVAL, "invalid object reference %d", id);
				return 0;
			}
			return 1;
		case 2:
			if (!li_reader_get_uint32 (serialize->reader, &id))
				return 0;
			*value = liarc_serialize_find_object (serialize, id);
			if (*value != NULL)
			{
				lisys_error_set (EINVAL, "duplicate object id %d", id);
				return 0;
			}
			*value = lieng_object_new (self, NULL, LIPHY_SHAPE_MODE_CONVEX, LIPHY_CONTROL_MODE_RIGID, 0, NULL);
			if (*value == NULL)
				return 0;
			if (!liarc_serialize_insert_object (serialize, *value))
			{
				lieng_object_free (*value);
				return 0;
			}
			if (!lieng_object_serialize (*value, serialize))
			{
				lieng_object_free (*value);
				return 0;
			}
			return 1;
		default:
			lisys_error_set (EINVAL, "invalid object save data");
			return 0;
	}
}

/**
 * \brief Incrementally deserializes object fields.
 *
 * \param self Engine.
 * \param serialize Deserializer.
 * \param value Object to modify.
 * \return Nonzero on success.
 */
int
lieng_engine_read_object_data (liengEngine*    self,
                               liarcSerialize* serialize,
                               liengObject*    value)
{
	uint8_t mode;
	uint32_t id;
	liengObject* tmp;

	if (!li_reader_get_uint8 (serialize->reader, &mode))
		return 0;
	switch (mode)
	{
		case 0:
			lisys_error_set (EINVAL, "expected object data, got NULL");
			return 0;
		case 1:
			lisys_error_set (EINVAL, "expected object data, got reference");
			return 0;
		case 2:
			if (!li_reader_get_uint32 (serialize->reader, &id))
				return 0;
			tmp = liarc_serialize_find_object (serialize, id);
			if (tmp != NULL)
			{
				lisys_error_set (EINVAL, "duplicate object id %d", id);
				return 0;
			}
			if (!liarc_serialize_insert_object (serialize, value))
				return 0;
			if (!lieng_object_serialize (value, serialize))
				return 0;
			return 1;
		default:
			lisys_error_set (EINVAL, "invalid object save data");
			return 0;
	}
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
	liengObject* object;

	/* Update objects. */
	LI_FOREACH_U32DIC (iter, self->objects)
	{
		object = iter.value;
		lieng_object_update (object, secs);
	}

	/* Update physics. */
	liphy_physics_update (self->physics, secs);

	/* Update renderer state. */
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->renderapi != NULL)
		self->renderapi->lirnd_render_update (self->render, secs);
#endif

	/* Maintain callbacks. */
	lical_callbacks_update (self->callbacks);
}

/**
 * \brief Saves all the currently loaded sectors.
 *
 * \param self Engine.
 * \return Nonzero on success.
 */
int
lieng_engine_save (liengEngine* self)
{
	int ret;
	lialgU32dicIter iter;
	liengSector* sector;

	ret = 1;
	LI_FOREACH_U32DIC (iter, self->sectors)
	{
		sector = iter.value;
		if (!lieng_sector_save (sector))
			ret = 0;
	}

	return ret;
}

/**
 * \brief Serializes an object.
 *
 * \param self Engine.
 * \param serialize Serializer.
 * \param value Object to be serialized.
 * \return Nonzero on success.
 */
int
lieng_engine_write_object (liengEngine*    self,
                           liarcSerialize* serialize,
                           liengObject*    value)
{
	int id;

	if (value == NULL)
	{
		liarc_writer_append_uint8 (serialize->writer, 0);
		return 1;
	}
	id = liarc_serialize_find_object_id (serialize, value);
	if (id != -1)
	{
		liarc_writer_append_uint8 (serialize->writer, 1);
		liarc_writer_append_uint32 (serialize->writer, id);
	}
	else
	{
		liarc_writer_append_uint8 (serialize->writer, 2);
		if (!liarc_serialize_insert_object (serialize, value))
			return 0;
		id = liarc_serialize_find_object_id (serialize, value);
		assert (id != -1);
		liarc_writer_append_uint32 (serialize->writer, id);
		if (!lieng_object_serialize (value, serialize))
			return 0;
	}

	return 1;
}

liengCalls*
lieng_engine_get_calls (liengEngine* self)
{
	return &self->calls;
}

/**
 * \brief Sets the center point of the scene.
 *
 * \param self Engine.
 * \param center Center position vector.
 */
int
lieng_engine_set_center (liengEngine*       self,
                         const limatVector* center)
{
	int error = 0;
	uint32_t id;
	lialgU32dicIter iter_ptr;
	liengSector* sector;
	lisecIter iter_sec;

	/* Get new center sector. */
	id = lisec_pointer_new (center);

	/* Remove sectors. */
	LI_FOREACH_U32DIC (iter_ptr, self->sectors)
	{
		sector = iter_ptr.value;
		if (!lisec_pointer_get_nearby (id, sector->id, self->config.radius))
			lieng_sector_free (sector);
	}

	/* Insert sectors. */
	LI_FOREACH_SECTOR (iter_sec, id, self->config.radius)
	{
		sector = lialg_u32dic_find (self->sectors, iter_sec.id);
		if (sector != NULL)
			continue;
		sector = lieng_sector_new (self, iter_sec.id, self->config.dir);
		if (sector == NULL)
		{
			lisys_error_report ();
			continue;
		}
	}

	return error;
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
lieng_engine_get_scene (liengEngine*   self,
                        liengSceneType type)
{
#ifndef LIENG_DISABLE_GRAPHICS
	switch (type)
	{
		case LIENG_SCENE_NORMAL:
			return self->scene_normal;
		case LIENG_SCENE_SELECTION:
			return self->scene_selection;
		default:
			assert (0);
			return NULL;
	}
#else
	return NULL;
#endif
}

void*
lieng_engine_get_userdata (liengEngine* self,
                           int          type)
{
	return self->userdata[type];
}

void
lieng_engine_set_userdata (liengEngine* self,
                           int          type,
                           void*        data)
{
	self->userdata[type] = data;
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
private_init (liengEngine* self,
              int          gfx)
{
	/* Callbacks. */
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
		return 0;

	/* Objects. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
		return 0;

	/* Selection. */
	self->selection = lialg_ptrdic_new ();
	if (self->selection == NULL)
		return 0;

	/* Physics. */
	self->physics = liphy_physics_new ();
	if (self->physics == NULL)
		return 0;
	liphy_physics_set_transform_callback (self->physics, private_physics_transform);

	/* Render. */
#ifndef LIENG_DISABLE_GRAPHICS
	if (gfx)
	{
		self->renderapi = lieng_render_new (self->config.dir);
		if (self->renderapi == NULL)
			return 0;
		self->render = self->renderapi->render;
		self->scene_normal = calloc (1, sizeof (lirndScene));
		if (self->scene_normal == NULL)
			return 0;
		self->scene_normal->data = self;
		self->scene_normal->begin = private_scene_begin_realized;
		self->scene_normal->next = private_scene_next_realized;
		self->scene_selection = calloc (1, sizeof (lirndScene));
		if (self->scene_selection == NULL)
			return 0;
		self->scene_selection->data = self;
		self->scene_selection->begin = private_scene_begin_selected;
		self->scene_selection->next = private_scene_next_selected;
	}
#endif

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
private_physics_transform (liphyObject* object)
{
	liengObject* obj;

	obj = liphy_object_get_userdata (object);
	if (obj == NULL || obj->sector == NULL)
		return;
	obj->engine->calls.lieng_object_moved (obj);
}

#ifndef LIENG_DISABLE_GRAPHICS
static int
private_scene_begin_realized (lirndSceneIter* iter,
                              lirndScene*     scene)
{
	lialgU32dicIter* u32i = (lialgU32dicIter*) iter->data;
	liengEngine* self = scene->data;
	liengObject* object;

	iter->scene = scene;

	/* Find first realized object. */
	for (lialg_u32dic_iter_start (u32i, self->objects) ; u32i->value != NULL ;
	     lialg_u32dic_iter_next (u32i))
	{
		if (lieng_object_get_realized (u32i->value))
		{
			object = u32i->value;
			iter->value = object->render;
			return 1;
		}
	}
	iter->value = NULL;

	return 0;
}

static int
private_scene_begin_selected (lirndSceneIter* iter,
                              lirndScene*     scene)
{
	lialgPtrdicIter* ptri = (lialgPtrdicIter*) iter->data;
	liengEngine* self = scene->data;
	liengSelection* selection;

	iter->scene = scene;
	lialg_ptrdic_iter_start (ptri, self->selection);

	selection = ptri->value;
	if (selection != NULL)
		iter->value = selection->object->render;
	else
		iter->value = NULL;

	return 1;
}

static int
private_scene_next_realized (lirndSceneIter* iter)
{
	lialgU32dicIter* u32i = (lialgU32dicIter*) iter->data;
	liengObject* object;

	/* Find next realized object. */
	for (lialg_u32dic_iter_next (u32i) ; u32i->value != NULL ;
	     lialg_u32dic_iter_next (u32i))
	{
		if (lieng_object_get_realized (u32i->value))
		{
			object = u32i->value;
			iter->value = object->render;
			return 1;
		}
	}
	iter->value = NULL;

	return 0;
}

static int
private_scene_next_selected (lirndSceneIter* iter)
{
	int ret;
	lialgPtrdicIter* ptri = (lialgPtrdicIter*) iter->data;
	liengSelection* selection;

	ret = lialg_ptrdic_iter_next (ptri);

	selection = ptri->value;
	if (selection != NULL)
		iter->value = selection->object->render;
	else
		iter->value = NULL;

	return ret;
}
#endif

/** @} */
/** @} */
