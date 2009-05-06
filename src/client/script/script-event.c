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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 * \addtogroup licliscrEvent Event
 * @{
 */

#include <client/lips-client.h>
#include "lips-client-script.h"

void
licliEventScript (liscrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, licomEventScript);
	liscr_class_insert_enum (self, "ACTION", LICLI_EVENT_TYPE_ACTION);
	liscr_class_insert_enum (self, "FRAME", LICLI_EVENT_TYPE_FRAME);
	liscr_class_insert_enum (self, "PACKET", LICLI_EVENT_TYPE_PACKET);
	liscr_class_insert_enum (self, "SELECT", LICLI_EVENT_TYPE_SELECT);
}

/** @} */
/** @} */
/** @} */
