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
 * \addtogroup lical Callback
 * @{
 * \addtogroup LICalHandle Handle
 * @{
 */

#include "cal-callbacks.h"
#include "cal-handle.h"

typedef struct _LICalCalladdr LICalCalladdr;
struct _LICalCalladdr
{
	void* object;
	char type[32];
};

typedef struct _LICalCalltype LICalCalltype;
struct _LICalCalltype
{
	LICalCallfunc* funcs;
};

static void
private_addr_type (LICalCalladdr* self,
                   void*          object,
                   const char*    type);

static void
private_free_type (LICalCalltype* self);

/*****************************************************************************/

/**
 * \brief Releases an event handler callback.
 *
 * \param self Handle.
 */
void
lical_handle_release (LICalHandle* self)
{
	LICalCalladdr addr;
	LICalCalltype* typ;
	LICalCallfunc* func;

	if (self->func == NULL)
		return;
	private_addr_type (&addr, self->object, self->type);
	typ = lialg_memdic_find (self->calls->types, &addr, sizeof (addr));
	if (typ == NULL)
		return;
	for (func = typ->funcs ; func != NULL ; func = func->next)
	{
		if (func == self->func)
		{
			/* Remove from type. */
			if (func->prev == NULL)
				typ->funcs = func->next;
			else
				func->prev->next = func->next;
			if (func->next != NULL)
				func->next->prev = func->prev;

			/* Remove empty types. */
			if (typ->funcs == NULL)
			{
				lialg_memdic_remove (self->calls->types, &addr, sizeof (addr));
				private_free_type (typ);
			}

			/* Queue for removal. */
			func->prev = self->calls->removed;
			func->removed = 1;
			self->calls->removed = func;
			break;
		}
	}
	memset (self, 0, sizeof (LICalHandle));
}

/**
 * \brief Releases event handler callbacks.
 *
 * \param self Array of handles.
 * \param count Number of handles.
 */
void
lical_handle_releasev (LICalHandle* self,
                       int          count)
{
	int i;

	for (i = 0 ; i < count ; i++)
		lical_handle_release (self + i);
}

/*****************************************************************************/

static void
private_addr_type (LICalCalladdr* self,
                   void*          object,
                   const char*    type)
{
	memset (self, 0, sizeof (LICalCalladdr));
	self->object = object;
	strncpy (self->type, type, sizeof (self->type) - 1);
}

static void
private_free_type (LICalCalltype* self)
{
	LICalCallfunc* func;
	LICalCallfunc* func_next;

	for (func = self->funcs ; func != NULL ; func = func_next)
	{
		func_next = func->next;
		lisys_free (func);
	}
	lisys_free (self);
}

/** @} */
/** @} */
