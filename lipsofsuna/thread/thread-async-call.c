/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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

#include <stdio.h>
#include <stdlib.h>
#include <system/lips-system.h>
#include "thread-async-call.h"

static void*
private_thread (void* data);

/*****************************************************************************/

lithrAsyncCall*
lithr_async_call_new (lithrAsyncFunc func,
                      lithrAsyncFunc freecb,
                      void*          data)
{
	lithrAsyncCall* self;

	self = calloc (1, sizeof (lithrAsyncCall));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->data = data;
	self->func = func;
	self->free = freecb;
	if (pthread_create (&self->thread, NULL, private_thread, self) != 0)
	{
		lisys_error_set (ENOMEM, "not enough resources to create thread");
		free (self);
		return NULL;
	}

	return self;
}

void
lithr_async_call_free (lithrAsyncCall* self)
{
	lithr_async_call_wait (self);
	if (self->free != NULL)
		self->free (self);
	free (self);
}

void
lithr_async_call_stop (lithrAsyncCall* self)
{
	self->stop = 1;
}

int
lithr_async_call_wait (lithrAsyncCall* self)
{
	pthread_join (self->thread, NULL);
	return self->result;
}

int
lithr_async_call_get_done (lithrAsyncCall* self)
{
	return self->done;
}

float
lithr_async_call_get_progress (lithrAsyncCall* self)
{
	return self->progress;
}

int
lithr_async_call_get_result (lithrAsyncCall* self)
{
	return self->result;
}

/*****************************************************************************/

static void*
private_thread (void* data)
{
	lithrAsyncCall* self;

	self = data;
	self->func (self);
	self->done = 1;
	return NULL;
}

/** @} */
/** @} */
