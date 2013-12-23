/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIMai Main
 * @{
 * \addtogroup LIMaiMemstat Memstat
 * @{
 */

#include "main-memstat.h"

LIMaiMemstat* limai_memstat_new ()
{
	LIMaiMemstat* self;

	self = lisys_calloc (1, sizeof (LIMaiMemstat));
	if (self == NULL)
		return 0;

	return self;
}

void limai_memstat_free (
	LIMaiMemstat* self)
{
	LIMaiMemstatObject* obj;
	LIMaiMemstatObject* obj_next;

	for (obj = self->first ; obj != NULL ; obj = obj_next)
	{
		obj_next = obj->next;
		lisys_free (obj);
	}
	lisys_free (self);
}

int limai_memstat_add_object (
	LIMaiMemstat* self,
	const char*   ext,
	const char*   cls,
	int           bytes)
{
	LIMaiMemstatObject* obj;

	obj = lisys_calloc (1, sizeof (LIMaiMemstatObject));
	if (obj == NULL)
		return 0;
	strncpy (obj->ext, ext, 128);
	strncpy (obj->cls, cls, 128);
	obj->ext[127] = '\0';
	obj->cls[127] = '\0';
	obj->bytes = bytes;

	if (self->last != NULL)
	{
		self->last->next = obj;
		self->last = obj;
	}
	else
	{
		self->first = obj;
		self->last = obj;
	}

	return 1;
}

/** @} */
/** @} */
