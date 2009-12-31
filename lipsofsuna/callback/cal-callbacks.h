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
#include "cal-handle.h"
#include "cal-marshal.h"

struct _licalCallfunc
{
	int prio;
	int removed;
	void* call;
	void* data;
	licalCallfunc* prev;
	licalCallfunc* next;
};

struct _licalCallbacks
{
	lialgMemdic* types;
	licalCallfunc* removed;
};

#ifdef __cplusplus
extern "C" {
#endif

licalCallbacks*
lical_callbacks_new ();

void
lical_callbacks_free (licalCallbacks* self);

int
lical_callbacks_call (licalCallbacks* self,
                      void*           object,
                      const char*     type,
                      licalMarshal    marshal,
                                      ...);

int
lical_callbacks_callva (licalCallbacks* self,
                        void*           object,
                        const char*     type,
                        licalMarshal    marshal,
                        va_list         args);

int
lical_callbacks_insert (licalCallbacks* self,
                        void*           object,
                        const char*     type,
                        int             priority,
                        void*           call,
                        void*           data,
                        licalHandle*    result);

void
lical_callbacks_update (licalCallbacks* self);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
