/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup listr String
 * @{
 * \addtogroup listrReader Reader
 * @{
 */

#ifndef __STRING_READER_H__
#define __STRING_READER_H__

#include <system/lips-system.h>

typedef struct _liReader liReader;
struct _liReader
{
	int pos;
	int mmap;
	int length;
	const char* buffer;
};

LI_BEGIN_DECLS

liReader* li_reader_new (const char* buffer, int length);
liReader* li_reader_new_from_file (const char* path);
liReader* li_reader_new_from_string (const char* buffer);
void      li_reader_free (liReader* self);
int       li_reader_check_end (liReader* self);
int       li_reader_check_char (liReader* self, char c);
int       li_reader_check_data (liReader* self, const void* data, int length);
int       li_reader_check_text (liReader* self, const char* data, const char* list);
int       li_reader_check_key_value_pair (liReader* self, const char* key, const char* value);
int       li_reader_check_uint32 (liReader* self, uint32_t value);
int       li_reader_get_char (liReader* self, char* value);
int       li_reader_get_compressed (liReader* self, char** value, int* length);
int       li_reader_get_float (liReader* self, float* value);
int       li_reader_get_int8 (liReader* self, int8_t* value);
int       li_reader_get_int16 (liReader* self, int16_t* value);
int       li_reader_get_int32 (liReader* self, int32_t* value);
int       li_reader_get_offset (liReader* self);
int       li_reader_set_offset (liReader* self, int offset);
int       li_reader_get_uint8 (liReader* self, uint8_t* value);
int       li_reader_get_uint16 (liReader* self, uint16_t* value);
int       li_reader_get_uint32 (liReader* self, uint32_t* value);
int       li_reader_get_text (liReader* self, const char* list, char** value);
int       li_reader_get_text_int (liReader* self, int* value);
int       li_reader_get_text_uint (liReader* self, int* value);
int       li_reader_get_text_float (liReader* self, float* value);
int       li_reader_get_key_value_pair (liReader* self, char** key, char** value);
int       li_reader_skip_bytes (liReader* self, int num);
int       li_reader_skip_chars (liReader* self, const char* list);

LI_END_DECLS

#endif

/** @} */
/** @} */
