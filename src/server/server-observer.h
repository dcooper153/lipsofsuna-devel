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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvObserver Observer
 * @{
 */

#ifndef __SERVER_OBSERVER_H__
#define __SERVER_OBSERVER_H__

#include <algorithm/lips-algorithm.h>
#include <engine/lips-engine.h>
#include "server.h"

typedef struct _lisrvObserverIter lisrvObserverIter;
struct _lisrvObserverIter
{
	liengObjectIter objects;
	liengObject* subject;
	liengObject* object;
};

/**
 * \brief Iterates through all objects near the subject object that can see it.
 *
 * This is a macro that works in the same way with a for loop.
 *
 * \param iter Observer iterator.
 * \param subject Pointer to an object.
 * \param radius Search radius in sectors.
 */
#define LISRV_FOREACH_OBSERVER(iter, subject, radius) \
	for (lisrv_observer_iter_first (&iter, subject, radius) ; iter.object != NULL ; \
	     lisrv_observer_iter_next (&iter))

static inline int
lisrv_observer_iter_first (lisrvObserverIter* self,
                           liengObject*       subject,
                           int                radius)
{
	int x;
	int y;
	int z;

	/* Initialize self. */
	memset (self, 0, sizeof (lisrvObserverIter));
	if (subject->sector == NULL)
		return 0;
	x = subject->sector->x;
	y = subject->sector->y;
	z = subject->sector->z;
	self->subject = subject;
	if (!lieng_object_iter_first (&self->objects, subject->engine, x, y, z, radius))
		return 0;

	/* Find first observer. */
	while (self->objects.object != NULL)
	{
		if (lisrv_object_sees (self->objects.object, self->subject))
		{
			self->object = self->objects.object;
			return 1;
		}
		if (!lieng_object_iter_next (&self->objects))
			return 0;
	}

	return 0;
}

static inline int
lisrv_observer_iter_next (lisrvObserverIter* self)
{
	/* Find next observer. */
	while (self->objects.object != NULL)
	{
		if (!lieng_object_iter_next (&self->objects))
			return 0;
		if (lisrv_object_sees (self->objects.object, self->subject))
		{
			self->object = self->objects.object;
			return 1;
		}
	}
	self->object = NULL;

	return 0;
}

#endif

/** @} */
/** @} */
