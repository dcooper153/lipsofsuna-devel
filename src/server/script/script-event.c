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
 * \addtogroup lisrvscrEvent Event
 * @{
 */

#include <server/lips-server.h>
#include "lips-server-script.h"

void
lisrvEventScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licomEventScript);
	liscr_class_insert_enum (self, "ACTION", LISRV_EVENT_TYPE_ACTION);
	liscr_class_insert_enum (self, "CONTROL", LISRV_EVENT_TYPE_CONTROL);
	liscr_class_insert_enum (self, "HEAR", LISRV_EVENT_TYPE_HEAR);
	liscr_class_insert_enum (self, "LOGIN", LISRV_EVENT_TYPE_LOGIN);
	liscr_class_insert_enum (self, "LOGOUT", LISRV_EVENT_TYPE_LOGOUT);
	liscr_class_insert_enum (self, "MESSAGE", LISRV_EVENT_TYPE_MESSAGE);
	liscr_class_insert_enum (self, "PACKET", LISRV_EVENT_TYPE_PACKET);
	liscr_class_insert_enum (self, "SIMULATE", LISRV_EVENT_TYPE_SIMULATE);
	liscr_class_insert_enum (self, "SPEECH", LISRV_EVENT_TYPE_SPEECH);
	liscr_class_insert_enum (self, "VISIBILITY", LISRV_EVENT_TYPE_VISIBILITY);
}

/** @} */
/** @} */
/** @} */
