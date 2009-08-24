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
 * \addtogroup lical Callback
 * @{
 * \addtogroup licalCallbacks Callbacks
 * @{
 */

#ifndef __CAL_CALLBACKS_H__
#define __CAL_CALLBACKS_H__

#include <algorithm/lips-algorithm.h>
#include "cal-marshal.h"

typedef int licalType;

typedef struct _licalCallfunc licalCallfunc;
struct _licalCallfunc
{
	int prio;
	int removed;
	void* call;
	void* data;
	licalCallfunc* prev;
	licalCallfunc* next;
};

typedef struct _licalHandle licalHandle;
struct _licalHandle
{
	licalType type;
	licalCallfunc* func;
};

typedef struct _licalCallbacks licalCallbacks;
struct _licalCallbacks
{
	lialgU32dic* types;
	licalCallfunc* removed;
};

licalCallbacks*
lical_callbacks_new ();

void
lical_callbacks_free (licalCallbacks* self);

int
lical_callbacks_call (licalCallbacks* self,
                      licalType       type,
                                      ...);

int
lical_callbacks_callva (licalCallbacks* self,
                        licalType       type,
                        va_list         args);

int
lical_callbacks_insert_type (licalCallbacks* self,
                             licalType       type,
                             licalMarshal    marshal);

void
lical_callbacks_update (licalCallbacks* self);

void
lical_callbacks_remove_type (licalCallbacks* self,
                             licalType       type);

int
lical_callbacks_insert_callback (licalCallbacks* self,
                                 licalType       type,
                                 int             priority,
                                 void*           call,
                                 void*           data,
                                 licalHandle*    result);

void
lical_callbacks_remove_callback (licalCallbacks* self,
                                 licalHandle*    handle);

#endif

/** @} */
/** @} */
