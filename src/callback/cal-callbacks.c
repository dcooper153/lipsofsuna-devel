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

typedef struct _licalCalltype licalCalltype;
struct _licalCalltype
{
	licalMarshal marshal;
	licalCallfunc* funcs;
};

static void
private_cleanup (licalCallbacks* self);

static void
private_free_type (licalCalltype* type);

/*****************************************************************************/

licalCallbacks*
lical_callbacks_new ()
{
	licalCallbacks* self;

	self = calloc (1, sizeof (licalCallbacks));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->types = lialg_u32dic_new ();
	if (self->types == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (self);
		return NULL;
	}

	return self;
}

void
lical_callbacks_free (licalCallbacks* self)
{
	lialgU32dicIter iter;

	private_cleanup (self);
	LI_FOREACH_U32DIC (iter, self->types)
		private_free_type (iter.value);
	lialg_u32dic_free (self->types);
	free (self);
}

int
lical_callbacks_call (licalCallbacks* self,
                      licalType       type,
                                      ...)
{
	int ret;
	va_list args;
	licalCalltype* typ;
	licalCallfunc* func;
	licalCallfunc* func_next;

	typ = lialg_u32dic_find (self->types, type);
	if (typ == NULL)
		return 0;
	for (func = typ->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		if (func->removed)
			continue;
		va_start (args, type);
		ret = typ->marshal (func->call, func->data, args);
		va_end (args);
		if (!ret)
			return 0;
	}

	return 1;
}

int
lical_callbacks_callva (licalCallbacks* self,
                        licalType       type,
                        va_list         args)
{
	int ret;
	va_list copy;
	licalCalltype* typ;
	licalCallfunc* func;
	licalCallfunc* func_next;

	typ = lialg_u32dic_find (self->types, type);
	if (typ == NULL)
		return 0;
	for (func = typ->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		if (func->removed)
			continue;
		va_copy (copy, args);
		ret = typ->marshal (func->call, func->data, copy);
		va_end (copy);
		if (!ret)
			return 0;
	}

	return 1;
}

int
lical_callbacks_insert_type (licalCallbacks* self,
                             licalType       type,
                             licalMarshal    marshal)
{
	licalCalltype* typ;

	typ = calloc (1, sizeof (licalCalltype));
	if (typ == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	typ->marshal = marshal;
	if (!lialg_u32dic_insert (self->types, type, typ))
	{
		lisys_error_set (ENOMEM, NULL);
		free (typ);
		return 0;
	}

	return 1;
}

void
lical_callbacks_update (licalCallbacks* self)
{
	private_cleanup (self);
}

void
lical_callbacks_remove_type (licalCallbacks* self,
                             licalType       type)
{
	licalCalltype* typ;

	typ = lialg_u32dic_find (self->types, type);
	if (typ == NULL)
		return;
	private_free_type (typ);
	lialg_u32dic_remove (self->types, type);
}

licalHandle
lical_callbacks_insert_callback (licalCallbacks* self,
                                 licalType       type,
                                 int             priority,
                                 void*           call,
                                 void*           data)
{
	licalCalltype* typ;
	licalCallfunc* ptr;
	licalCallfunc* func;

	/* Find type. */
	typ = lialg_u32dic_find (self->types, type);
	if (typ == NULL)
	{
		lisys_error_set (EINVAL, "invalid call type %d", type);
		return NULL;
	}

	/* Allocate function. */
	func = calloc (1, sizeof (licalCallfunc));
	if (func == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	func->prio = priority;
	func->call = call;
	func->data = data;

	/* Insertion sort by priority. */
	if (typ->funcs == NULL)
	{
		typ->funcs = func;
		return func;
	}
	if (priority < typ->funcs->prio)
	{
		func->next = typ->funcs;
		typ->funcs->prev = func;
		typ->funcs = func;
		return func;
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

	return func;
}

void
lical_callbacks_remove_callback (licalCallbacks* self,
                                 licalType       type,
                                 licalHandle     handle)
{
	licalCalltype* typ;
	licalCallfunc* func;

	typ = lialg_u32dic_find (self->types, type);
	if (typ == NULL)
		return;
	for (func = typ->funcs ; func != NULL ; func = func->next)
	{
		if (func == handle)
		{
			/* Remove from type. */
			if (func->prev == NULL)
				typ->funcs = func->next;
			else
				func->prev->next = func->next;
			if (func->next != NULL)
				func->next->prev = func->prev;

			/* Queue for removal. */
			func->prev = self->removed;
			func->removed = 1;
			self->removed = func;
			break;
		}
	}
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
		free (func);
	}
	self->removed = NULL;
}

static void
private_free_type (licalCalltype* type)
{
	licalCallfunc* func;
	licalCallfunc* func_next;

	for (func = type->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		free (func);
	}
	free (type);
}

/** @} */
/** @} */
