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
 * \addtogroup liscrData Data
 * @{
 */

#ifndef __SCRIPT_DATA_H__
#define __SCRIPT_DATA_H__

#include <archive/lips-archive.h>
#include "script-types.h"

liscrData*
liscr_data_new (liscrScript* script,
                void*        data,
                const char*  meta);

liscrData*
liscr_data_new_from_stream (liscrScript*    script,
                            liarcSerialize* serialize);

void
liscr_data_free (liscrData* object);

int
liscr_data_read (liscrData*      self,
                 liarcSerialize* serialize);

void
liscr_data_ref (liscrData* object,
                liscrData* referencer);

void
liscr_data_unref (liscrData* object,
                  liscrData* referencer);

int
liscr_data_write (liscrData*      self,
                  liarcSerialize* serialize);

liscrClass*
liscr_data_get_class (liscrData* self);

liscrScript*
liscr_data_get_script (liscrData* self);

/*****************************************************************************/
/* Lua specific. */

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

liscrData*
liscr_isiface (lua_State*  lua,
               int         arg,
               const char* meta);

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
liscr_checkiface (lua_State*  lua,
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

int
liscr_getref (lua_State* lua,
              liscrData* owner,
              int        slot);

void
liscr_setref (lua_State* lua,
              liscrData* owner,
              int        slot);

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
