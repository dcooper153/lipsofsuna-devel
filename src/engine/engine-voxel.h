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
 * \addtogroup liengVoxel Voxel
 * @{
 */

#ifndef __ENGINE_VOXEL_H__
#define __ENGINE_VOXEL_H__

#include <math/lips-math.h>
#include "engine-types.h"

liengTile
lieng_voxel_init (int shape,
                  int type);

int
lieng_voxel_triangulate (liengTile    self,
                         limatVector* result);

liengTile
lieng_voxel_validate (liengTile self);

int
lieng_voxel_get_shape (liengTile self);

int
lieng_voxel_get_type (liengTile self);

#endif

/** @} */
/** @} */
