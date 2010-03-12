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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "system-error.h"
#include "system-memory.h"

void*
lisys_calloc (size_t num,
              size_t size)
{
	void* mem;

	mem = calloc (num, size);
	if (mem == NULL)
		lisys_error_set (ENOMEM, NULL);

	return mem;
}

void*
lisys_malloc (size_t size)
{
	void* mem;

	mem = malloc (size);
	if (mem == NULL)
		lisys_error_set (ENOMEM, NULL);

	return mem;
}

void*
lisys_realloc (void*  mem,
               size_t size)
{
	void* mem1;

	mem1 = realloc (mem, size);
	if (mem1 == NULL && size)
		lisys_error_set (ENOMEM, NULL);

	return mem1;
}

void
lisys_free (void* mem)
{
	free (mem);
}
