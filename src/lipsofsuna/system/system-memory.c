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

#ifdef MEMDEBUG
#define __USE_GNU 1
#include <pthread.h>
#include "lipsofsuna/algorithm.h"
static int private_mem_skip = 0;
static LIAlgPtrdic* private_mem_dict = NULL;
static pthread_mutex_t private_mem_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
static void private_mem_add (
	void* mem)
{
	pthread_mutex_lock (&private_mem_mutex);
	if (!private_mem_skip && mem != NULL)
	{
		private_mem_skip++;
		if (private_mem_dict == NULL)
			private_mem_dict = lialg_ptrdic_new ();
		lialg_ptrdic_insert (private_mem_dict, mem, NULL + 1);
		private_mem_skip--;
	}
	pthread_mutex_unlock (&private_mem_mutex);
}
static void private_mem_del (
	void* mem)
{
	pthread_mutex_lock (&private_mem_mutex);
	if (!private_mem_skip && mem != NULL)
	{
		private_mem_skip++;
		lisys_assert (private_mem_dict != NULL);
		lisys_assert (lialg_ptrdic_find (private_mem_dict, mem));
		lialg_ptrdic_remove (private_mem_dict, mem);
		private_mem_skip--;
	}
	pthread_mutex_unlock (&private_mem_mutex);
}
#else
#define private_mem_add(mem)
#define private_mem_del(mem)
#endif

void* lisys_calloc (
	size_t num,
	size_t size)
{
	void* mem;

	mem = calloc (num, size);
	if (mem == NULL)
		lisys_error_set (ENOMEM, NULL);
	private_mem_add (mem);

	return mem;
}

void* lisys_malloc (
	size_t size)
{
	void* mem;

	mem = malloc (size);
	if (mem == NULL)
		lisys_error_set (ENOMEM, NULL);
	private_mem_add (mem);

	return mem;
}

void* lisys_realloc (
	void*  mem,
	size_t size)
{
	void* mem1;

	mem1 = realloc (mem, size);
	if (mem1 == NULL && size)
		lisys_error_set (ENOMEM, NULL);
	private_mem_del (mem);
	private_mem_add (mem1);

	return mem1;
}

void lisys_free (void* mem)
{
	private_mem_del (mem);
	free (mem);
}
