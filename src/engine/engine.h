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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <algorithm/lips-algorithm.h>
#include <archive/lips-archive.h>
#include <callback/lips-callback.h>
#include <math/lips-math.h>
#include <model/lips-model.h>
#include <physics/lips-physics.h>
#include <system/lips-system.h>
#include "engine-model.h"
#include "engine-object.h"
#include "engine-render.h"
#include "engine-resources.h"
#include "engine-sector.h"
#include "engine-types.h"

#ifndef LIENG_DISABLE_GRAPHICS
#include <render/lips-render.h>
#endif

/*****************************************************************************/

typedef struct _liengCalls liengCalls;
struct _liengCalls
{
	liengObject* (*lieng_object_new)(liengEngine*, liengModel*, liphyShapeMode, liphyControlMode, uint32_t, void*);
	void (*lieng_object_free)(liengObject*);
	int (*lieng_object_moved)(liengObject*);
	int (*lieng_object_serialize)(liengObject*, liarcSerialize*);
	void (*lieng_object_update)(liengObject*, float);
	int (*lieng_object_set_model)(liengObject*, liengModel* model);;
	int (*lieng_object_set_realized)(liengObject*, int);
	int (*lieng_object_set_transform)(liengObject*, const limatTransform*);
	int (*lieng_object_set_velocity)(liengObject*, const limatVector*);
};

extern const liengCalls lieng_default_calls;

struct _liengEngine
{
	void* userdata[LIENG_DATA_MAX];
	lialgU32dic* objects;
	lialgU32dic* sectors;
	lialgPtrdic* selection;
	licalCallbacks* callbacks;
	liengCalls calls;
	liengResources* resources;
	liphyPhysics* physics;
#ifndef LIENG_DISABLE_GRAPHICS
	liengRender* renderapi;
	lirndRender* render;
	lirndScene* scene_normal;
	lirndScene* scene_selection;
#endif
	struct
	{
		int flags;
		int radius;
		char* datadir;
		char* dir;
	} config;
	struct
	{
		uint32_t start;
		uint32_t size;
	} range;
};

liengEngine*
lieng_engine_new (const char* datadir,
                  const char* moddir,
                  int         gfx);

void
lieng_engine_free (liengEngine* self);

int
lieng_engine_call (liengEngine* self,
                   licalType    type,
                                ...);

licalHandle
lieng_engine_call_insert (liengEngine* self,
                          licalType    type,
                          int          priority,
                          void*        call,
                          void*        data);

void
lieng_engine_call_remove (liengEngine* self,
                          licalType    type,
                          licalHandle  handle);

void
lieng_engine_clear_selection (liengEngine* self);

liengSector*
lieng_engine_create_sector (liengEngine* self,
                            uint32_t     id);

liengAnimation*
lieng_engine_find_animation_by_code (liengEngine* self,
                                     int          id);

liengAnimation*
lieng_engine_find_animation_by_name (liengEngine* self,
                                     const char*  name);

liengModel*
lieng_engine_find_model_by_code (liengEngine* self,
                                 uint32_t     id);

liengModel*
lieng_engine_find_model_by_name (liengEngine* self,
                                 const char*  name);

liengObject*
lieng_engine_find_object (liengEngine* self,
                          uint32_t     id);

liengSector*
lieng_engine_find_sector (liengEngine* self,
                          uint32_t     id);

int
lieng_engine_load_model (liengEngine* self,
                         const char*  name);

int
lieng_engine_load_texture (liengEngine* self,
                           const char*  name);

liengSector*
lieng_engine_load_sector (liengEngine* self,
                          uint32_t     id);

int
lieng_engine_read_object (liengEngine*    self,
                          liarcSerialize* serialize,
                          liengObject**   value);

int
lieng_engine_load_resources (liengEngine* self,
                             liReader*    reader);

int
lieng_engine_read_object_data (liengEngine*    self,
                               liarcSerialize* serialize,
                               liengObject*    value);

void
lieng_engine_update (liengEngine* self,
                     float        secs);


int
lieng_engine_save (liengEngine* self);

int
lieng_engine_write_object (liengEngine*    self,
                           liarcSerialize* serialize,
                           liengObject*    value);

liengCalls*
lieng_engine_get_calls (liengEngine* self);

int
lieng_engine_set_center (liengEngine*       self,
                         const limatVector* center);

int
lieng_engine_get_flags (const liengEngine* self);

void
lieng_engine_set_flags (liengEngine* self,
                        int          flags);

void
lieng_engine_set_local_range (liengEngine* self,
                              uint32_t     start,
                              uint32_t     end);

void*
lieng_engine_get_scene (liengEngine*   self,
                        liengSceneType type);

void*
lieng_engine_get_userdata (liengEngine* self,
                           int          type);

void
lieng_engine_set_userdata (liengEngine* self,
                           int          type,
                           void*        data);

#endif

/** @} */
/** @} */
