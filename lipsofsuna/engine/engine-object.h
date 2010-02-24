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
 * \addtogroup LIEngObject Object
 * @{
 */

#ifndef __ENGINE_OBJECT_H__
#define __ENGINE_OBJECT_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/system.h>
#include "engine-model.h"
#include "engine-types.h"

#ifndef LIENG_DISABLE_SCRIPT
#include <lipsofsuna/script.h>
#endif

#define LIENG_OBJECT(o) ((LIEngObject*)(o))

struct _LIEngObject
{
	int refs;
	void* userdata;
	uint32_t id;
	uint16_t flags;
	LIEngEngine* engine;
	LIEngModel* model;
	LIEngSector* sector;
	LIMdlPose* pose;
	LIPhyObject* physics;
#ifndef LIENG_DISABLE_SCRIPTS
	LIScrData* script;
#endif
	struct
	{
		float pos;
		float rot;
		LIMatTransform target;
	} smoothing;
};

LIEngObject*
lieng_object_new (LIEngEngine*     engine,
                  LIEngModel*      model,
                  LIPhyControlMode control,
                  uint32_t         id);

void
lieng_object_free (LIEngObject* self);

void
lieng_object_ref (LIEngObject* self,
                  int          count);

int
lieng_object_approach (LIEngObject*       self,
                       const LIMatVector* target,
                       float              speed,
                       float              dist);

LIMdlNode*
lieng_object_find_node (LIEngObject* self,
                        const char*  name);

void
lieng_object_impulse (LIEngObject*       self,
                      const LIMatVector* point,
                      const LIMatVector* impulse);

void
lieng_object_jump (LIEngObject*       self,
                   const LIMatVector* impulse);

int
lieng_object_moved (LIEngObject* self);

void
lieng_object_refresh (LIEngObject* self,
                      float        radius);

void
lieng_object_update (LIEngObject* self,
                     float        secs);

void
lieng_object_get_angular_momentum (const LIEngObject* self,
                                   LIMatVector*       value);

void
lieng_object_set_angular_momentum (LIEngObject*       self,
                                   const LIMatVector* value);

void
lieng_object_set_animation (LIEngObject* self,
                            int          channel,
                            const char*  animation,
                            int          repeats,
                            float        priority);

void
lieng_object_get_bounds (const LIEngObject* self,
                         LIMatAabb*         bounds);

void
lieng_object_get_bounds_transform (const LIEngObject* self,
                                   LIMatAabb*         bounds);

int
lieng_object_get_collision_group (const LIEngObject* self);

void
lieng_object_set_collision_group (LIEngObject* self,
                                  int          mask);

int
lieng_object_get_collision_mask (const LIEngObject* self);

void
lieng_object_set_collision_mask (LIEngObject* self,
                                 int          mask);

int
lieng_object_get_dirty (const LIEngObject* self);

void
lieng_object_set_dirty (LIEngObject* self,
                        int          value);

float
lieng_object_get_distance (const LIEngObject* self,
                           const LIEngObject* object);

int
lieng_object_get_flags (const LIEngObject* self);

void
lieng_object_set_flags (LIEngObject* self,
                        int          flags);

int
lieng_object_get_ground (const LIEngObject* self);

float
lieng_object_get_mass (const LIEngObject* self);

void
lieng_object_set_mass (LIEngObject* self,
                       float        value);

int
lieng_object_set_model (LIEngObject* self,
                        LIEngModel*  model);

int
lieng_object_get_model_code (const LIEngObject* self);

int
lieng_object_set_model_code (LIEngObject* self,
                             int          value);

const char*
lieng_object_get_model_name (const LIEngObject* self);

int
lieng_object_set_model_name (LIEngObject* self,
                             const char*  value);

int
lieng_object_get_realized (const LIEngObject* self);

int
lieng_object_set_realized (LIEngObject* self,
                           int          value);

LIEngSector*
lieng_object_get_sector (LIEngObject* self);

int
lieng_object_get_selected (const LIEngObject* self);

int
lieng_object_set_selected (LIEngObject* self,
                           int          select);

void
lieng_object_set_shape (LIEngObject* self,
                        LIPhyShape*  shape);

void
lieng_object_set_smoothing (LIEngObject* self,
                            float        pos,
                            float        rot);

float
lieng_object_get_speed (const LIEngObject* self);

void
lieng_object_set_speed (LIEngObject* self,
                        float        value);

void
lieng_object_get_target (const LIEngObject* self,
                         LIMatTransform*    value);

void
lieng_object_get_transform (const LIEngObject* self,
                            LIMatTransform*    value);

int
lieng_object_set_transform (LIEngObject*          self,
                            const LIMatTransform* value);

void*
lieng_object_get_userdata (LIEngObject* self);

void
lieng_object_set_userdata (LIEngObject* self,
                           void*        data);

void
lieng_object_get_velocity (const LIEngObject* self,
                           LIMatVector*       velocity);

int
lieng_object_set_velocity (LIEngObject*       self,
                           const LIMatVector* velocity);

#endif

/** @} */
/** @} */
