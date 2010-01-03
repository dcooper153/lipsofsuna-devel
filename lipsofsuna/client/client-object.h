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
 * \addtogroup licli Client
 * @{
 * \addtogroup LICliObject Object
 * @{
 */

#ifndef __CLIENT_OBJECT_H__
#define __CLIENT_OBJECT_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/render.h>
#include <lipsofsuna/system.h>
#include "client-types.h"

#define LICLI_IS_CLIENT_OBJECT(o) (lieng_object_get_userdata (o) != NULL)

#define LICLI_OBJECT_POSITION_SMOOTHING 0.5f
#define LICLI_OBJECT_ROTATION_SMOOTHING 0.5f

void
licli_object_set_animation (LIEngObject* self,
                            int          value,
                            int          channel,
                            int          permanent,
                            float        priority);

void
licli_object_set_flags (LIEngObject* self,
                        int          value);

#endif

/** @} */
/** @} */
