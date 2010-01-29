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

#ifndef __SCRIPT_LIBRARY_H__
#define __SCRIPT_LIBRARY_H__

#include <lipsofsuna/math.h>
#include <lipsofsuna/script.h>

#define LISCR_SCRIPT_CLASS "Class"
#define LISCR_SCRIPT_DATA "Data"
#define LISCR_SCRIPT_EVENT "Event"
#define LISCR_SCRIPT_OBJECT "Object"
#define LISCR_SCRIPT_PACKET "Packet"
#define LISCR_SCRIPT_PATH "Path"
#define LISCR_SCRIPT_QUATERNION "Quaternion"
#define LISCR_SCRIPT_VECTOR "Vector"

void
liscr_script_class (LIScrClass* clss,
                    void*       data);

void
liscr_script_data (LIScrClass* clss,
                   void*       data);

void
liscr_script_event (LIScrClass* clss,
                    void*       data);

void
liscr_script_object (LIScrClass* self,
                     void*       data);

void
liscr_script_packet (LIScrClass* self,
                     void*       data);

void
liscr_script_path (LIScrClass* self,
                   void*       data);

void
liscr_script_quaternion (LIScrClass* self,
                         void*       data);

void
liscr_script_vector (LIScrClass* self,
                     void*       data);

/*****************************************************************************/

LIScrData*
liscr_quaternion_new (LIScrScript*           script,
                      const LIMatQuaternion* quat);

LIScrData*
liscr_vector_new (LIScrScript*       script,
                  const LIMatVector* vector);

/*****************************************************************************/

#include "script-event.h"
#include "script-packet.h"

#endif

/** @} */

