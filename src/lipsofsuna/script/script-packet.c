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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrPacket Packet
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/script.h>
#include "script-private.h"

static void private_read (
	LIScrPacket* self,
	LIScrArgs*   args);

static void private_write (
	LIScrPacket* self,
	LIScrArgs*   args,
	int          start);

/*****************************************************************************/

/* @luadoc
 * module "builtin/packet"
 * ---
 * -- Send and receive network data.
 * -- @name Packet
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new packet.
 * -- @param self Packet class.
 * -- @param type Packet type.
 * -- @param ... Packet contents.
 * -- @return New packet.
 * function Packet.new(self, type, ...)
 */
static void Packet_new (LIScrArgs* args)
{
	int type = 0;
	LIScrData* self;

	liscr_args_geti_int (args, 0, &type);

	/* Allocate packet. */
	self = liscr_packet_new_writable (args->script, type);
	if (self == NULL)
		return;

	/* Write content. */
	private_write (self->data, args, 1);

	/* Return packet. */
	liscr_args_seti_data (args, self);
	liscr_data_unref (self);
}

/* @luadoc
 * ---
 * -- Reads data starting from the beginning of the packet.
 * -- @param self Packet.
 * -- @param ... Types to read.
 * -- @return Boolean and a list of read values.
 * function Packet.read(self, ...)
 */
static void Packet_read (LIScrArgs* args)
{
	LIScrPacket* self;

	self = args->self;
	if (self->reader != NULL)
	{
		self->reader->pos = 1;
		private_read (self, args);
	}
}

/* @luadoc
 * ---
 * -- Reads data starting from the last read positiong of the packet.
 * -- @param self Packet.
 * -- @param ... Types to read.
 * -- @return Boolean and a list of read values.
 * function Packet.resume(self, ...)
 */
static void Packet_resume (LIScrArgs* args)
{
	LIScrPacket* self;

	self = args->self;
	if (self->reader != NULL)
		private_read (self, args);
}

/* @luadoc
 * ---
 * -- Appends data to the packet.
 * -- @param self Packet.
 * -- @param ... Types to write.
 * function Packet.write(self, ...)
 */
static void Packet_write (LIScrArgs* args)
{
	LIScrPacket* self;

	self = args->self;
	if (self->writer != NULL)
		private_write (self, args, 0);
}

/* @luadoc
 * ---
 * -- Size in bytes.
 * -- @name Packet.size
 * -- @class table
 */
static void Packet_getter_size (LIScrArgs* args)
{
	LIScrPacket* self;

	self = args->self;
	if (self->reader != NULL)
		liscr_args_seti_int (args, self->reader->length);
	else
		liscr_args_seti_int (args, liarc_writer_get_length (self->writer));
}

/* @luadoc
 * ---
 * -- Type number.
 * -- @name Packet.type
 * -- @class table
 */
static void Packet_getter_type (LIScrArgs* args)
{
	LIScrPacket* self;

	self = args->self;
	if (self->reader != NULL)
		liscr_args_seti_int (args, ((uint8_t*) self->reader->buffer)[0]);
	else
		liscr_args_seti_int (args, ((uint8_t*) liarc_writer_get_buffer (self->writer))[0]);
}
static void Packet_setter_type (LIScrArgs* args)
{
	int value;
	uint8_t* buffer;
	LIScrPacket* self;

	self = args->self;
	if (self->writer != NULL)
	{
		if (liscr_args_geti_int (args, 0, &value))
		{
			buffer = (uint8_t*) liarc_writer_get_buffer (self->writer);
			*buffer = value;
		}
	}
}

/*****************************************************************************/

void liscr_script_packet (
	LIScrClass* self,
	void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Packet_new);
	liscr_class_insert_mfunc (self, "read", Packet_read);
	liscr_class_insert_mfunc (self, "resume", Packet_resume);
	liscr_class_insert_mfunc (self, "write", Packet_write);
	liscr_class_insert_mvar (self, "size", Packet_getter_size, NULL);
	liscr_class_insert_mvar (self, "type", Packet_getter_type, Packet_setter_type);
}

LIScrData* liscr_packet_new_readable (
	LIScrScript*       script,
	const LIArcReader* reader)
{
	LIScrClass* clss;
	LIScrData* data;
	LIScrPacket* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIScrPacket));
	if (self == NULL)
		return NULL;
	self->buffer = lisys_calloc (reader->length, 1);
	if (self->buffer == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate reader. */
	self->reader = liarc_reader_new (self->buffer, reader->length);
	if (self->reader == NULL)
	{
		lisys_free (self->buffer);
		lisys_free (self);
		return NULL;
	}
	memcpy (self->buffer, reader->buffer, reader->length);

	/* Allocate script data. */
	clss = liscr_script_find_class (script, LISCR_SCRIPT_PACKET);
	data = liscr_data_new (script, self, clss, liscr_packet_free);
	if (data == NULL)
	{
		liarc_reader_free (self->reader);
		lisys_free (self->buffer);
		lisys_free (self);
	}

	return data;
}

LIScrData* liscr_packet_new_writable (
	LIScrScript* script,
	int          type)
{
	LIScrClass* clss;
	LIScrData* data;
	LIScrPacket* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIScrPacket));
	if (self == NULL)
		return NULL;

	/* Allocate writer. */
	self->writer = liarc_writer_new_packet (type);
	if (self->writer == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate script data. */
	clss = liscr_script_find_class (script, LISCR_SCRIPT_PACKET);
	data = liscr_data_new (script, self, clss, liscr_packet_free);
	if (data == NULL)
	{
		liarc_writer_free (self->writer);
		lisys_free (self);
		return NULL;
	}

	return data;
}

