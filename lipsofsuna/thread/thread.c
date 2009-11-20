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
 * \addtogroup lithrThread Thread
 * @{
 */

#include <pthread.h>
#include <system/lips-system.h>
#include "thread.h"

struct _lithrThread
{
	pthread_t thread;
	int done;
	void* data;
	lithrThreadFunc func;
};

static void*
private_thread (void* data);

/*****************************************************************************/

lithrThread*
lithr_thread_new (lithrThreadFunc func,
                  void*           data)
{
	lithrThread* self;

	self = lisys_calloc (1, sizeof (lithrThread));
	if (self == NULL)
		return NULL;
	self->data = data;
	self->func = func;
	if (pthread_create (&self->thread, NULL, private_thread, self) != 0)
	{
		lisys_error_set (ENOMEM, "not enough resources to create thread");
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
lithr_thread_free (lithrThread* self)
{
	lithr_thread_join (self);
	lisys_free (self);
}

void
lithr_thread_join (lithrThread* self)
{
	pthread_join (self->thread, NULL);
}

int
lithr_thread_call_get_done (lithrThread* self)
{
	return self->done;
}

/*****************************************************************************/

static void*
private_thread (void* data)
{
	lithrThread* self;

	self = data;
	self->func (self, self->data);
	self->done = 1;
	return NULL;
}

/** @} */
/** @} */
