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
 * \addtogroup liser Server
 * @{
 * \addtogroup LISerObject Object
 * @{
 */

#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/string.h>
#include "server-types.h"

int
liser_object_moved (LIEngObject* self);

int
liser_object_purge (LIEngObject* self,
                    LISerServer* server);

int
liser_object_serialize (LIEngObject* self,
                        LISerServer* server,
                        int          save);

#endif

/** @} */
/** @} */