void liscr_packet_free (
	LIScrPacket* self)
{
	if (self->writer != NULL)
		liarc_writer_free (self->writer);
	if (self->reader != NULL)
		liarc_reader_free (self->reader);
	lisys_free (self->buffer);
	lisys_free (self);
}

/*****************************************************************************/

static void private_read (
	LIScrPacket* self,
	LIScrArgs*   args)
{
	int i;
	int p;
	int ok = 1;
	const char* type;
	union
	{
		int8_t i8;
		int16_t i16;
		int32_t i32;
		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		float flt;
		char* str;
	} tmp;

	/* Check for valid format. */
	p = self->reader->pos;
	for (i = 0 ; liscr_args_geti_string (args, i, &type) ; i++)
	{
		if (!strcmp (type, "bool"))
			ok = liarc_reader_get_int8 (self->reader, &tmp.i8);
		else if (!strcmp (type, "float"))
			ok = liarc_reader_get_float (self->reader, &tmp.flt);
		else if (!strcmp (type, "int8"))
			ok = liarc_reader_get_int8 (self->reader, &tmp.i8);
		else if (!strcmp (type, "int16"))
			ok = liarc_reader_get_int16 (self->reader, &tmp.i16);
		else if (!strcmp (type, "int32"))
			ok = liarc_reader_get_int32 (self->reader, &tmp.i32);
		else if (!strcmp (type, "string"))
		{
			if (liarc_reader_get_text (self->reader, "", &tmp.str))
			{
				ok = listr_utf8_get_valid (tmp.str);
				lisys_free (tmp.str);
			}
			else
				ok = 0;
		}
		else if (!strcmp (type, "uint8"))
			ok = liarc_reader_get_uint8 (self->reader, &tmp.u8);
		else if (!strcmp (type, "uint16"))
			ok = liarc_reader_get_uint16 (self->reader, &tmp.u16);
		else if (!strcmp (type, "uint32"))
			ok = liarc_reader_get_uint32 (self->reader, &tmp.u32);
		else
			ok = 0;
		if (!ok)
			return;
	}
	liscr_args_seti_bool (args, 1);
	self->reader->pos = p;

	/* Read and return contents. */
	for (i = 0 ; liscr_args_geti_string (args, i, &type) ; i++)
	{
		liscr_args_geti_string (args, i, &type);
		if (!strcmp (type, "bool"))
		{
			liarc_reader_get_uint8 (self->reader, &tmp.u8);
			liscr_args_seti_bool (args, tmp.u8);
		}
		else if (!strcmp (type, "float"))
		{
			liarc_reader_get_float (self->reader, &tmp.flt);
			liscr_args_seti_float (args, tmp.flt);
		}
		else if (!strcmp (type, "int8"))
		{
			liarc_reader_get_int8 (self->reader, &tmp.i8);
			liscr_args_seti_int (args, tmp.i8);
		}
		else if (!strcmp (type, "int16"))
		{
			liarc_reader_get_int16 (self->reader, &tmp.i16);
			liscr_args_seti_int (args, tmp.i16);
		}
		else if (!strcmp (type, "int32"))
		{
			liarc_reader_get_int32 (self->reader, &tmp.i32);
			liscr_args_seti_float (args, tmp.i32);
		}
		else if (!strcmp (type, "string"))
		{
			if (liarc_reader_get_text (self->reader, "", &tmp.str))
			{
				liscr_args_seti_string (args, tmp.str);
				lisys_free (tmp.str);
			}
		}
		else if (!strcmp (type, "uint8"))
		{
			liarc_reader_get_uint8 (self->reader, &tmp.u8);
			liscr_args_seti_int (args, tmp.u8);
		}
		else if (!strcmp (type, "uint16"))
		{
			liarc_reader_get_uint16 (self->reader, &tmp.u16);
			liscr_args_seti_int (args, tmp.u16);
		}
		else if (!strcmp (type, "uint32"))
		{
			liarc_reader_get_uint32 (self->reader, &tmp.u32);
			liscr_args_seti_float (args, tmp.u32);
		}
	}
}

static void private_write (
	LIScrPacket* self,
	LIScrArgs*   args,
	int          start)
{
	int i;
	int bol;
	float flt;
	const char* str;
	const char* type;

	for (i = start ; liscr_args_geti_string (args, i, &type) ; i++)
	{
		i++;
		if (!strcmp (type, "bool"))
		{
			bol = 0;
			liscr_args_geti_bool_convert (args, i, &bol);
			liarc_writer_append_uint8 (self->writer, bol);
		}
		else if (!strcmp (type, "float"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_float (self->writer, flt);
		}
		else if (!strcmp (type, "int8"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int8 (self->writer, (int8_t) flt);
		}
		else if (!strcmp (type, "int16"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int16 (self->writer, (int16_t) flt);
		}
		else if (!strcmp (type, "int32"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int32 (self->writer, (int32_t) flt);
		}
		else if (!strcmp (type, "string"))
		{
			str = "";
			liscr_args_geti_string (args, i, &str);
			liarc_writer_append_string (self->writer, str);
			liarc_writer_append_nul (self->writer);
		}
		else if (!strcmp (type, "uint8"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int8 (self->writer, (uint8_t) flt);
		}
		else if (!strcmp (type, "uint16"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int16 (self->writer, (uint16_t) flt);
		}
		else if (!strcmp (type, "uint32"))
		{
			flt = 0.0f;
			liscr_args_geti_float (args, i, &flt);
			liarc_writer_append_int32 (self->writer, (uint32_t) flt);
		}
	}
}

/** @} */
/** @} */
