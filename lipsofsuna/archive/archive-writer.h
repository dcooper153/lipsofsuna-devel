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
 * \addtogroup liarcWriter Writer
 * @{
 */

#ifndef __ARCHIVE_WRITER_H__
#define __ARCHIVE_WRITER_H__

#include <stdio.h>
#include <stdarg.h>
#include <system/lips-system.h>

typedef struct _liarcWriter liarcWriter;
struct _liarcWriter
{
	int error;
	void (*close)(liarcWriter*);
	int (*write)(liarcWriter*, const void*, int);
	struct
	{
		FILE* pointer;
	} file;
	struct
	{
		void* pointer;
	} gzip;
	struct
	{
		char* buffer;
		int length;
		int capacity;
	} memory;
};

#ifdef __cplusplus
extern "C" {
#endif

liarcWriter*
liarc_writer_new ();

liarcWriter*
liarc_writer_new_file (const char* path);

liarcWriter*
liarc_writer_new_gzip (const char* path);

liarcWriter*
liarc_writer_new_packet (int type);

void
liarc_writer_free (liarcWriter* self);

int
liarc_writer_append_file (liarcWriter* self,
                          FILE*        file);

int
liarc_writer_append_format (liarcWriter* self,
                            const char*  format,
                                         ...) __LI_ATTRIBUTE_FORMAT(2, 3);

int
liarc_writer_append_formatv (liarcWriter* self,
                             const char*  format,
                             va_list      args);

int
liarc_writer_append_string (liarcWriter* self,
                            const char*  string);

int
liarc_writer_append_float (liarcWriter* self,
                           float        value);

int
liarc_writer_append_int8 (liarcWriter* self,
                          int8_t       value);

int
liarc_writer_append_int16 (liarcWriter* self,
                           int16_t      value);

int
liarc_writer_append_int32 (liarcWriter* self,
                           int32_t      value);

int
liarc_writer_append_nul (liarcWriter* self);

int
liarc_writer_append_uint8 (liarcWriter* self,
                           uint8_t      value);

int
liarc_writer_append_uint16 (liarcWriter* self,
                            uint16_t     value);

int
liarc_writer_append_uint32 (liarcWriter* self,
                            uint32_t     value);

int
liarc_writer_append_raw (liarcWriter* self,
                         const void*  data,
                         int          size);

void
liarc_writer_clear (liarcWriter* self);

int
liarc_writer_erase (liarcWriter* self,
                    int          size);

const char*
liarc_writer_get_buffer (const liarcWriter* self);

int
liarc_writer_get_length (const liarcWriter* self);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
