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
 * \addtogroup liarcReader Reader
 * @{
 */

#ifndef __ARCHIVE_READER_H__
#define __ARCHIVE_READER_H__

#include <system/lips-system.h>

typedef struct _liarcReader liarcReader;
struct _liarcReader
{
	int pos;
	int length;
	lisysMmap* mmap;
	const char* buffer;
};

#ifdef __cplusplus
extern "C" {
#endif

liarcReader*
liarc_reader_new (const char* buffer, int length);

liarcReader*
liarc_reader_new_from_file (const char* path);

liarcReader*
liarc_reader_new_from_string (const char* buffer);

void liarc_reader_free (liarcReader* self);
int liarc_reader_check_end (liarcReader* self);
int liarc_reader_check_char (liarcReader* self, char c);
int liarc_reader_check_data (liarcReader* self, const void* data, int length);
int liarc_reader_check_text (liarcReader* self, const char* data, const char* list);
int liarc_reader_check_key_value_pair (liarcReader* self, const char* key, const char* value);
int liarc_reader_check_uint32 (liarcReader* self, uint32_t value);
int liarc_reader_get_char (liarcReader* self, char* value);
int liarc_reader_get_float (liarcReader* self, float* value);
int liarc_reader_get_int8 (liarcReader* self, int8_t* value);
int liarc_reader_get_int16 (liarcReader* self, int16_t* value);
int liarc_reader_get_int32 (liarcReader* self, int32_t* value);
int liarc_reader_get_offset (liarcReader* self);
int liarc_reader_set_offset (liarcReader* self, int offset);
int liarc_reader_get_uint8 (liarcReader* self, uint8_t* value);
int liarc_reader_get_uint16 (liarcReader* self, uint16_t* value);
int liarc_reader_get_uint32 (liarcReader* self, uint32_t* value);
int liarc_reader_get_text (liarcReader* self, const char* list, char** value);
int liarc_reader_get_text_int (liarcReader* self, int* value);
int liarc_reader_get_text_uint (liarcReader* self, int* value);
int liarc_reader_get_text_float (liarcReader* self, float* value);
int liarc_reader_get_key_value_pair (liarcReader* self, char** key, char** value);
int liarc_reader_skip_bytes (liarcReader* self, int num);
int liarc_reader_skip_chars (liarcReader* self, const char* list);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
