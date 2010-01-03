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
 * \addtogroup lisys System
 * @{
 * \addtogroup LISysDir Directory
 * @{
 */

#ifndef __SYSTEM_DIRECTORY_H__
#define __SYSTEM_DIRECTORY_H__

typedef int (*LISysDirFilter)(const char* dir, const char* name, void* data);
typedef int (*LISysDirSorter)(const char** name0, const char** name1);
typedef struct _LISysDir LISysDir;
struct _LISysDir;

#ifdef __cplusplus
extern "C" {
#endif

LISysDir*
lisys_dir_open (const char* path);

void
lisys_dir_free (LISysDir* self);

int
lisys_dir_scan (LISysDir* self);

int
lisys_dir_get_count (const LISysDir* self);

const char*
lisys_dir_get_name (const LISysDir* self,
                    int             i);

char*
lisys_dir_get_path (const LISysDir* self,
                    int             i);

void
lisys_dir_set_filter (LISysDir*      self,
                      LISysDirFilter filter,
                      void*          data);

void
lisys_dir_set_sorter (LISysDir*      self,
                      LISysDirSorter sorter);

int
LISYS_DIR_FILTER_FILES (const char* dir,
                        const char* name,
                        void*       data);

int
LISYS_DIR_FILTER_DIRS (const char* dir,
                       const char* name,
                       void*       data);

int
LISYS_DIR_FILTER_HIDDEN (const char* dir,
                         const char* name,
                         void*       data);

int
LISYS_DIR_FILTER_VISIBLE (const char* dir,
                          const char* name,
                          void*       data);

int
LISYS_DIR_SORTER_ALPHA (const char** name0,
                        const char** name1);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */

