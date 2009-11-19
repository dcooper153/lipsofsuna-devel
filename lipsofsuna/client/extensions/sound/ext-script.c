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
 * \addtogroup liextcliSound Sound
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Sound"
 * ---
 * -- Sound playback.
 * -- @name Sound
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Music track name.
 * -- @name Sound.music
 * -- @class table
 */
static int
Sound_setter_music (lua_State* lua)
{
	const char* value;
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SOUND);
	value = luaL_checkstring (lua, 3);

#ifndef LI_DISABLE_SOUND
	liext_module_set_music (module, value);
#endif

	return 0;
}

/*****************************************************************************/

void
liextSoundScript (liscrClass* self,
                  void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SOUND, data);
	liscr_class_insert_setter (self, "music", Sound_setter_music);
}

/** @} */
/** @} */
/** @} */
