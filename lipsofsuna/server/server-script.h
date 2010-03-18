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
 * \addtogroup liserscr Script
 * @{
 */

#ifndef __SERVER_SCRIPT_H__
#define __SERVER_SCRIPT_H__

#include <lipsofsuna/script.h>

#define LISER_SCRIPT_SERVER "Server"

void
liser_script_object (LIScrClass* self,
                     void*       data);

void
liser_script_server (LIScrClass* self,
                     void*       data);

#endif

/** @} */
/** @} */
