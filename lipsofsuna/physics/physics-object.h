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
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphyObject Object
 * @{
 */

#ifndef __PHYSICS_OBJECT_H__
#define __PHYSICS_OBJECT_H__

#include <math/lips-math.h>
#include <system/lips-system.h>
#include "physics.h"
#include "physics-shape.h"
#include "physics-types.h"

LI_BEGIN_DECLS

liphyObject*
liphy_object_new (liphyPhysics*    physics,
                  liphyShape*      shape,
                  liphyShapeMode   shape_mode,
                  liphyControlMode control_mode);

void
liphy_object_free (liphyObject* self);

void
liphy_object_clear_shape (liphyObject* self);

void
liphy_object_impulse (liphyObject*       self,
                      const limatVector* point,
                      const limatVector* impulse);

int
liphy_object_insert_shape (liphyObject*       self,
                           liphyShape*        shape,
                           liphyShapeMode     mode,
                           const limatVector* origin);

void
liphy_object_jump (liphyObject*       self,
                   const limatVector* impulse);

liphyObject*
liphy_object_scan_sphere (liphyObject*       self,
                          const limatVector* relctr,
                          float              radius);

float
liphy_object_sweep (const liphyObject* self,
                    const limatVector* sweep);

int
liphy_object_sweep_sphere (liphyObject*       self,
                           const limatVector* relsrc,
                           const limatVector* reldst,
                           float              radius,
                           liphyCollision*    result);

void
liphy_object_get_angular (const liphyObject* self,
                          limatVector*       value);

void
liphy_object_set_angular (liphyObject*       self,
                          const limatVector* value);

int
liphy_object_get_collision_group (const liphyObject* self);

void
liphy_object_set_collision_group (liphyObject* self,
                                  int          mask);

int
liphy_object_get_collision_mask (const liphyObject* self);

void
liphy_object_set_collision_mask (liphyObject* self,
                                 int          mask);

void
liphy_object_set_contact_call (liphyObject*     self,
                               liphyContactCall value);

liphyControlMode
liphy_object_get_control_mode (const liphyObject* self);

void
liphy_object_set_control_mode (liphyObject*     self,
                               liphyControlMode value);

liphyPhysics*
liphy_object_get_engine (liphyObject* self);

void
liphy_object_get_gravity (const liphyObject* self,
                          limatVector*       value);

void
liphy_object_set_gravity (const liphyObject* self,
                          const limatVector* value);

int
liphy_object_get_ground (const liphyObject* self);

void
liphy_object_get_inertia (liphyObject* self,
                          limatVector* result);

float
liphy_object_get_mass (const liphyObject* self);

void
liphy_object_set_mass (liphyObject* self,
                       float        value);

float
liphy_object_get_movement (const liphyObject* self);

void
liphy_object_set_movement (liphyObject* self,
                           float        value);

int
liphy_object_get_realized (const liphyObject* self);

int
liphy_object_set_realized (liphyObject* self,
                           int          value);

void
liphy_object_set_rotating (liphyObject* self,
                           float        value);

float
liphy_object_get_speed (const liphyObject* self);

void
liphy_object_set_speed (liphyObject* self,
                        float        value);

void
liphy_object_set_shape (liphyObject*   self,
                        liphyShape*    shape,
                        liphyShapeMode mode);

liphyShapeMode
liphy_object_get_shape_mode (const liphyObject* self);

void
liphy_object_set_shape_mode (liphyObject*   self,
                             liphyShapeMode value);

void
liphy_object_get_transform (const liphyObject* self,
                            limatTransform*    value);

void
liphy_object_set_transform (liphyObject*          self,
                            const limatTransform* value);

liphyCallback
liphy_object_get_usercall (liphyObject* self);

void
liphy_object_set_usercall (liphyObject*  self,
                           liphyCallback value);

void*
liphy_object_get_userdata (liphyObject* self);

void
liphy_object_set_userdata (liphyObject* self,
                           void*        value);

void
liphy_object_get_velocity (liphyObject* self,
                           limatVector* value);

void
liphy_object_set_velocity (liphyObject*       self,
                           const limatVector* value);

LI_END_DECLS

#endif

/** @} */
/** @} */
