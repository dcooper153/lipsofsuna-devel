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
 * \addtogroup licliscrObject Object
 * @{
 */

#include <client/lips-client.h>
#include <script/common/lips-common-script.h>
#include "lips-client-script.h"

/*****************************************************************************/

/* @luadoc
 * module "Core.Client.Object"
 * ---
 * -- Create and manipulate client side objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * ---
 * -- FIXME
 * --
 * -- @param self Object.
 * function Object.emit_particles(self)
 */
static int
Object_emit_particles (lua_State* lua)
{
	liscrData* object;
	liengObject* data;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	data = LIENG_OBJECT (object->data);

	if (data->render != NULL)
		lirnd_object_emit_particles (data->render);
	return 1;
}

/*****************************************************************************/

void
licliObjectScript (liscrClass* self,
                   void*       data)
{
	liscr_class_inherit (self, licomObjectScript);
	liscr_class_insert_func (self, "emit_particles", Object_emit_particles);
}

/** @} */
/** @} */
/** @} */
