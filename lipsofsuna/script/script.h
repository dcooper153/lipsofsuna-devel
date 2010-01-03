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
 * \addtogroup liscr Script
 * @{
 * \addtogroup LIScrScript Script
 * @{
 */

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <lipsofsuna/system.h>
#include "script-types.h"

LIScrScript*
liscr_script_new ();

int
liscr_script_load (LIScrScript* self,
                   const char*  path);

void
liscr_script_free (LIScrScript* self);

LIScrClass*
liscr_script_create_class (LIScrScript*   self,
                           const char*    name,
                           LIScrClassInit init,
                           void*          data);

LIScrClass*
liscr_script_find_class (LIScrScript* self,
                         const char*  name);

int
liscr_script_insert_class (LIScrScript* self,
                           LIScrClass*  clss);

void
liscr_script_update (LIScrScript* self,
                     float        secs);

void*
liscr_script_get_userdata (LIScrScript* self);

void
liscr_script_set_userdata (LIScrScript* self,
                           void*        data);

#endif

/** @} */
/** @} */

