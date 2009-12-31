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

#include "cal-callbacks.h"

typedef struct _licalCalladdr licalCalladdr;
struct _licalCalladdr
{
	void* object;
	char type[32];
};

typedef struct _licalCalltype licalCalltype;
struct _licalCalltype
{
	licalCallfunc* funcs;
};

static void
private_cleanup (licalCallbacks* self);

static void
private_addr_type (licalCalladdr* self,
                   void*          object,
                   const char*    type);

static void
private_free_type (licalCalltype* self);

/*****************************************************************************/

licalCallbacks*
lical_callbacks_new ()
{
	licalCallbacks* self;

	self = lisys_calloc (1, sizeof (licalCallbacks));
	if (self == NULL)
		return NULL;
	self->types = lialg_memdic_new ();
	if (self->types == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
lical_callbacks_free (licalCallbacks* self)
{
	lialgMemdicIter iter;

	private_cleanup (self);
	LI_FOREACH_MEMDIC (iter, self->types)
		private_free_type (iter.value);
	lialg_memdic_free (self->types);
	lisys_free (self);
}

int
lical_callbacks_call (licalCallbacks* self,
                      void*           object,
                      const char*     type,
                      licalMarshal    marshal,
                                      ...)
{
	int ret;
	va_list args;

	va_start (args, marshal);
	ret = lical_callbacks_callva (self, object, type, marshal, args);
	va_end (args);

	return ret;
}

int
lical_callbacks_callva (licalCallbacks* self,
                        void*           object,
                        const char*     type,
                        licalMarshal    marshal,
                        va_list         args)
{
	int ret;
	va_list copy;
	licalCalladdr addr;
	licalCalltype* typ;
	licalCallfunc* func;
	licalCallfunc* func_next;

	private_addr_type (&addr, object, type);
	typ = lialg_memdic_find (self->types, &addr, sizeof (addr));
	if (typ == NULL)
		return 0;
	for (func = typ->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		if (func->removed)
			continue;
		va_copy (copy, args);
		ret = marshal (func->call, func->data, copy);
		va_end (copy);
		if (!ret)
			return 0;
	}

	return 1;
}

int
lical_callbacks_insert (licalCallbacks* self,
                        void*           object,
                        const char*     type,
                        int             priority,
                        void*           call,
                        void*           data,
                        licalHandle*    result)
{
	licalCalladdr addr;
	licalCalltype* typ;
	licalCallfunc* ptr;
	licalCallfunc* func;

	/* Clear handle. */
	if (result != NULL)
		memset (result, 0, sizeof (licalHandle));

	/* Find or create type. */
	private_addr_type (&addr, object, type);
	typ = lialg_memdic_find (self->types, &addr, sizeof (addr));
	if (typ == NULL)
	{
		typ = lisys_calloc (1, sizeof (licalCalltype));
		if (typ == NULL)
			return 0;
		if (!lialg_memdic_insert (self->types, &addr, sizeof (addr), typ))
		{
			lisys_free (typ);
			return 0;
		}
	}

	/* Allocate function. */
	func = lisys_calloc (1, sizeof (licalCallfunc));
	if (func == NULL)
		return 0;
	func->prio = priority;
	func->call = call;
	func->data = data;

	/* Set handle. */
	if (result != NULL)
	{
		result->calls = self;
		result->object = object;
		result->func = func;
		strncpy (result->type, type, sizeof (result->type) - 1);
	}

	/* Insertion sort by priority. */
	if (typ->funcs == NULL)
	{
		typ->funcs = func;
		return 1;
	}
	if (priority < typ->funcs->prio)
	{
		func->next = typ->funcs;
		typ->funcs->prev = func;
		typ->funcs = func;
		return 1;
	}
	for (ptr = typ->funcs ; ptr->next != NULL ; ptr = ptr->next)
	{
		if (priority < ptr->next->prio)
			break;
	}
	func->prev = ptr;
	func->next = ptr->next;
	if (ptr->next != NULL)
		ptr->next->prev = func;
	ptr->next = func;

	return 1;
}

void
lical_callbacks_update (licalCallbacks* self)
{
	private_cleanup (self);
}

/*****************************************************************************/

static void
private_cleanup (licalCallbacks* self)
{
	licalCallfunc* func;
	licalCallfunc* prev;

	for (func = self->removed ; func != NULL ; func = prev)
	{
		prev = func->prev;
		lisys_free (func);
	}
	self->removed = NULL;
}

static void
private_addr_type (licalCalladdr* self,
                   void*          object,
                   const char*    type)
{
	memset (self, 0, sizeof (licalCalltype));
	self->object = object;
	strncpy (self->type, type, sizeof (self->type) - 1);
}

static void
private_free_type (licalCalltype* self)
{
	licalCallfunc* func;
	licalCallfunc* func_next;

	for (func = self->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		lisys_free (func);
	}
	lisys_free (self);
}

/** @} */
/** @} */
