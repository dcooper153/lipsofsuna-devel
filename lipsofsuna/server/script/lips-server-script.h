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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvscr Script
 * @{
 */

#ifndef __LIPS_SERVER_SCRIPT_H__
#define __LIPS_SERVER_SCRIPT_H__

#include <script/lips-script.h>

#define LISRV_SCRIPT_EXTENSION "Lips.Extension"
#define LISRV_SCRIPT_OBJECT "Lips.Object"
#define LISRV_SCRIPT_SERVER "Lips.Server"

void
lisrvEffectScript (liscrClass* self,
                   void*       data);

void
lisrvExtensionScript (liscrClass* self,
                      void*       data);

void
lisrvObjectScript (liscrClass* self,
                   void*       data);

void
lisrvServerScript (liscrClass* self,
                   void*       data);

#endif

/** @} */
/** @} */
