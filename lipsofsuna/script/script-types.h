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

#ifndef __SCRIPT_TYPES_H__
#define __SCRIPT_TYPES_H__

#include <lua.h>
#include <lauxlib.h>
#include <algorithm/lips-algorithm.h>

#define LISCR_TYPE_BOOLEAN ((void*) -1)
#define LISCR_TYPE_FLOAT ((void*) -2)
#define LISCR_TYPE_INT ((void*) -3)
#define LISCR_TYPE_STRING ((void*) -4)

typedef struct _liscrArgs liscrArgs;
typedef struct _liscrClass liscrClass;
typedef struct _liscrData liscrData;
typedef struct _liscrScript liscrScript;
typedef void (*liscrClassInit)(liscrClass*, void*);
typedef void (*liscrGCFunc)();
typedef int (*liscrMarshal)(lua_State*);
typedef void (*liscrArgsFunc)(liscrArgs*);

/* FIXME */
struct _liscrData
{
	liscrClass* clss;
	liscrScript* script;
	liscrGCFunc free;
	void* data;
	int refcount;
};

/* FIXME */
struct _liscrScript
{
	void* userpointer;
	lua_State* lua;
	lialgStrdic* classes;
};

/* FIXME */
#include "script-private.h"

#endif
