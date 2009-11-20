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

#ifndef __ENGINE_OBJECT_H__
#define __ENGINE_OBJECT_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>
#include <physics/lips-physics.h>
#include <system/lips-system.h>
#include "engine-model.h"
#include "engine-types.h"

#ifndef LIENG_DISABLE_SCRIPT
#include <script/lips-script.h>
#endif

#define LIENG_OBJECT(o) ((liengObject*)(o))

struct _liengObject
{
	int refs;
	uint32_t id;
	uint16_t flags;
	void* userdata[LIENG_DATA_MAX];
	liengEngine* engine;
	liengModel* model;
	liengSector* sector;
	limdlPose* pose;
	liphyObject* physics;
#ifndef LIENG_DISABLE_SCRIPTS
	liscrData* script;
#endif
	struct
	{
		float pos;
		float rot;
		limatTransform target;
	} smoothing;
};

liengObject*
lieng_object_new (liengEngine*     engine,
                  liengModel*      model,
                  liphyControlMode control_mode,
                  uint32_t         id,
                  void*            data);

void
lieng_object_free (liengObject* self);

void
lieng_object_ref (liengObject* self,
                  int          count);

void
lieng_object_approach (liengObject*       self,
                       const limatVector* target,
                       float              speed);

limdlNode*
lieng_object_find_node (liengObject* self,
                        const char*  name);

void
lieng_object_impulse (liengObject*       self,
                      const limatVector* point,
                      const limatVector* impulse);

void
lieng_object_jump (liengObject*       self,
                   const limatVector* impulse);

void
lieng_object_update (liengObject* self,
                     float        secs);

void
lieng_object_get_angular_momentum (const liengObject* self,
                                   limatVector*       value);

void
lieng_object_set_angular_momentum (liengObject*       self,
                                   const limatVector* value);

void
lieng_object_set_animation (liengObject* self,
                            int          channel,
                            const char*  animation,
                            int          repeats,
                            float        priority);

void
lieng_object_get_bounds (const liengObject* self,
                         limatAabb*         bounds);

void
lieng_object_get_bounds_transform (const liengObject* self,
                                   limatAabb*         bounds);

int
lieng_object_get_collision_group (const liengObject* self);

void
lieng_object_set_collision_group (liengObject* self,
                                  int          mask);

int
lieng_object_get_collision_mask (const liengObject* self);

void
lieng_object_set_collision_mask (liengObject* self,
                                 int          mask);

int
lieng_object_get_dirty (const liengObject* self);

void
lieng_object_set_dirty (liengObject* self,
                        int          value);

float
lieng_object_get_distance (const liengObject* self,
                           const liengObject* object);

int
lieng_object_get_flags (const liengObject* self);

void
lieng_object_set_flags (liengObject* self,
                        int          flags);

int
lieng_object_get_ground (const liengObject* self);

float
lieng_object_get_mass (const liengObject* self);

void
lieng_object_set_mass (liengObject* self,
                       float        value);

int
lieng_object_set_model (liengObject* self,
                        liengModel*  model);

int
lieng_object_get_model_code (const liengObject* self);

int
lieng_object_set_model_code (liengObject* self,
                             int          value);

const char*
lieng_object_get_model_name (const liengObject* self);

int
lieng_object_set_model_name (liengObject* self,
                             const char*  value);

int
lieng_object_get_realized (const liengObject* self);

int
lieng_object_set_realized (liengObject* self,
                           int          value);

liengSector*
lieng_object_get_sector (liengObject* self);

int
lieng_object_get_selected (const liengObject* self);

int
lieng_object_set_selected (liengObject* self,
                           int          select);

void
lieng_object_set_shape (liengObject* self,
                        liphyShape*  shape);

void
lieng_object_set_smoothing (liengObject* self,
                            float        pos,
                            float        rot);

float
lieng_object_get_speed (const liengObject* self);

void
lieng_object_set_speed (liengObject* self,
                        float        value);

void
lieng_object_get_target (const liengObject* self,
                         limatTransform*    value);

void
lieng_object_get_transform (const liengObject* self,
                            limatTransform*    value);

int
lieng_object_set_transform (liengObject*          self,
                            const limatTransform* value);

void*
lieng_object_get_userdata (liengObject* self,
                           int          type);

void
lieng_object_set_userdata (liengObject* self,
                           int          type,
                           void*        data);

void
lieng_object_get_velocity (const liengObject* self,
                           limatVector*       velocity);

int
lieng_object_set_velocity (liengObject*       self,
                           const limatVector* velocity);

#endif

/** @} */
/** @} */
