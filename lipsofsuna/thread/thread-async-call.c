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

#include <pthread.h>
#include <lipsofsuna/system.h>
#include "thread-async-call.h"

struct _LIThrAsyncCall
{
	pthread_t thread;
	int done;
	int result;
	int stop;
	float progress;
	void* data;
	lithrAsyncFunc func;
	lithrAsyncFunc free;
};

static void*
private_thread (void* data);

/*****************************************************************************/

LIThrAsyncCall*
lithr_async_call_new (lithrAsyncFunc func,
                      lithrAsyncFunc freecb,
                      void*          data)
{
	LIThrAsyncCall* self;

	self = lisys_calloc (1, sizeof (LIThrAsyncCall));
	if (self == NULL)
		return NULL;
	self->data = data;
	self->func = func;
	self->free = freecb;
	if (pthread_create (&self->thread, NULL, private_thread, self) != 0)
	{
		lisys_error_set (ENOMEM, "not enough resources to create thread");
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
lithr_async_call_free (LIThrAsyncCall* self)
{
	lithr_async_call_join (self);
	if (self->free != NULL)
		self->free (self, self->data);
	lisys_free (self);
}

int
lithr_async_call_join (LIThrAsyncCall* self)
{
	pthread_join (self->thread, NULL);
	return self->result;
}

void
lithr_async_call_stop (LIThrAsyncCall* self)
{
	self->stop = 1;
}

int
lithr_async_call_get_done (LIThrAsyncCall* self)
{
	return self->done;
}

float
lithr_async_call_get_progress (LIThrAsyncCall* self)
{
	return self->progress;
}

void
lithr_async_call_set_progress (LIThrAsyncCall* self,
                               float           value)
{
	self->progress = value;
}

int
lithr_async_call_get_result (LIThrAsyncCall* self)
{
	return self->result;
}

void
lithr_async_call_set_result (LIThrAsyncCall* self,
                             int             value)
{
	self->result = value;
}

int
lithr_async_call_get_stop (LIThrAsyncCall* self)
{
	return self->stop;
}

void*
lithr_async_call_get_userdata (LIThrAsyncCall* self)
{
	return self->data;
}

/*****************************************************************************/

static void*
private_thread (void* data)
{
	LIThrAsyncCall* self;

	self = data;
	self->func (self, self->data);
	self->done = 1;
	return NULL;
}

/** @} */
/** @} */
