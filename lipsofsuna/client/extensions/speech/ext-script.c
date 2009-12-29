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
 * -- Arguments:
 * -- id: Object id.
 * -- object: Object.
 * -- message: Speech string. (required)
 * --
 * -- @param self Speech class.
 * -- @param args Arguments.
 * function Speech.add(self, args)
 */
static void Speech_add (liscrArgs* args)
{
	int id;
	const char* msg;
	liscrData* object;
	liextModule* module;

	if (!liscr_args_gets_string (args, "message", &msg))
		return;
	if (liscr_args_gets_data (args, "object", LICOM_SCRIPT_OBJECT, &object))
		id = ((liengObject*) object->data)->id;
	else if (!liscr_args_gets_int (args, "id", &id))
		return;
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SPEECH);
	liext_module_set_speech (module, id, msg);
}

/*****************************************************************************/

void
liextSpeechScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SPEECH, data);
	liscr_class_insert_cfunc (self, "add", Speech_add);
}

/** @} */
/** @} */
/** @} */
