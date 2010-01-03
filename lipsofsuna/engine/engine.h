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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/archive.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/system.h>
#include "engine-model.h"
#include "engine-object.h"
#include "engine-resources.h"
#include "engine-sector.h"
#include "engine-types.h"

/*****************************************************************************/

struct _LIEngEngine
{
	void* userdata;
	LIAlgPtrdic* selection;
	LIAlgSectors* sectors;
	LIAlgU32dic* objects;
	LICalCallbacks* callbacks;
	LICalHandle calls[1];
	LIEngConstraint* constraints;
	LIEngResources* resources;
	LIPhyPhysics* physics;
	struct
	{
		int flags;
		int radius;
		char* dir;
	} config;
	struct
	{
		uint32_t start;
		uint32_t size;
	} range;
};

LIEngEngine*
lieng_engine_new (LICalCallbacks* calls,
                  LIAlgSectors*   sectors,
                  const char*     path);

void
lieng_engine_free (LIEngEngine* self);

void
lieng_engine_clear_selection (LIEngEngine* self);

LIEngAnimation*
lieng_engine_find_animation_by_code (LIEngEngine* self,
                                     int          id);

LIEngAnimation*
lieng_engine_find_animation_by_name (LIEngEngine* self,
                                     const char*  name);

LIEngModel*
lieng_engine_find_model_by_code (LIEngEngine* self,
                                 uint32_t     id);

LIEngModel*
lieng_engine_find_model_by_name (LIEngEngine* self,
                                 const char*  name);

LIEngObject*
lieng_engine_find_object (LIEngEngine* self,
                          uint32_t     id);

void
lieng_engine_insert_constraint (LIEngEngine*     self,
                                LIEngConstraint* constraint);

int
lieng_engine_load_model (LIEngEngine* self,
                         const char*  name);

int
lieng_engine_load_resources (LIEngEngine* self,
                             LIArcReader* reader);

void
lieng_engine_remove_constraint (LIEngEngine*     self,
                                LIEngConstraint* constraint);

void
lieng_engine_update (LIEngEngine* self,
                     float        secs);

int
lieng_engine_get_flags (const LIEngEngine* self);

void
lieng_engine_set_flags (LIEngEngine* self,
                        int          flags);

void
lieng_engine_set_local_range (LIEngEngine* self,
                              uint32_t     start,
                              uint32_t     end);

void*
lieng_engine_get_userdata (LIEngEngine* self);

void
lieng_engine_set_userdata (LIEngEngine* self,
                           void*        value);

#endif

/** @} */
/** @} */
