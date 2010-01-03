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
 * \addtogroup LIPhyPhysics Physics
 * @{
 */

#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <lipsofsuna/callback.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "physics-types.h"

#ifdef __cplusplus
extern "C" {
#endif

LIPhyPhysics*
liphy_physics_new (LICalCallbacks* callbacks);

void
liphy_physics_free (LIPhyPhysics* self);

int
liphy_physics_cast_ray (const LIPhyPhysics* self,
                        const LIMatVector*  start,
                        const LIMatVector*  end,
                        LIMatVector*        result,
                        LIMatVector*        normal);

int
liphy_physics_cast_shape (const LIPhyPhysics*   self,
                          const LIMatTransform* start,
                          const LIMatTransform* end,
                          const LIPhyShape*     shape,
                          int                   group,
                          int                   mask,
                          LIPhyObject**         ignore_array,
                          int                   ignore_count,
                          LIPhyCollision*       result);

int
liphy_physics_cast_sphere (const LIPhyPhysics* self,
                           const LIMatVector*  start,
                           const LIMatVector*  end,
                           float               radius,
                           LIPhyObject**       ignore_array,
                           int                 ignore_count,
                           LIPhyCollision*     result);

void
liphy_physics_clear_constraints (LIPhyPhysics* self,
                                 LIPhyObject*  object);

void
liphy_physics_clear_contacts (LIPhyPhysics* self,
                              LIPhyObject*  object);

void
liphy_physics_update (LIPhyPhysics* self,
                      float         secs);

void*
liphy_physics_get_userdata (LIPhyPhysics* self);

void
liphy_physics_set_userdata (LIPhyPhysics* self,
                            void*         data);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
