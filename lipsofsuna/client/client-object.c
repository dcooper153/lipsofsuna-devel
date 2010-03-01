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

#include "client.h"
#include "client-object.h"
#include "client-window.h"

void
licli_object_set_flags (LIEngObject* self,
                        int          value)
{
	if (value & LINET_OBJECT_FLAG_DYNAMIC)
	{
		lieng_object_set_collision_group (self, LICLI_PHYSICS_GROUP_OBJECTS);
		lieng_object_set_collision_mask (self,
			LICLI_PHYSICS_GROUP_OBJECTS | LIPHY_GROUP_STATICS);
	}
	else
	{
		lieng_object_set_collision_group (self, LIPHY_GROUP_STATICS);
		lieng_object_set_collision_mask (self,
			LICLI_PHYSICS_GROUP_CAMERA | LICLI_PHYSICS_GROUP_OBJECTS);
	}
}

/** @} */
/** @} */
