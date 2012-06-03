/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __MAIN_EVENT_H__
#define __MAIN_EVENT_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/archive.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/script.h"

#define LIMAI_EVENT_FIELDS_MAX 16

enum
{
	LIMAI_FIELD_BOOL,
	LIMAI_FIELD_FLOAT,
	LIMAI_FIELD_INT,
	LIMAI_FIELD_PACKET,
	LIMAI_FIELD_STRING,
	LIMAI_FIELD_VECTOR
};

typedef struct _LIMaiEventField LIMaiEventField;
struct _LIMaiEventField
{
	char* name;
	int type;
	union
	{
		int value_bool;
		float value_float;
		int value_int;
		char* value_string;
		LIArcPacket* value_packet;
		LIMatVector value_vector;
	};
};

typedef struct _LIMaiEvent LIMaiEvent;
struct _LIMaiEvent
{
	LIMaiEvent* next;
	LIMaiEvent* prev;
	char* type;
	struct
	{
		int count;
		LIMaiEventField array[LIMAI_EVENT_FIELDS_MAX];
	} fields;
};

LIAPICALL (LIMaiEvent*, limai_event_new, (
	const char* type,
	va_list     args));

LIAPICALL (void, limai_event_free, (
	LIMaiEvent* self));

LIAPICALL (void, limai_event_write_script, (
	LIMaiEvent*  self,
	LIScrScript* script));

#endif
