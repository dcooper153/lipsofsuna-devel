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
 * \addtogroup liscr Script
 * @{
 * \addtogroup liscrPacket Packet
 * @{
 */

#include <string.h>
#include <network/lips-network.h>
#include <script/lips-script.h>

static int
private_read (liscrPacket* data,
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
 * -- Read or write a boolean value.
 * -- @name Packet.BOOL
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
 * -- Read or write a floating point value.
 * -- @name Packet.FLOAT
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Read or write a signed 8-bit integer.
 * -- @name Packet.INT8
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Read or write a signed 16-bit integer.
 * -- @name Packet.INT16
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Read or write a signed 32-bit integer.
 * -- @name Packet.INT32
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Read or write a NUL terminated string.
 * -- @name Packet.STRING
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Read or write an unsigned 8-bit integer.
 * -- @name Packet.UINT8
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Read or write an unsigned 16-bit integer.
 * -- @name Packet.UINT16
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Read or write an unsigned 32-bit integer.
 * -- @name Packet.UINT32
 * -- @class table
 */

static int
Packet___gc (lua_State* lua)
{
	liscrData* self;
	liscrPacket* data;

	self = liscr_isdata (lua, 1, LICOM_SCRIPT_PACKET);
	data = self->data;

	liscr_packet_free (data);
	liscr_data_free (self);
	return 0;
}

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
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	liscr_checkclass (lua, 1, LICOM_SCRIPT_PACKET);
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
	liscrData* self;
	liscrPacket* data;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_PACKET);
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
	liscrData* self;
	liscrPacket* data;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_PACKET);
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
	int type;
	liscrData* self;
	liscrPacket* data;
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

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_PACKET);
	data = self->data;
	luaL_argcheck (lua, data->writer != NULL, 1, "packet is not writable");

	/* Write content. */
	for (i = 2 ; i <= lua_gettop (lua) ; i++)
	{
		type = luaL_checkinteger (lua, i++);
		switch (type)
		{
			case LISCR_PACKET_FORMAT_BOOL:
				tmp.u8 = lua_toboolean (lua, i);
				liarc_writer_append_uint8 (data->writer, tmp.u8);
				break;
			case LISCR_PACKET_FORMAT_FLOAT:
				tmp.flt = luaL_checknumber (lua, i);
				liarc_writer_append_float (data->writer, tmp.flt);
				break;
			case LISCR_PACKET_FORMAT_INT8:
				tmp.i8 = luaL_checknumber (lua, i);
				liarc_writer_append_int8 (data->writer, tmp.i8);
				break;
			case LISCR_PACKET_FORMAT_INT16:
				tmp.i16 = luaL_checknumber (lua, i);
				liarc_writer_append_int16 (data->writer, tmp.i16);
				break;
			case LISCR_PACKET_FORMAT_INT32:
				tmp.i32 = luaL_checknumber (lua, i);
				liarc_writer_append_int32 (data->writer, tmp.i32);
				break;
			case LISCR_PACKET_FORMAT_STRING:
				tmp.str = luaL_checkstring (lua, i);
				liarc_writer_append_string (data->writer, tmp.str);
				liarc_writer_append_nul (data->writer);
				break;
			case LISCR_PACKET_FORMAT_UINT8:
				tmp.u8 = luaL_checknumber (lua, i);
				liarc_writer_append_uint8 (data->writer, tmp.u8);
				break;
			case LISCR_PACKET_FORMAT_UINT16:
				tmp.u16 = luaL_checknumber (lua, i);
				liarc_writer_append_uint16 (data->writer, tmp.u16);
				break;
			case LISCR_PACKET_FORMAT_UINT32:
				tmp.u32 = luaL_checknumber (lua, i);
				liarc_writer_append_uint32 (data->writer, tmp.u32);
				break;
			default:
				luaL_argerror (lua, i - 1, "invalid format");
				break;
		}
	}

	return 0;
}

/* @luadoc
 * ---
 * -- Size in bytes.
 * -- @name Packet.size
 * -- @class table
 */
static int
Packet_getter_size (lua_State* lua)
{
	liscrData* self;
	liscrPacket* data;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_PACKET);
	data = self->data;

	if (data->reader != NULL)
		lua_pushnumber (lua, data->reader->length);
	else
		lua_pushnumber (lua, liarc_writer_get_length (data->writer));
	return 1;
}

/* @luadoc
 * ---
 * -- Type number.
 * -- @name Packet.type
 * -- @class table
 */
static int
Packet_getter_type (lua_State* lua)
{
	liscrData* self;
	liscrPacket* data;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_PACKET);
	data = self->data;

	if (data->reader != NULL)
		lua_pushnumber (lua, ((uint8_t*) data->reader->buffer)[0]);
	else
		lua_pushnumber (lua, ((uint8_t*) liarc_writer_get_buffer (data->writer))[0]);
	return 1;
}

/*****************************************************************************/

