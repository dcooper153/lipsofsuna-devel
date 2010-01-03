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
 * \addtogroup liser Server
 * @{
 * \addtogroup liserCallbacks Callbacks
 * @{
 */

#include <lipsofsuna/network.h>
#include "server-callbacks.h"
#include "server-observer.h"
#include "server-script.h"

static int
private_client_client_login (LISerServer* server,
                             LIEngObject* object,
                             const char*  name,
                             const char*  pass)
{
#warning No accounts
#if 0
	char* path;
	LICfgAccount* account;

	/* Contruct the account path. */
	path = lisys_path_concat (self->paths->server_state, "accounts", message->NEW_USER.name, NULL);
	if (path == NULL)
	{
		grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
		lisys_free (pass);
		return 1;
	}

	/* Parse the account file. */
	account = licfg_account_new (path);
	lisys_free (path);
	if (account == NULL)
	{
		if (lisys_error_peek () != EIO)
		{
			grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
			lisys_free (pass);
			return 1;
		}
		if (!self->config.server->enable_create_account)
		{
			grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
			lisys_free (pass);
			return 1;
		}
	}

	/* Set credentials. */
	liser_client_set_account (client, message->NEW_USER.name);
	if (pass != NULL)
		liser_client_set_password (client, pass);
#endif

	return 1;
}

static int
private_client_client_packet (LISerServer* server,
                              LISerClient* client,
                              LIArcReader* reader)
{
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t w;
	uint8_t flags;
	LIMatQuaternion tmp;

	if (((uint8_t*) reader->buffer)[0] == LINET_CLIENT_PACKET_MOVE)
	{
		if (!liarc_reader_get_uint8 (reader, &flags) || (flags & ~LI_CONTROL_MASK) ||
			!liarc_reader_get_int8 (reader, &x) ||
			!liarc_reader_get_int8 (reader, &y) ||
			!liarc_reader_get_int8 (reader, &z) ||
			!liarc_reader_get_int8 (reader, &w) ||
			!liarc_reader_check_end (reader))
			return 1;
		tmp = limat_quaternion_init (x / 127.0f, y / 127.0f, z / 127.0f, w / 127.0f);
		lical_callbacks_call (server->callbacks, server->engine, "client-control", lical_marshal_DATA_PTR_PTR_INT, client->object, &tmp, flags);
	}

	return 1;
}

static int
private_client_vision_hide (LISerServer* server,
                            LIEngObject* object,
                            LIEngObject* target)
{
	LIArcWriter* writer;

	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_DESTROY);
	if (writer != NULL)
	{
		liarc_writer_append_uint32 (writer, target->id);
		liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

static int
private_client_vision_show (LISerServer* server,
                            LIEngObject* object,
                            LIEngObject* target)
{
	int engflags;
	int netflags;
	LIAlgU32dicIter iter;
	LIArcWriter* writer;
	LIMatTransform transform;
	LIMatVector velocity;
	LISerAniminfo* animation;

	/* Create standard vision message. */
	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_CREATE);
	if (writer != NULL)
	{
		/* Translate flags. */
		netflags = 0;
		engflags = lieng_object_get_flags (target);
		if (engflags & LIENG_OBJECT_FLAG_DYNAMIC)
			netflags |= LINET_OBJECT_FLAG_DYNAMIC;

		/* Send creation packet. */
		lieng_object_get_transform (target, &transform);
		lieng_object_get_velocity (target, &velocity);
		liarc_writer_append_uint32 (writer, target->id);
		liarc_writer_append_uint16 (writer, lieng_object_get_model_code (target));
		liarc_writer_append_uint8 (writer, netflags);
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.x));
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.y));
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.z));
		liarc_writer_append_int8 (writer, (int8_t)(127 * transform.rotation.w));
		liarc_writer_append_float (writer, velocity.x);
		liarc_writer_append_float (writer, velocity.y);
		liarc_writer_append_float (writer, velocity.z);
		liarc_writer_append_float (writer, transform.position.x);
		liarc_writer_append_float (writer, transform.position.y);
		liarc_writer_append_float (writer, transform.position.z);
		liarc_writer_append_uint8 (writer, LISER_OBJECT (target)->animations->size);
		LI_FOREACH_U32DIC (iter, LISER_OBJECT (target)->animations)
		{
			animation = iter.value;
			liarc_writer_append_uint16 (writer, animation->animation->id);
			liarc_writer_append_uint8 (writer, animation->channel);
			liarc_writer_append_uint8 (writer, animation->permanent);
			liarc_writer_append_float (writer, animation->priority);
		}
		liser_client_send (LISER_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

static int
private_object_free (LISerServer* server,
                     LIEngObject* object)
{
	LIAlgU32dicIter iter;
	LISerObject* data = LISER_OBJECT (object);

	/* Free client. */
	liser_object_disconnect (object);

	/* Unrealize before server data is freed. */
	lieng_object_set_realized (object, 0);

	/* Free server data. */
	if (data != NULL)
	{
		LI_FOREACH_U32DIC (iter, data->animations)
			lisys_free (iter.value);
		lialg_u32dic_free (data->animations);
		lisys_free (data);
	}

	return 1;
}

static int
private_object_new (LISerServer* server,
                    LIEngObject* object)
{
	LISerObject* data;

	/* Allocate server data. */
	data = lisys_calloc (1, sizeof (LISerObject));
	if (data == NULL)
		return 0;
	data->server = server;
	data->animations = lialg_u32dic_new ();
	if (data->animations == NULL)
	{
		lisys_free (data);
		return 0;
	}

	/* Allocate script data. */
	object->script = liscr_data_new (server->script, object, LISCR_SCRIPT_OBJECT, lieng_object_free);
	if (object->script == NULL)
	{
		lialg_u32dic_free (data->animations);
		lisys_free (data);
		return 0;
	}
	liscr_data_unref (object->script, NULL);

	/* Extend engine object. */
	lieng_object_set_userdata (object, data);
	liphy_object_set_userdata (object->physics, object);

	return 1;
}

static int
private_sector_load (LISerServer* server,
                     LIEngSector* sector)
{
	int id;
	int ret;
	const char* query;
	LIEngObject* object;
	sqlite3_stmt* statement;

	id = sector->sector->index;

	/* Prepare statement. */
	query = "SELECT id FROM objects WHERE sector=?;";
	if (sqlite3_prepare_v2 (server->sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (server->sql));
		return 1;
	}
	if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (server->sql));
		sqlite3_finalize (statement);
		return 1;
	}

	/* Execute statement. */
	while (1)
	{
		ret = sqlite3_step (statement);
		if (ret == SQLITE_DONE)
		{
			sqlite3_finalize (statement);
			return 1;
		}
		if (ret != SQLITE_ROW)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (server->sql));
			sqlite3_finalize (statement);
			return 1;
		}
		id = sqlite3_column_int (statement, 0);
		object = lieng_object_new (server->engine, NULL, LIPHY_CONTROL_MODE_RIGID, id);
		if (object != NULL)
		{
			if (liser_object_serialize (object, 0))
				lieng_object_set_realized (object, 1);
		}
	}
}

int
liser_server_init_callbacks_client (LISerServer* server)
{
	lical_callbacks_insert (server->callbacks, server->engine, "client-login", 0, private_client_client_login, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "client-packet", 0, private_client_client_packet, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "vision-hide", 0, private_client_vision_hide, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "vision-show", 0, private_client_vision_show, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "object-free", 65535, private_object_free, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "object-new", -65535, private_object_new, server, NULL);
	lical_callbacks_insert (server->callbacks, server->engine, "sector-load", -65535, private_sector_load, server, NULL);
	return 1;
}

/** @} */
/** @} */
