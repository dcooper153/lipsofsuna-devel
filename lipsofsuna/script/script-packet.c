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
 * \addtogroup liscr Script
 * @{
 * \addtogroup LIScrPacket Packet
 * @{
 */

#include <string.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/script.h>

static int
private_read (LIScrPacket* data,
              lua_State*   lua);

/*****************************************************************************/

/* @luadoc
 * module "Core.Common.Packet"
 * ---
 * -- Send and receive network data.
 * -- @name Packet
 * -- @class table
 */

/* @luadoc
 * ---
 * -- First packet number for custom packets.
 * -- @name Packet.CUSTOM
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new packet.
 * --
 * -- @param self Packet class.
 * -- @param type Packet type.
 * -- @param ... Packet contents.
 * -- @return New packet.
 * function Packet.new(self, type, ...)
 */
static int
Packet_new (lua_State* lua)
{
	int err;
	int type;
	LIScrData* self;
	LIScrScript* script = liscr_script (lua);

	liscr_checkclass (lua, 1, LISCR_SCRIPT_PACKET);
	type = luaL_checkinteger (lua, 2);

	/* Allocate packet. */
	self = liscr_packet_new_writable (script, type);
	if (self == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Write content. */
	lua_getfield (lua, LUA_GLOBALSINDEX, "Packet");
	lua_getfield (lua, -1, "write");
	lua_remove (lua, -2);
	lua_replace (lua, 1);
	liscr_pushdata (lua, self);
	lua_replace (lua, 2);
	err = lua_pcall (lua, lua_gettop (lua) - 1, 0, 0);
	if (err != 0)
	{
		liscr_data_unref (self, NULL);
		lua_error (lua);
	}

	/* Return packet. */
	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Reads data starting from the beginning of the packet.
 * --
 * -- @param self Packet.
 * -- @param ... Types to read.
 * -- @return Boolean and a list of read values.
 * function Packet.read(self, ...)
 */
static int
Packet_read (lua_State* lua)
{
	LIScrData* self;
	LIScrPacket* data;

	self = liscr_checkdata (lua, 1, LISCR_SCRIPT_PACKET);
	data = self->data;
	luaL_argcheck (lua, data->reader != NULL, 1, "packet is not readable");
	data->reader->pos = 1;

	return private_read (data, lua);
}

/* @luadoc
 * ---
 * -- Reads data starting from the last read positiong of the packet.
 * --
 * -- @param self Packet.
 * -- @param ... Types to read.
 * -- @return Boolean and a list of read values.
 * function Packet.resume(self, ...)
 */
static int
Packet_resume (lua_State* lua)
{
	LIScrData* self;
	LIScrPacket* data;

	self = liscr_checkdata (lua, 1, LISCR_SCRIPT_PACKET);
	data = self->data;
	luaL_argcheck (lua, data->reader != NULL, 1, "packet is not readable");

	return private_read (data, lua);
}

/* @luadoc
 * ---
 * -- Appends data to the packet.
 * --
 * -- @param self Packet.
 * -- @param ... Types to write.
 * function Packet.write(self, ...)
 */
static int
Packet_write (lua_State* lua)
{
	int i;
	const char* type;
	LIScrData* self;
	LIScrPacket* data;
	union
	{
		int8_t i8;
		int16_t i16;
		int32_t i32;
		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		float flt;
		const char* str;
	} tmp;

	self = liscr_checkdata (lua, 1, LISCR_SCRIPT_PACKET);
	data = self->data;
	luaL_argcheck (lua, data->writer != NULL, 1, "packet is not writable");

	/* Write content. */
	for (i = 2 ; i <= lua_gettop (lua) ; i++)
	{
		type = luaL_checkstring (lua, i++);
		if (!strcmp (type, "bool"))
		{
			tmp.u8 = lua_toboolean (lua, i);
			liarc_writer_append_uint8 (data->writer, tmp.u8);
		}
		else if (!strcmp (type, "float"))
		{
			tmp.flt = luaL_checknumber (lua, i);
			liarc_writer_append_float (data->writer, tmp.flt);
		}
		else if (!strcmp (type, "int8"))
		{
			tmp.i8 = luaL_checknumber (lua, i);
			liarc_writer_append_int8 (data->writer, tmp.i8);
		}
		else if (!strcmp (type, "int16"))
		{
			tmp.i16 = luaL_checknumber (lua, i);
			liarc_writer_append_int16 (data->writer, tmp.i16);
		}
		else if (!strcmp (type, "int32"))
		{
			tmp.i32 = luaL_checknumber (lua, i);
			liarc_writer_append_int32 (data->writer, tmp.i32);
		}
		else if (!strcmp (type, "string"))
		{
			tmp.str = luaL_checkstring (lua, i);
			liarc_writer_append_string (data->writer, tmp.str);
			liarc_writer_append_nul (data->writer);
		}
		else if (!strcmp (type, "uint8"))
		{
			tmp.u8 = luaL_checknumber (lua, i);
			liarc_writer_append_uint8 (data->writer, tmp.u8);
		}
		else if (!strcmp (type, "uint16"))
		{
			tmp.u16 = luaL_checknumber (lua, i);
			liarc_writer_append_uint16 (data->writer, tmp.u16);
		}
		else if (!strcmp (type, "uint32"))
		{
			tmp.u32 = luaL_checknumber (lua, i);
			liarc_writer_append_uint32 (data->writer, tmp.u32);
		}
		else
			luaL_argerror (lua, i - 1, "invalid format");
	}

	return 0;
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

void
liscr_script_packet (LIScrClass* self,
                   void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_func (self, "new", Packet_new);
	liscr_class_insert_func (self, "read", Packet_read);
	liscr_class_insert_func (self, "resume", Packet_resume);
	liscr_class_insert_func (self, "write", Packet_write);
	liscr_class_insert_mvar (self, "size", Packet_getter_size, NULL);
	liscr_class_insert_mvar (self, "type", Packet_getter_type, Packet_setter_type);
}

LIScrData*
liscr_packet_new_readable (LIScrScript*       script,
                           const LIArcReader* reader)
{
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
	data = liscr_data_new (script, self, LISCR_SCRIPT_PACKET, liscr_packet_free);
	if (data == NULL)
	{
		liarc_reader_free (self->reader);
		lisys_free (self->buffer);
		lisys_free (self);
	}

	return data;
}

LIScrData*
liscr_packet_new_writable (LIScrScript* script,
                           int          type)
{
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
	data = liscr_data_new (script, self, LISCR_SCRIPT_PACKET, liscr_packet_free);
	if (data == NULL)
	{
		liarc_writer_free (self->writer);
		lisys_free (self);
		return NULL;
	}

	return data;
}

void
liscr_packet_free (LIScrPacket* self)
{
	if (self->writer != NULL)
		liarc_writer_free (self->writer);
	if (self->reader != NULL)
		liarc_reader_free (self->reader);
	lisys_free (self->buffer);
	lisys_free (self);
}

/*****************************************************************************/

static int
private_read (LIScrPacket* data,
              lua_State*   lua)
{
	int i;
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

	/* Read content. */
	for (i = 2 ; i <= lua_gettop (lua) ; i++)
	{
		type = luaL_checkstring (lua, i);
		if (!strcmp (type, "bool"))
		{
			if (ok) ok &= liarc_reader_get_int8 (data->reader, &tmp.i8);
			if (ok) lua_pushboolean (lua, tmp.i8);
		}
		else if (!strcmp (type, "float"))
		{
			if (ok) ok &= liarc_reader_get_float (data->reader, &tmp.flt);
			if (ok) lua_pushnumber (lua, tmp.flt);
		}
		else if (!strcmp (type, "int8"))
		{
			if (ok) ok &= liarc_reader_get_int8 (data->reader, &tmp.i8);
			if (ok) lua_pushnumber (lua, tmp.i8);
		}
		else if (!strcmp (type, "int16"))
		{
			if (ok) ok &= liarc_reader_get_int16 (data->reader, &tmp.i16);
			if (ok) lua_pushnumber (lua, tmp.i16);
		}
		else if (!strcmp (type, "int32"))
		{
			if (ok) ok &= liarc_reader_get_int32 (data->reader, &tmp.i32);
			if (ok) lua_pushnumber (lua, tmp.i32);
		}
		else if (!strcmp (type, "string"))
		{
			tmp.str = NULL;
			if (ok) ok &= liarc_reader_get_text (data->reader, "", &tmp.str);
			if (ok) ok &= listr_utf8_get_valid (tmp.str);
			if (ok) lua_pushstring (lua, tmp.str);
			lisys_free (tmp.str);
		}
		else if (!strcmp (type, "uint8"))
		{
			if (ok) ok &= liarc_reader_get_uint8 (data->reader, &tmp.u8);
			if (ok) lua_pushnumber (lua, tmp.u8);
		}
		else if (!strcmp (type, "uint16"))
		{
			if (ok) ok &= liarc_reader_get_uint16 (data->reader, &tmp.u16);
			if (ok) lua_pushnumber (lua, tmp.u16);
		}
		else if (!strcmp (type, "uint32"))
		{
			if (ok) ok &= liarc_reader_get_uint32 (data->reader, &tmp.u32);
			if (ok) lua_pushnumber (lua, tmp.u32);
		}
		else
			luaL_argerror (lua, i, "invalid format");
		if (!ok)
			lua_pushnil (lua);
		lua_replace (lua, i);
	}

	lua_pushboolean (lua, ok);
	lua_replace (lua, 1);
	return lua_gettop (lua);
}

/** @} */
/** @} */
