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

LIRenMessage* liren_message_new (
	int type,
	int wait_for_result)
{
	LIRenMessage* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenMessage));
	if (self == NULL)
		return NULL;
	self->type = type;

	/* Allocate a mutex if a return value is needed. */
	if (wait_for_result)
	{
		self->mutex_caller = lisys_mutex_new ();
		if (self->mutex_caller == NULL)
		{
			lisys_free (self);
			return NULL;
		}
		lisys_mutex_lock (self->mutex_caller);
	}

	return self;
}

void liren_message_free (
	LIRenMessage* self)
{
	/* The mutex is not freed here. It's owned by the caller and freed by it
	   when it's done waiting. */
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
