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

#ifndef __SCRIPT_LIBRARY_H__
#define __SCRIPT_LIBRARY_H__

#include <math/lips-math.h>
#include <script/lips-script.h>

#define LICOM_SCRIPT_CLASS "Lips.Class"
#define LICOM_SCRIPT_EVENT "Lips.Event"
#define LICOM_SCRIPT_OBJECT "Lips.Object"
#define LICOM_SCRIPT_PACKET "Lips.Packet"
#define LICOM_SCRIPT_PATH "Lips.Path"
#define LICOM_SCRIPT_QUATERNION "Lips.Quaternion"
#define LICOM_SCRIPT_VECTOR "Lips.Vector"

void
licomClassScript (liscrClass* clss,
                  void*       data);

void
licomEventScript (liscrClass* clss,
                  void*       data);

void
licomObjectScript (liscrClass* self,
                   void*       data);

void
licomPacketScript (liscrClass* self,
                   void*       data);

void
licomPathScript (liscrClass* self,
                 void*       data);

void
licomQuaternionScript (liscrClass* self,
                       void*       data);

void
licomVectorScript (liscrClass* self,
                   void*       data);

/*****************************************************************************/

liscrData*
liscr_quaternion_new (liscrScript*           script,
                      const limatQuaternion* quat);

liscrData*
liscr_vector_new (liscrScript*       script,
                  const limatVector* vector);

/*****************************************************************************/

#include "script-event.h"
#include "script-packet.h"

#endif

/** @} */

