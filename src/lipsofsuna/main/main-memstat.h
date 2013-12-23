/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __MAIN_MEMSTAT_H__
#define __MAIN_MEMSTAT_H__

#include "lipsofsuna/system.h"

typedef struct _LIMaiMemstatObject LIMaiMemstatObject;
struct _LIMaiMemstatObject
{
	LIMaiMemstatObject* next;
	char ext[128];
	char cls[128];
	int bytes;
};

typedef struct _LIMaiMemstat LIMaiMemstat;
struct _LIMaiMemstat
{
	LIMaiMemstatObject* first;
	LIMaiMemstatObject* last;
};

LIAPICALL (LIMaiMemstat*, limai_memstat_new, ());

LIAPICALL (void, limai_memstat_free, (
	LIMaiMemstat* self));

LIAPICALL (int, limai_memstat_add_object, (
	LIMaiMemstat* self,
	const char*   ext,
	const char*   cls,
	int           bytes));

#endif
