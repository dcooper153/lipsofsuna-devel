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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvCallbacks Callbacks
 * @{
 */

#include <network/lips-network.h>
#include "server-callbacks.h"
#include "server-observer.h"
#include "server-script.h"

static int
private_client_client_login (lisrvServer* server,
                             liengObject* object,
                             const char*  name,
                             const char*  pass)
{
#warning No accounts
#if 0
	char* path;
	licfgAccount* account;

	/* Contruct the account path. */
	path = lisys_path_concat (self->paths->server_state, "accounts", message->NEW_USER.name, NULL);
	if (path == NULL)
	{
		grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
		free (pass);
		return 1;
	}

	/* Parse the account file. */
	account = licfg_account_new (path);
	free (path);
	if (account == NULL)
	{
		if (lisys_error_peek () != EIO)
		{
			grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
			free (pass);
			return 1;
		}
		if (!self->config.server->enable_create_account)
		{
			grapple_server_disconnect_client (self->network.server, message->NEW_USER.id);
			free (pass);
			return 1;
		}
	}

	/* Set credentials. */
	lisrv_client_set_account (client, message->NEW_USER.name);
	if (pass != NULL)
		lisrv_client_set_password (client, pass);
#endif

	return 1;
}

static int
private_client_client_packet (lisrvServer* server,
                              lisrvClient* client,
                              liReader*    reader)
{
	int8_t x;
	int8_t y;
	int8_t z;
	int8_t w;
	uint8_t flags;
	limatQuaternion tmp;

	if (((uint8_t*) reader->buffer)[0] == LINET_CLIENT_PACKET_MOVE)
	{
		if (!li_reader_get_uint8 (reader, &flags) || (flags & ~LI_CONTROL_MASK) ||
			!li_reader_get_int8 (reader, &x) ||
			!li_reader_get_int8 (reader, &y) ||
			!li_reader_get_int8 (reader, &z) ||
			!li_reader_get_int8 (reader, &w) ||
			!li_reader_check_end (reader))
			return 1;
		tmp = limat_quaternion_init (x / 127.0f, y / 127.0f, z / 127.0f, w / 127.0f);
		lieng_engine_call (server->engine, LISRV_CALLBACK_CLIENT_CONTROL, client->object, &tmp, flags);
	}

	return 1;
}

static int
private_client_vision_hide (lisrvServer* server,
                            liengObject* object,
                            liengObject* target)
{
	liarcWriter* writer;

	writer = liarc_writer_new_packet (LINET_SERVER_PACKET_OBJECT_DESTROY);
	if (writer != NULL)
	{
		liarc_writer_append_uint32 (writer, target->id);
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

static int
private_client_vision_show (lisrvServer* server,
                            liengObject* object,
                            liengObject* target)
{
	int engflags;
	int netflags;
	lialgU32dicIter iter;
	liarcWriter* writer;
	limatTransform transform;
	limatVector velocity;
	lisrvAniminfo* animation;

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
		liarc_writer_append_uint8 (writer, LISRV_OBJECT (target)->animations->size);
		LI_FOREACH_U32DIC (iter, LISRV_OBJECT (target)->animations)
		{
			animation = iter.value;
			liarc_writer_append_uint16 (writer, animation->animation->id);
			liarc_writer_append_uint8 (writer, animation->channel);
			liarc_writer_append_uint8 (writer, animation->permanent);
			liarc_writer_append_float (writer, animation->priority);
		}
		lisrv_client_send (LISRV_OBJECT (object)->client, writer, GRAPPLE_RELIABLE);
		liarc_writer_free (writer);
	}

	return 1;
}

int lisrv_server_init_callbacks_client (lisrvServer* server)
{
	lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_LOGIN, 0, private_client_client_login, server, NULL);
	lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_PACKET, 0, private_client_client_packet, server, NULL);
	lieng_engine_insert_call (server->engine, LISRV_CALLBACK_VISION_HIDE, 0, private_client_vision_hide, server, NULL);
	lieng_engine_insert_call (server->engine, LISRV_CALLBACK_VISION_SHOW, 0, private_client_vision_show, server, NULL);
	return 1;
}

/** @} */
/** @} */
