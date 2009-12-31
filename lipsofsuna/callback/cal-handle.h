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
 * \addtogroup licalHandle Handle
 * @{
 */

#ifndef __CAL_HANDLE_H__
#define __CAL_HANDLE_H__

#include "cal-marshal.h"

typedef struct _licalCallfunc licalCallfunc;
typedef struct _licalCallbacks licalCallbacks;
typedef struct _licalHandle licalHandle;

struct _licalHandle
{
	licalCallbacks* calls;
	void* object;
	char type[32];
	licalCallfunc* func;
};

#ifdef __cplusplus
extern "C" {
#endif

void
lical_handle_release (licalHandle* self);

void
lical_handle_releasev (licalHandle* self,
                       int          count);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
