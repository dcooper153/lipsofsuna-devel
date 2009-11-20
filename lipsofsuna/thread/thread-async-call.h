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
 * \addtogroup lithr Thread
 * @{
 * \addtogroup lithrAsyncCall Async Call
 * @{
 */

#ifndef __THREAD_ASYNC_CALL_H__
#define __THREAD_ASYNC_CALL_H__

typedef struct _lithrAsyncCall lithrAsyncCall;
typedef void (*lithrAsyncFunc)(lithrAsyncCall*, void*);

lithrAsyncCall*
lithr_async_call_new (lithrAsyncFunc func,
                      lithrAsyncFunc freecb,
                      void*          data);

void
lithr_async_call_free (lithrAsyncCall* self);

int
lithr_async_call_join (lithrAsyncCall* self);

void
lithr_async_call_stop (lithrAsyncCall* self);

int
lithr_async_call_get_done (lithrAsyncCall* self);

float
lithr_async_call_get_progress (lithrAsyncCall* self);

void
lithr_async_call_set_progress (lithrAsyncCall* self,
                               float           value);

int
lithr_async_call_get_result (lithrAsyncCall* self);

void
lithr_async_call_set_result (lithrAsyncCall* self,
                             int             value);

int
lithr_async_call_get_stop (lithrAsyncCall* self);

void*
lithr_async_call_get_userdata (lithrAsyncCall* self);

#endif

/** @} */
/** @} */
