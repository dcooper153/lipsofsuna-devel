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
 * \addtogroup liarc Archive
 * @{
 * \addtogroup LIArcWriter Writer
 * @{
 */

#ifndef __ARCHIVE_WRITER_H__
#define __ARCHIVE_WRITER_H__

#include <stdio.h>
#include <stdarg.h>
#include <lipsofsuna/system.h>

typedef struct _LIArcWriter LIArcWriter;
struct _LIArcWriter
{
	int error;
	void (*close)(LIArcWriter*);
	int (*write)(LIArcWriter*, const void*, int);
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

LIArcWriter*
liarc_writer_new ();

LIArcWriter*
liarc_writer_new_file (const char* path);

LIArcWriter*
liarc_writer_new_gzip (const char* path);

LIArcWriter*
liarc_writer_new_packet (int type);

void
liarc_writer_free (LIArcWriter* self);

int
liarc_writer_append_file (LIArcWriter* self,
                          FILE*        file);

int
liarc_writer_append_format (LIArcWriter* self,
                            const char*  format,
                                         ...) __LI_ATTRIBUTE_FORMAT(2, 3);

int
liarc_writer_append_formatv (LIArcWriter* self,
                             const char*  format,
                             va_list      args);

int
liarc_writer_append_string (LIArcWriter* self,
                            const char*  string);

int
liarc_writer_append_float (LIArcWriter* self,
                           float        value);

int
liarc_writer_append_int8 (LIArcWriter* self,
                          int8_t       value);

int
liarc_writer_append_int16 (LIArcWriter* self,
                           int16_t      value);

int
liarc_writer_append_int32 (LIArcWriter* self,
                           int32_t      value);

int
liarc_writer_append_nul (LIArcWriter* self);

int
liarc_writer_append_uint8 (LIArcWriter* self,
                           uint8_t      value);

int
liarc_writer_append_uint16 (LIArcWriter* self,
                            uint16_t     value);

int
liarc_writer_append_uint32 (LIArcWriter* self,
                            uint32_t     value);

int
liarc_writer_append_raw (LIArcWriter* self,
                         const void*  data,
                         int          size);

void
liarc_writer_clear (LIArcWriter* self);

int
liarc_writer_erase (LIArcWriter* self,
                    int          size);

const char*
liarc_writer_get_buffer (const LIArcWriter* self);

int
liarc_writer_get_length (const LIArcWriter* self);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
