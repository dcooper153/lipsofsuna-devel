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

#ifndef __SYSTEM_MEMORY_H__
#define __SYSTEM_MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

#define lisys_calloc lisys_calloc_func
#define lisys_malloc lisys_malloc_func
#define lisys_realloc lisys_realloc_func
#define lisys_free lisys_free_func
void* lisys_calloc_func (size_t num, size_t size);
void* lisys_malloc_func (size_t size);
void* lisys_realloc_func (void*, size_t size);
void lisys_free_func (void* mem);

#ifdef LI_MALLOC_DEBUG
#undef lisys_calloc
#undef lisys_malloc
#undef lisys_realloc
#undef lisys_free
#define lisys_calloc(n,s) lisys_calloc_log ((n), (s), "calloc %d %s %d\n", (n) * (s), __FILE__, __LINE__)
#define lisys_malloc(s) lisys_malloc_log ((s), "malloc %d %s %d\n", (s), __FILE__, __LINE__)
#define lisys_realloc(m,s) lisys_realloc_log ((m), (s), "realloc %d %s %d\n", (s), __FILE__, __LINE__)
#define lisys_free(m) lisys_free_log ((m), "free %s %d\n", __FILE__, __LINE__)
void* lisys_calloc_log (size_t num, size_t size, const char* fmt, ...);
void* lisys_malloc_log (size_t size, const char* fmt, ...);
void* lisys_realloc_log (void* mem, size_t size, const char* fmt, ...);
void lisys_free_log (void* mem, const char* fmt, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif
