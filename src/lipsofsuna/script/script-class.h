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

#ifndef __SCRIPT_CLASS_H__
#define __SCRIPT_CLASS_H__

#include <lipsofsuna/system.h>
#include "script-types.h"

LIAPICALL (LIScrClass*, liscr_class_new, (
	LIScrScript* script,
	const char*  name));

LIAPICALL (LIScrClass*, liscr_class_new_full, (
	LIScrScript* script,
	LIScrClass*  base,
	const char*  name,
	int          global));

LIAPICALL (void, liscr_class_free, (
	LIScrClass* self));

LIAPICALL (int, liscr_class_inherit, (
	LIScrClass* self,
	const char* meta));

LIAPICALL (int, liscr_class_insert_cfunc, (
	LIScrClass*   self,
	const char*   name,
	LIScrArgsFunc func));

LIAPICALL (void, liscr_class_insert_func, (
	LIScrClass*  self,
	const char*  name,
	LIScrMarshal value));

LIAPICALL (int, liscr_class_insert_mfunc, (
	LIScrClass*   self,
	const char*   name,
	LIScrArgsFunc func));

LIAPICALL (int, liscr_class_get_interface, (
	const LIScrClass* self,
	const char*       name));

LIAPICALL (const char*, liscr_class_get_name, (
	const LIScrClass* self));

LIAPICALL (void*, liscr_class_get_userdata, (
	LIScrClass* self,
	const char* key));

LIAPICALL (void, liscr_class_set_userdata, (
	LIScrClass* self,
	const char* key,
	void*       value));

/*****************************************************************************/
/* Lua specific. */

int
liscr_class_default___call (lua_State* lua);

int
liscr_class_default___gc (lua_State* lua);

int
liscr_class_default___index (lua_State* lua);

int
liscr_class_default___newindex (lua_State* lua);

#endif
