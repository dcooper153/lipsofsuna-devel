/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenQueue Queue
 * @{
 */

#include "render-internal.h"

LIRenQueue* liren_queue_new ()
{
	LIRenQueue* self;

	self = lisys_calloc (1, sizeof (LIRenQueue));
	if (self == NULL)
		return NULL;

	self->mutex = lisys_mutex_new ();
	if (self->mutex == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void liren_queue_free (
	LIRenQueue* self)
{
	lisys_assert (self->message_first == NULL);
	lisys_assert (self->message_last == NULL);
	lisys_mutex_free (self->mutex);
	lisys_free (self);
}

LIRenMessage* liren_queue_pop_message (
	LIRenQueue* self)
{
	LIRenMessage* message;

	lisys_mutex_lock (self->mutex);
	if (self->message_first != NULL)
	{
		message = self->message_first;
		self->message_first = message->next;
		if (message == self->message_last)
			self->message_last = NULL;
	}
	else
		message = NULL;
	lisys_mutex_unlock (self->mutex);

	return message;
}

/**
 * \brief Pushes a message to the queue.
 *
 * If the message needs the handler to return the value, this function pauses
 * the caller thread until the handler has returned the value.
 *
 * \param self Queue.
 * \param message Message.
 */
void liren_queue_push_message (
	LIRenQueue*   self,
	LIRenMessage* message)
{
	/* Push the message to the end of the queue. */
	lisys_mutex_lock (self->mutex);
	if (self->message_last != NULL)
		self->message_last->next = message;
	else
		self->message_first = message;
	self->message_last = message;
	lisys_mutex_unlock (self->mutex);

	/* Wait for the return value if needed. */
	/* The mutex is freed here and not in the handler because it obviously
	   needs to stay around until we have finished waiting. The handler might
	   free it before we have waken up so this is necessary. */
	if (message->mutex_caller != NULL)
	{
		lisys_mutex_lock (message->mutex_caller);
		lisys_mutex_free (message->mutex_caller);
	}
}

/** @} */
/** @} */
/** @} */
