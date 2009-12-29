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
 * \addtogroup liscr Script
 * @{
 * \addtogroup liscrClass Class
 * @{
 */

#ifndef __SCRIPT_CLASS_H__
#define __SCRIPT_CLASS_H__

#include "script-types.h"

liscrClass*
liscr_class_new (liscrScript* script,
                 const char*  name);

liscrClass*
liscr_class_new_full (liscrScript* script,
                      liscrClass*  base,
                      const char*  name,
                      int          global);

void
liscr_class_free (liscrClass* self);

void
liscr_class_inherit (liscrClass*    self,
                     liscrClassInit init,
                     void*          data);

int
liscr_class_insert_cfunc (liscrClass*   self,
                          const char*   name,
                          liscrArgsFunc func);

int
liscr_class_insert_cvar (liscrClass*   self,
                         const char*   name,
                         liscrArgsFunc getter,
                         liscrArgsFunc setter);

void
liscr_class_insert_enum (liscrClass* self,
                         const char* name,
                         int         value);

void
liscr_class_insert_func (liscrClass*  self,
                         const char*  name,
                         liscrMarshal value);

int
liscr_class_insert_interface (liscrClass* self,
                              const char* name);

int
liscr_class_insert_mfunc (liscrClass*   self,
                          const char*   name,
                          liscrArgsFunc func);

int
liscr_class_insert_mvar (liscrClass*   self,
                         const char*   name,
                         liscrArgsFunc getter,
                         liscrArgsFunc setter);

int
liscr_class_get_interface (const liscrClass* self,
                           const char*       name);

const char*
liscr_class_get_name (const liscrClass* self);

void*
liscr_class_get_userdata (liscrClass* self,
                          const char* key);

void
liscr_class_set_userdata (liscrClass* self,
                          const char* key,
                          void*       value);

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

/** @} */
/** @} */
