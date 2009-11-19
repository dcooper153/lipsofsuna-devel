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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSpeech Speech
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Speech"
 * ---
 * -- Display speech above objects.
 * -- @name Sound
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Displays a message above the object.
 * --
 * -- @param self Speech class.
 * -- @param object Object or object id.
 * -- @param message Message.
 * function Speech.add(self, object, message)
 */
static int
Speech_add (lua_State* lua)
{
	uint32_t id;
	const char* msg;
	liscrData* data;
	liengObject* object;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SPEECH);
	data = liscr_isdata (lua, 2, LICOM_SCRIPT_OBJECT);
	if (data != NULL)
	{
		object = data->data;
		id = object->id;
	}
	else
		id = luaL_checknumber (lua, 2);
	msg = luaL_checkstring (lua, 3);

	liext_module_set_speech (module, id, msg);
	return 0;
}

/*****************************************************************************/

void
liextSpeechScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SPEECH, data);
	liscr_class_insert_func (self, "add", Speech_add);
}

/** @} */
/** @} */
/** @} */
