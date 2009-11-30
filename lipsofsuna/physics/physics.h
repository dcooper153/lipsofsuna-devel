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
 * \addtogroup liphyPhysics Physics
 * @{
 */

#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <math/lips-math.h>
#include <system/lips-system.h>
#include "physics-types.h"

LI_BEGIN_DECLS

liphyPhysics*
liphy_physics_new ();

void
liphy_physics_free (liphyPhysics* self);

int
liphy_physics_cast_ray (const liphyPhysics* self,
                        const limatVector*  start,
                        const limatVector*  end,
                        limatVector*        result,
                        limatVector*        normal);

int
liphy_physics_cast_sphere (const liphyPhysics* self,
                           const limatVector*  start,
                           const limatVector*  end,
                           float               radius,
                           liphyObject**       ignore_array,
                           int                 ignore_count,
                           liphyCollision*     result);

void
liphy_physics_clear_contacts (liphyPhysics* self,
                              liphyObject*  object);

void
liphy_physics_update (liphyPhysics* self,
                      float         secs);

void
liphy_physics_set_transform_callback (liphyPhysics*      self,
                                      liphyTransformCall value);

void*
liphy_physics_get_userdata (liphyPhysics* self);

void
liphy_physics_set_userdata (liphyPhysics* self,
                            void*         data);

LI_END_DECLS

#endif

/** @} */
/** @} */
