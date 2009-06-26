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
 * \addtogroup liscrUtil Util
 * @{
 */

#ifndef __SCRIPT_UTIL_H__
#define __SCRIPT_UTIL_H__

#include "script-types.h"

liscrClass*
liscr_isanyclass (lua_State*  lua,
                  int         arg);

liscrData*
liscr_isanydata (lua_State* lua,
                 int        arg);

liscrClass*
liscr_isclass (lua_State*  lua,
               int         arg,
               const char* meta);

liscrData*
liscr_isdata (lua_State*  lua,
              int         arg,
              const char* meta);

liscrClass*
liscr_checkanyclass (lua_State* lua,
                     int        arg);

liscrData*
liscr_checkanydata (lua_State* lua,
                    int        arg);

liscrClass*
liscr_checkclass (lua_State*  lua,
                  int         arg,
                  const char* meta);

void*
liscr_checkclassdata (lua_State*  lua,
                      int         arg,
                      const char* meta);

liscrData*
liscr_checkdata (lua_State*  lua,
                 int         arg,
                 const char* meta);

int
liscr_copyargs (lua_State* lua,
                liscrData* data,
                int        arg);

void
liscr_pushclass (lua_State*  lua,
                 liscrClass* clss);

void
liscr_pushdata (lua_State* lua,
                liscrData* object);

void
liscr_pushpriv (lua_State* lua,
                liscrData* object);

liscrScript*
liscr_script (lua_State* lua);

#endif

/** @} */
/** @} */
