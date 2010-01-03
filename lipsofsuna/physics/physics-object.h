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
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyObject Object
 * @{
 */

#ifndef __PHYSICS_OBJECT_H__
#define __PHYSICS_OBJECT_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "physics.h"
#include "physics-shape.h"
#include "physics-types.h"

#ifdef __cplusplus
extern "C" {
#endif

LIPhyObject*
liphy_object_new (LIPhyPhysics*    physics,
                  uint32_t         id,
                  LIPhyShape*      shape,
                  LIPhyControlMode control_mode);

void
liphy_object_free (LIPhyObject* self);

void
liphy_object_clear_shape (LIPhyObject* self);

void
liphy_object_impulse (LIPhyObject*       self,
                      const LIMatVector* point,
                      const LIMatVector* impulse);

int
liphy_object_insert_shape (LIPhyObject*          self,
                           LIPhyShape*           shape,
                           const LIMatTransform* transform);

void
liphy_object_jump (LIPhyObject*       self,
                   const LIMatVector* impulse);

LIPhyObject*
liphy_object_scan_sphere (LIPhyObject*       self,
                          const LIMatVector* relctr,
                          float              radius);

float
liphy_object_sweep (const LIPhyObject* self,
                    const LIMatVector* sweep);

int
liphy_object_sweep_sphere (LIPhyObject*       self,
                           const LIMatVector* relsrc,
                           const LIMatVector* reldst,
                           float              radius,
                           LIPhyCollision*    result);

void
liphy_object_get_angular (const LIPhyObject* self,
                          LIMatVector*       value);

void
liphy_object_set_angular (LIPhyObject*       self,
                          const LIMatVector* value);

int
liphy_object_get_collision_group (const LIPhyObject* self);

void
liphy_object_set_collision_group (LIPhyObject* self,
                                  int          mask);

int
liphy_object_get_collision_mask (const LIPhyObject* self);

void
liphy_object_set_collision_mask (LIPhyObject* self,
                                 int          mask);

void
liphy_object_set_contact_call (LIPhyObject*     self,
                               LIPhyContactCall value);

LIPhyControlMode
liphy_object_get_control_mode (const LIPhyObject* self);

void
liphy_object_set_control_mode (LIPhyObject*     self,
                               LIPhyControlMode value);

LIPhyPhysics*
liphy_object_get_engine (LIPhyObject* self);

void
liphy_object_get_gravity (const LIPhyObject* self,
                          LIMatVector*       value);

void
liphy_object_set_gravity (const LIPhyObject* self,
                          const LIMatVector* value);

int
liphy_object_get_ground (const LIPhyObject* self);

void
liphy_object_get_inertia (LIPhyObject* self,
                          LIMatVector* result);

float
liphy_object_get_mass (const LIPhyObject* self);

void
liphy_object_set_mass (LIPhyObject* self,
                       float        value);

float
liphy_object_get_movement (const LIPhyObject* self);

void
liphy_object_set_movement (LIPhyObject* self,
                           float        value);

int
liphy_object_get_realized (const LIPhyObject* self);

int
liphy_object_set_realized (LIPhyObject* self,
                           int          value);

void
liphy_object_set_rotating (LIPhyObject* self,
                           float        value);

float
liphy_object_get_strafing (const LIPhyObject* self);

void
liphy_object_set_strafing (LIPhyObject* self,
                           float        value);

float
liphy_object_get_speed (const LIPhyObject* self);

void
liphy_object_set_speed (LIPhyObject* self,
                        float        value);

void
liphy_object_set_shape (LIPhyObject* self,
                        LIPhyShape*  shape);

void
liphy_object_get_transform (const LIPhyObject* self,
                            LIMatTransform*    value);

void
liphy_object_set_transform (LIPhyObject*          self,
                            const LIMatTransform* value);

liphyCallback
liphy_object_get_usercall (LIPhyObject* self);

void
liphy_object_set_usercall (LIPhyObject*  self,
                           liphyCallback value);

void*
liphy_object_get_userdata (LIPhyObject* self);

void
liphy_object_set_userdata (LIPhyObject* self,
                           void*        value);

void
liphy_object_get_velocity (LIPhyObject* self,
                           LIMatVector* value);

void
liphy_object_set_velocity (LIPhyObject*       self,
                           const LIMatVector* value);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
