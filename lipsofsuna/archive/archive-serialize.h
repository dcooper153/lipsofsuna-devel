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
 * \addtogroup liarcSerialize Serialize
 * @{
 */

#ifndef __ARCHIVE_SERIALIZE_H__
#define __ARCHIVE_SERIALIZE_H__

#include <algorithm/lips-algorithm.h>
#include <string/lips-string.h>
#include "archive-writer.h"

typedef struct _liarcSerialize liarcSerialize;
struct _liarcSerialize
{
	lialgList* stack;
	lialgPtrdic* objects;
	lialgPtrdic* pointers;
	liReader* reader;
	liarcWriter* writer;
};

liarcSerialize*
liarc_serialize_new_read (const char* file);

liarcSerialize*
liarc_serialize_new_write (const char* file);

void
liarc_serialize_free (liarcSerialize* self);

void*
liarc_serialize_find_object (liarcSerialize* self,
                             int             value);

int
liarc_serialize_find_object_id (liarcSerialize* self,
                                void*           value);

void*
liarc_serialize_find_pointer (liarcSerialize* self,
                              int             value);

int
liarc_serialize_find_pointer_id (liarcSerialize* self,
                                 void*           value);

int
liarc_serialize_insert_object (liarcSerialize* self,
                               void*           value);

int
liarc_serialize_insert_pointer (liarcSerialize* self,
                                int             key,
                                void*           value);

liarcWriter*
liarc_serialize_pop (liarcSerialize* self);

int
liarc_serialize_push (liarcSerialize* self);

int
liarc_serialize_read_object (liarcSerialize* self,
                             void*           value);

int
liarc_serialize_write_object (liarcSerialize* self,
                              void*           value);

int
liarc_serialize_get_write (const liarcSerialize* self);

#endif

/** @} */
/** @} */
