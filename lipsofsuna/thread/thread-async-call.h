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
 * \addtogroup lithr Thread
 * @{
 * \addtogroup LIThrAsyncCall Async Call
 * @{
 */

#ifndef __THREAD_ASYNC_CALL_H__
#define __THREAD_ASYNC_CALL_H__

typedef struct _LIThrAsyncCall LIThrAsyncCall;
typedef void (*lithrAsyncFunc)(LIThrAsyncCall*, void*);

LIThrAsyncCall*
lithr_async_call_new (lithrAsyncFunc func,
                      lithrAsyncFunc freecb,
                      void*          data);

void
lithr_async_call_free (LIThrAsyncCall* self);

int
lithr_async_call_join (LIThrAsyncCall* self);

void
lithr_async_call_stop (LIThrAsyncCall* self);

int
lithr_async_call_get_done (LIThrAsyncCall* self);

float
lithr_async_call_get_progress (LIThrAsyncCall* self);

void
lithr_async_call_set_progress (LIThrAsyncCall* self,
                               float           value);

int
lithr_async_call_get_result (LIThrAsyncCall* self);

void
lithr_async_call_set_result (LIThrAsyncCall* self,
                             int             value);

int
lithr_async_call_get_stop (LIThrAsyncCall* self);

void*
lithr_async_call_get_userdata (LIThrAsyncCall* self);

#endif

/** @} */
/** @} */
