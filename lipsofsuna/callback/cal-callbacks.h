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
 * \addtogroup lical Callback
 * @{
 * \addtogroup LICalCallbacks Callbacks
 * @{
 */

#ifndef __CAL_CALLBACKS_H__
#define __CAL_CALLBACKS_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include "cal-handle.h"
#include "cal-marshal.h"

struct _LICalCallfunc
{
	int prio;
	int removed;
	void* call;
	void* data;
	LICalCallfunc* prev;
	LICalCallfunc* next;
};

struct _LICalCallbacks
{
	LIAlgMemdic* types;
	LICalCallfunc* removed;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (LICalCallbacks*, lical_callbacks_new, ());

LIAPICALL (void, lical_callbacks_free, (
	LICalCallbacks* self));

LIAPICALL (int, lical_callbacks_call, (
	LICalCallbacks* self,
	void*           object,
	const char*     type,
	licalMarshal    marshal,
	                ...));

LIAPICALL (int, lical_callbacks_callva, (
	LICalCallbacks* self,
	void*           object,
	const char*     type,
	licalMarshal    marshal,
	va_list         args));

LIAPICALL (int, lical_callbacks_insert, (
	LICalCallbacks* self,
	void*           object,
	const char*     type,
	int             priority,
	void*           call,
	void*           data,
	LICalHandle*    result));

LIAPICALL (void, lical_callbacks_update, (
	LICalCallbacks* self));

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
