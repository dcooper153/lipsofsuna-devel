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
 * \addtogroup liarc Archive
 * @{
 * \addtogroup liarcTar Tar
 * @{
 */

#ifndef __ARCHIVE_TAR_H__
#define __ARCHIVE_TAR_H__

#include "archive-writer.h"

enum
{
	LIARC_TAR_FILE = '0',
	LIARC_TAR_LINK = '2',
	LIARC_TAR_CHAR = '3',
	LIARC_TAR_BLOCK = '4',
	LIARC_TAR_DIRECTORY = '5'
};

typedef struct _liarcTarHeader liarcTarHeader;
struct _liarcTarHeader
{
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char reserved[12];
};

typedef struct _liarcTar liarcTar;
struct _liarcTar
{
	liarcWriter* writer;
};

liarcTar*
liarc_tar_new (liarcWriter* writer);

void
liarc_tar_free (liarcTar* self);

int
liarc_tar_write_data (liarcTar*   self,
                      const char* name,
                      const void* data,
                      int         length);

int
liarc_tar_write_directory (liarcTar*   self,
                           const char* name);

int
liarc_tar_write_end (liarcTar* self);

int
liarc_tar_write_file (liarcTar*   self,
                      const char* src,
                      const char* dst);

#endif

/** @} */
/** @} */
