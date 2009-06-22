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
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysFile File
 * @{
 */

#ifndef __SYSTEM_FILE_H__
#define __SYSTEM_FILE_H__

#include <stdio.h>

enum
{
	LISYS_STAT_BLOCK,
	LISYS_STAT_CHAR,
	LISYS_STAT_DIRECTORY,
	LISYS_STAT_FILE,
	LISYS_STAT_LINK
};

typedef struct _lisysMmap lisysMmap;
typedef struct _lisysStat lisysStat;
struct _lisysStat
{
	int uid;
	int gid;
	int mode;
	int size;
	int type;
	long mtime;
	long rdev;
};

#ifdef __cplusplus
extern "C" {
#endif

lisysMmap*
lisys_mmap_open (const char* path);

void
lisys_mmap_free (lisysMmap* self);

void*
lisys_mmap_get_buffer (lisysMmap* self);

int
lisys_mmap_get_size (lisysMmap* self);

char*
lisys_readlink (const char* path);

int
lisys_stat (const char* path,
            lisysStat*  result);

int
lisys_lstat (const char* path,
             lisysStat*  result);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