void
licomPacketScript (liscrClass* self,
                   void*       data)
{
	liscr_class_insert_enum (self, "BOOL", LISCR_PACKET_FORMAT_BOOL);
	liscr_class_insert_enum (self, "CUSTOM", LINET_SERVER_PACKET_CUSTOM);
	liscr_class_insert_enum (self, "FLOAT", LISCR_PACKET_FORMAT_FLOAT);
	liscr_class_insert_enum (self, "INT8", LISCR_PACKET_FORMAT_INT8);
	liscr_class_insert_enum (self, "INT16", LISCR_PACKET_FORMAT_INT16);
	liscr_class_insert_enum (self, "INT32", LISCR_PACKET_FORMAT_INT32);
	liscr_class_insert_enum (self, "STRING", LISCR_PACKET_FORMAT_STRING);
	liscr_class_insert_enum (self, "UINT8", LISCR_PACKET_FORMAT_UINT8);
	liscr_class_insert_enum (self, "UINT16", LISCR_PACKET_FORMAT_UINT16);
	liscr_class_insert_enum (self, "UINT32", LISCR_PACKET_FORMAT_UINT32);
	liscr_class_insert_func (self, "__gc", Packet___gc);
	liscr_class_insert_func (self, "new", Packet_new);
	liscr_class_insert_func (self, "read", Packet_read);
	liscr_class_insert_func (self, "resume", Packet_resume);
	liscr_class_insert_func (self, "write", Packet_write);
	liscr_class_insert_getter (self, "size", Packet_getter_size);
	liscr_class_insert_getter (self, "type", Packet_getter_type);
}

liscrData*
liscr_packet_new_readable (liscrScript*       script,
                           const liarcReader* reader)
{
	liscrData* data;
	liscrPacket* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liscrPacket));
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
	data = liscr_data_new (script, self, LICOM_SCRIPT_PACKET);
	if (data == NULL)
	{
		liarc_reader_free (self->reader);
		lisys_free (self->buffer);
		lisys_free (self);
	}

	return data;
}

liscrData*
liscr_packet_new_writable (liscrScript* script,
                           int          type)
{
	liscrData* data;
	liscrPacket* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liscrPacket));
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
	data = liscr_data_new (script, self, LICOM_SCRIPT_PACKET);
	if (data == NULL)
	{
		liarc_writer_free (self->writer);
		lisys_free (self);
		return NULL;
	}

	return data;
}

void
liscr_packet_free (liscrPacket* self)
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
private_read (liscrPacket* data,
              lua_State*   lua)
{
	int i;
	int ok = 1;
	int type;
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
		type = luaL_checkinteger (lua, i);
		switch (type)
		{
			case LISCR_PACKET_FORMAT_BOOL:
				if (ok) ok &= liarc_reader_get_int8 (data->reader, &tmp.i8);
				if (ok) lua_pushboolean (lua, tmp.i8);
				break;
			case LISCR_PACKET_FORMAT_FLOAT:
				if (ok) ok &= liarc_reader_get_float (data->reader, &tmp.flt);
				if (ok) lua_pushnumber (lua, tmp.flt);
				break;
			case LISCR_PACKET_FORMAT_INT8:
				if (ok) ok &= liarc_reader_get_int8 (data->reader, &tmp.i8);
				if (ok) lua_pushnumber (lua, tmp.i8);
				break;
			case LISCR_PACKET_FORMAT_INT16:
				if (ok) ok &= liarc_reader_get_int16 (data->reader, &tmp.i16);
				if (ok) lua_pushnumber (lua, tmp.i16);
				break;
			case LISCR_PACKET_FORMAT_INT32:
				if (ok) ok &= liarc_reader_get_int32 (data->reader, &tmp.i32);
				if (ok) lua_pushnumber (lua, tmp.i32);
				break;
			case LISCR_PACKET_FORMAT_STRING:
				tmp.str = NULL;
				if (ok) ok &= liarc_reader_get_text (data->reader, "", &tmp.str);
				if (ok) ok &= listr_utf8_get_valid (tmp.str);
				if (ok) lua_pushstring (lua, tmp.str);
				lisys_free (tmp.str);
				break;
			case LISCR_PACKET_FORMAT_UINT8:
				if (ok) ok &= liarc_reader_get_uint8 (data->reader, &tmp.u8);
				if (ok) lua_pushnumber (lua, tmp.u8);
				break;
			case LISCR_PACKET_FORMAT_UINT16:
				if (ok) ok &= liarc_reader_get_uint16 (data->reader, &tmp.u16);
				if (ok) lua_pushnumber (lua, tmp.u16);
				break;
			case LISCR_PACKET_FORMAT_UINT32:
				if (ok) ok &= liarc_reader_get_uint32 (data->reader, &tmp.u32);
				if (ok) lua_pushnumber (lua, tmp.u32);
				break;
			default:
				luaL_argerror (lua, i, "invalid format");
				break;
		}
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
