/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __SCRIPT_TYPES_H__
#define __SCRIPT_TYPES_H__

#ifndef __cplusplus
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
#else
  #include <lua.hpp>
#endif
#include "lipsofsuna/algorithm.h"

typedef struct _LIScrArgs LIScrArgs;
typedef struct _LIScrData LIScrData;
typedef struct _LIScrScript LIScrScript;
typedef void (*LIScrGCFunc)();
typedef int (*LIScrMarshal)(lua_State*);
typedef void (*LIScrArgsFunc)(LIScrArgs*);

#endif
